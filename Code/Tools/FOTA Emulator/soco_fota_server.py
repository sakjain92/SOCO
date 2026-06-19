#!/usr/bin/env python3
"""
soco_fota_server.py - Modbus RTU slave that emulates a SOCO device's FOTA
behaviour so the TEMS-side client (soco_fota_client.py) can be exercised
end to end without real hardware. Faithful to the firmware's Modbus
*interface* (wire register addresses, status state machine, 0x15 record
sequence) in Code/Common/Comm.c.

This emulator is deliberately agnostic to the firmware file's contents: it
never inspects the record payload, never reads the header, never checks the
CRC, and never decrypts anything. It only drives the Modbus transfer
sequence. The version it reports after a successful upgrade is supplied on
the command line (--upgrade-version), NOT derived from the file. Consequently
the shipped binary carries no firmware-protection secrets to reverse
engineer; image validation is the real device's job.

Modbus interface mirrored from the firmware:
  * Read Holding Registers (0x03) at 50006 -> firmware version (Int32, x100).
  * Read/Write (0x03 / 0x10) at 40000 -> FOTA status state machine:
      - write 1: reset the transfer state and set status = READY (1).
      - write 2: accept the transfer, simulate flash + reboot, then report
                 the --upgrade-version.
  * Write File Record (0x15): file 1, refType 0x06, 100 registers (200 bytes)
    per record, sequential record numbers from 0, with a re-ack of the
    previous record (recordNo == received-1) to survive lost responses. Only
    the frame fields are validated; the 200-byte payload is ignored.

Usage:
    python3 soco_fota_server.py --port /dev/ttyS11 --version 2.13 --upgrade-version 2.21
"""
import argparse
import struct
import sys
import time

try:
    import serial
except ImportError:
    sys.exit("Error: pyserial is required (pip install pyserial).")

# --- Structural protocol constants (not secret) ---
FOTA_CHUNK_SIZE = 200
FOTA_MAX_CHUNKS = (96 * 1024) // FOTA_CHUNK_SIZE   # 491 (EEPROM staging cap)

# Wire register addresses - ground truth from the firmware (Comm.c).
REG_VERSION = 50006
REG_STATUS = 40000

FOTA_STATUS_IDLE = 0
FOTA_STATUS_READY = 1
FOTA_STATUS_ERROR = 127

# Simulated flash-erase+program+reboot window; the old version is still
# reported during it (matching the client's "old version = still flashing").
FLASH_REBOOT_DELAY_SEC = 3.0


def modbus_crc(data):
    """Standard Modbus RTU CRC-16 (poly 0xA001) for frame integrity - the
    public Modbus framing CRC, unrelated to any firmware-image checksum."""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if (crc & 1) else (crc >> 1)
    return crc


def frame_total_len(buf):
    """Expected RTU frame length from the function code, or None if not yet
    determinable."""
    if len(buf) < 2:
        return None
    func = buf[1]
    if func == 0x03:
        return 8
    if func == 0x10:
        if len(buf) < 7:
            return None
        return 9 + buf[6]
    if func == 0x15:
        if len(buf) < 3:
            return None
        return 5 + buf[2]
    return None


class FotaEmulator:
    def __init__(self, slave_id, version_int, upgrade_version_int, debug=False):
        self.slave_id = slave_id
        self.version = version_int                 # reported now
        self.upgrade_version = upgrade_version_int  # reported after write 2
        self.debug = debug
        self.status = FOTA_STATUS_IDLE
        self.pending_version = None
        self.upgrade_at = None
        self.chunks_received = 0

    def log(self, *a):
        if self.debug:
            print("[server]", *a, file=sys.stderr)

    # -- response builders -------------------------------------------------
    def _send(self, ser, payload):
        crc = modbus_crc(payload)
        ser.write(bytes(payload) + bytes([crc & 0xFF, (crc >> 8) & 0xFF]))
        ser.flush()

    def _exc(self, ser, func, code):
        self.log("exception func=%#x code=%d" % (func, code))
        self._send(ser, bytes([self.slave_id, func | 0x80, code]))

    def _read_int32_resp(self, ser, value):
        self._send(ser, bytes([self.slave_id, 0x03, 4]) + struct.pack(">I", value & 0xFFFFFFFF))

    def _current_version(self):
        if self.pending_version is not None and \
           (time.monotonic() - self.upgrade_at) >= FLASH_REBOOT_DELAY_SEC:
            self.version = self.pending_version
            self.pending_version = None
            self.log("reboot complete; now reporting v%d" % self.version)
        return self.version

    # -- function-code handlers --------------------------------------------
    def handle(self, ser, frame):
        if modbus_crc(frame[:-2]) != (frame[-2] | (frame[-1] << 8)):
            self.log("bad CRC, ignoring")
            return
        if frame[0] != self.slave_id:
            return
        func = frame[1]
        if func == 0x03:
            self._h_read(ser, frame)
        elif func == 0x10:
            self._h_write(ser, frame)
        elif func == 0x15:
            self._h_record(ser, frame)
        else:
            self._exc(ser, func, 0x01)

    def _h_read(self, ser, frame):
        start = (frame[2] << 8) | frame[3]
        qty = (frame[4] << 8) | frame[5]
        if start == REG_VERSION and qty == 2:
            self._read_int32_resp(ser, self._current_version())
        elif start == REG_STATUS and qty == 2:
            self._read_int32_resp(ser, self.status)
        else:
            self._exc(ser, 0x03, 0x02)

    def _h_write(self, ser, frame):
        start = (frame[2] << 8) | frame[3]
        qty = (frame[4] << 8) | frame[5]
        bytecount = frame[6]
        if not (start == REG_STATUS and qty == 2 and bytecount == 4):
            self._exc(ser, 0x10, 0x02)
            return
        value = struct.unpack(">I", frame[7:11])[0]
        if value == 1:
            self.chunks_received = 0
            self.status = FOTA_STATUS_READY
            self.log("request transfer -> status READY")
            self._send(ser, bytes([self.slave_id, 0x10]) + frame[2:6])
        elif value == 2:
            if not (self.status == FOTA_STATUS_READY and self.chunks_received > 0):
                self.status = FOTA_STATUS_ERROR
                self.log("upgrade rejected (status=%d chunks=%d)" % (self.status, self.chunks_received))
                self._exc(ser, 0x10, 0x04)
                return
            self.log("upgrade accepted (%d records) -> reporting v%d after reboot"
                     % (self.chunks_received, self.upgrade_version))
            self.pending_version = self.upgrade_version
            self.upgrade_at = time.monotonic()
            self.status = FOTA_STATUS_IDLE
            self._send(ser, bytes([self.slave_id, 0x10]) + frame[2:6])
        else:
            self._exc(ser, 0x10, 0x03)

    def _h_record(self, ser, frame):
        # Only the frame fields are validated; the 200-byte payload at
        # frame[10:210] is intentionally never inspected.
        data_len = frame[2]
        ref_type = frame[3]
        file_no = (frame[4] << 8) | frame[5]
        record_no = (frame[6] << 8) | frame[7]
        rec_len = (frame[8] << 8) | frame[9]

        is_re_ack = self.chunks_received > 0 and record_no == self.chunks_received - 1
        if (ref_type != 0x06 or file_no != 1 or rec_len != 100 or data_len != 207 or
                self.status != FOTA_STATUS_READY or record_no >= FOTA_MAX_CHUNKS or
                (record_no != self.chunks_received and not is_re_ack)):
            self._exc(ser, 0x15, 0x02)
            return

        if is_re_ack:
            self.log("re-ack record %d" % record_no)
            self._send(ser, frame[:-2])
            return

        self.chunks_received += 1
        self._send(ser, frame[:-2])


def main():
    p = argparse.ArgumentParser(description="Emulates a SOCO Modbus RTU slave with FOTA support.")
    p.add_argument("--port", required=True, help="Serial port (e.g. /dev/ttyS11).")
    p.add_argument("--id", type=int, default=1, help="Modbus slave ID. Default: 1.")
    p.add_argument("--baud", type=int, default=19200, help="Baudrate. Default: 19200.")
    p.add_argument("--parity", choices=["N", "E", "O"], default="N", help="Parity. Default: N.")
    p.add_argument("--stopbits", choices=["one", "two"], default="one", help="Stop bits. Default: one.")
    p.add_argument("--timeout", type=float, default=2, help="Serial read timeout, seconds.")
    p.add_argument("--version", default="2.13", help="Initial firmware version (X.YZ). Default: 2.13.")
    p.add_argument("--upgrade-version", default=None,
                   help="Version (X.YZ) to report after a successful upgrade. "
                        "Set this to the version of the firmware you are flashing. "
                        "Defaults to --version (no version change).")
    p.add_argument("--debug", action="store_true", help="Verbose logging.")
    args = p.parse_args()

    import re

    def parse_ver(s):
        m = re.match(r"^(\d+)\.(\d{2})$", s)
        if not m:
            sys.exit("Error: version must be X.YZ (e.g. 2.13), got '%s'" % s)
        return int(m.group(1)) * 100 + int(m.group(2))

    version_int = parse_ver(args.version)
    upgrade_int = parse_ver(args.upgrade_version) if args.upgrade_version else version_int

    ser = serial.Serial(
        port=args.port, baudrate=args.baud,
        parity={"N": serial.PARITY_NONE, "E": serial.PARITY_EVEN, "O": serial.PARITY_ODD}[args.parity],
        stopbits=serial.STOPBITS_ONE if args.stopbits == "one" else serial.STOPBITS_TWO,
        bytesize=serial.EIGHTBITS, timeout=0.2)

    emu = FotaEmulator(args.id, version_int, upgrade_int, debug=args.debug)
    print("SOCO FOTA emulator on %s @ %d %s%d, slave %d, v%s -> v%d.%02d on upgrade "
          "(regs: version=%d status=%d)"
          % (args.port, args.baud, args.parity, 1 if args.stopbits == "one" else 2,
             args.id, args.version, upgrade_int // 100, upgrade_int % 100,
             REG_VERSION, REG_STATUS), flush=True)

    buf = bytearray()
    try:
        while True:
            chunk = ser.read(256)
            if chunk:
                buf += chunk
            while True:
                L = frame_total_len(buf)
                if L is None:
                    if len(buf) >= 2 and buf[1] not in (0x03, 0x10, 0x15):
                        del buf[0]
                        continue
                    break
                if len(buf) < L:
                    break
                frame = bytes(buf[:L])
                del buf[:L]
                emu.handle(ser, frame)
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()


if __name__ == "__main__":
    main()
