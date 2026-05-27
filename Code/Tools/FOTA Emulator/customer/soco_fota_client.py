#!/usr/bin/env python3
"""
soco_fota_client.py - Modbus RTU master that drives a SOCO firmware
upgrade end-to-end. This is a reference implementation of the TEMS-side
behaviour

Flow:
  1. Read current firmware version from holding reg 50007.
  2. Parse the new version from the filename (X.YZ.bin).
  3. Reject the upgrade if new <= current.
  4. Write 1 to status reg 40001 (request file transfer).
  5. Poll reg 40001 every 2s for up to 10s, waiting for SOCO to report 1.
  6. Pad the file to a multiple of 200 bytes, send it record-by-record
     via Modbus function 0x15 (Write File Record). Up to 3 attempts
     per chunk.
  7. Write 2 to reg 40001 (request upgrade).
  8. Poll reg 50007 every 2s for up to 120s, waiting for the new version
     to appear. Treat any non-matching response (including timeouts) as
     'upgrade in progress'.

Example:

    python3 soco_fota_client.py --file /path/to/2.14.bin \\
        --port /dev/ttyS10 --id 1 --baud 19200 --parity N --stopbits one
"""

import argparse
import enum
import logging
import os
import re
import struct
import sys
import time

from importlib.metadata import (version as _pkg_version,
                                PackageNotFoundError)

try:
    _v = _pkg_version("pymodbus")
    if _v != "3.0.2":
        sys.exit("Error: pymodbus 3.0.2 is required (found %s). "
                 "Install with: pip install 'pymodbus==3.0.2'" % _v)
except PackageNotFoundError:
    sys.exit("Error: pymodbus 3.0.2 is not installed. "
             "Install with: pip install 'pymodbus==3.0.2'")

# pymodbus 3.0.2 soft-imports its serial backends and fails late with a
# confusing NameError when they are missing. Catch it early.
#
for _dep in ("pyserial", "pyserial-asyncio"):
    try:
        _pkg_version(_dep)
    except PackageNotFoundError:
        sys.exit("Error: %s is not installed (required by pymodbus). "
                 "Install with: pip install %s" % (_dep, _dep))

from pymodbus.client import ModbusSerialClient
from pymodbus.payload import BinaryPayloadBuilder, BinaryPayloadDecoder
from pymodbus.constants import Endian
from pymodbus import file_message as ModbusFileRequest


# ---------------------------------------------------------------------------
# Protocol constants - SOCO's Modbus FOTA interface.
# ---------------------------------------------------------------------------

REG_VERSION = 50007         # Int32, /100
REG_STATUS = 40001          # Int32

STATUS_TRANSFER = 1
STATUS_UPGRADE = 2
STATUS_READY = 1            # what SOCO reports when ready to receive

FILE_NUMBER = 1
REFERENCE_TYPE = 0x06
RECORD_SIZE_REGS = 100
RECORD_SIZE_BYTES = RECORD_SIZE_REGS * 2

# Per-phase timeouts.
#
PREPARE_POLL_INTERVAL_SEC = 2
PREPARE_TIMEOUT_SEC = 10
UPGRADE_POLL_INTERVAL_SEC = 2
UPGRADE_TIMEOUT_SEC = 120

CHUNK_RETRIES = 3

VERSION_FILENAME_RE = re.compile(r"^(\d+)\.(\d{2})\.bin$")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

class Parity(enum.Enum):
    none = 'N'
    even = 'E'
    odd = 'O'

    def __str__(self):
        return self.value


class Stopbits(enum.Enum):
    one = 1
    two = 2

    def __int__(self):
        return self.value

    def __str__(self):
        return self.name


def parse_args():
    parser = argparse.ArgumentParser(
        description="Drives a SOCO FOTA upgrade as a TEMS-side reference "
                    "client.")
    parser.add_argument("--file", required=True,
                        help="Path to the packed firmware file. Filename "
                             "must be 'X.YZ.bin' (e.g. 2.14.bin).")
    parser.add_argument("--port", required=True,
                        help="Serial port (e.g. /dev/ttyUSB0).")
    parser.add_argument("--id", type=int, default=1,
                        help="Modbus slave ID. Default: 1.")
    parser.add_argument("--baud", type=int, default=19200,
                        help="Baudrate. Default: 19200.")
    parser.add_argument("--parity", type=Parity, choices=list(Parity),
                        default=Parity.none, help="Parity. Default: N.")
    parser.add_argument("--stopbits", type=lambda v: Stopbits[v],
                        choices=list(Stopbits), default=Stopbits.one,
                        help="Stop bits. Default: one.")
    parser.add_argument("--timeout", type=int, default=2,
                        help="Per-request Modbus timeout, seconds. "
                             "Default: 2.")
    parser.add_argument("--debug", action="store_true",
                        help="Enable verbose pymodbus logging.")
    return parser.parse_args()


def version_from_filename(path):
    """'/path/to/2.13.bin' -> 213"""
    base = os.path.basename(path)
    m = VERSION_FILENAME_RE.match(base)
    if not m:
        raise ValueError("Filename '%s' is not in X.YZ.bin form" % base)
    return int(m.group(1)) * 100 + int(m.group(2))


def version_int_to_str(v):
    return "%d.%02d" % (v // 100, v % 100)


# ---------------------------------------------------------------------------
# Modbus primitives - thin wrappers that raise on error and decode Int32.
# ---------------------------------------------------------------------------

def read_int32(master, address, slave_id):
    result = master.read_holding_registers(address=address, count=2,
                                           unit=slave_id)
    if result.isError():
        raise IOError("Read at %d failed: %s" % (address, result))
    decoder = BinaryPayloadDecoder.fromRegisters(result.registers,
                                                 byteorder=Endian.Big,
                                                 wordorder=Endian.Big)
    return decoder.decode_32bit_int()


def write_int32(master, address, value, slave_id):
    builder = BinaryPayloadBuilder(byteorder=Endian.Big,
                                   wordorder=Endian.Big)
    builder.add_32bit_int(value)
    result = master.write_registers(address=address,
                                    values=builder.build(),
                                    skip_encode=True,
                                    unit=slave_id)
    if result.isError():
        raise IOError("Write at %d failed: %s" % (address, result))


def write_file_record(master, record_no, data, slave_id):
    """Send one 200-byte record via Modbus function 0x15."""
    if len(data) != RECORD_SIZE_BYTES:
        raise ValueError("Record data must be %d bytes (got %d)"
                         % (RECORD_SIZE_BYTES, len(data)))
    record = ModbusFileRequest.FileRecord(
        reference_type=REFERENCE_TYPE,
        file_number=FILE_NUMBER,
        record_number=record_no,
        record_data=data,
        record_length=RECORD_SIZE_REGS)
    request = ModbusFileRequest.WriteFileRecordRequest(records=[record],
                                                       unit=slave_id)
    result = master.execute(request)
    if result.isError():
        raise IOError("WriteFileRecord(rec=%d) failed: %s"
                      % (record_no, result))


# ---------------------------------------------------------------------------
# Phases of the upgrade flow
# ---------------------------------------------------------------------------

def phase_version_check(master, slave_id, new_version):
    print("Phase 1: version check")
    try:
        current = read_int32(master, REG_VERSION, slave_id)
    except IOError as e:
        raise SystemExit(
            "Aborting: could not read current version (%s). "
            "Is the server running and the port / baud / parity / "
            "stop-bit / slave-ID matched on both sides?" % e)
    print("  Current: v%s" % version_int_to_str(current))
    print("  New:     v%s" % version_int_to_str(new_version))
    if new_version <= current:
        raise SystemExit("Aborting: new version (%d) is not greater than "
                         "current (%d)" % (new_version, current))
    return current


def phase_prepare(master, slave_id):
    print("Phase 2: requesting file transfer")
    write_int32(master, REG_STATUS, STATUS_TRANSFER, slave_id)

    deadline = time.monotonic() + PREPARE_TIMEOUT_SEC
    print("  Polling status reg until SOCO reports ready (timeout %ds) ..."
          % PREPARE_TIMEOUT_SEC)

    while True:
        time.sleep(PREPARE_POLL_INTERVAL_SEC)
        try:
            status = read_int32(master, REG_STATUS, slave_id)
        except IOError as e:
            status = None
            print("  Status read failed (%s), retrying" % e)

        if status == STATUS_READY:
            print("  SOCO is ready")
            return

        if time.monotonic() > deadline:
            raise SystemExit("Aborting: SOCO did not become ready within "
                             "%ds (last status=%s)"
                             % (PREPARE_TIMEOUT_SEC, status))


def phase_transfer(master, slave_id, file_bytes):
    print("Phase 3: transferring %d bytes (%d records)"
          % (len(file_bytes),
             (len(file_bytes) + RECORD_SIZE_BYTES - 1) // RECORD_SIZE_BYTES))

    # Pad with 0x00 to a multiple of 200 bytes.
    #
    pad_len = (-len(file_bytes)) % RECORD_SIZE_BYTES
    padded = file_bytes + (b"\x00" * pad_len)
    num_records = len(padded) // RECORD_SIZE_BYTES

    last_pct = -1
    for record_no in range(num_records):
        chunk = padded[record_no * RECORD_SIZE_BYTES :
                       (record_no + 1) * RECORD_SIZE_BYTES]

        attempt = 0
        while True:
            try:
                write_file_record(master, record_no, chunk, slave_id)
                break
            except IOError as e:
                attempt += 1
                if attempt >= CHUNK_RETRIES:
                    # Newline first - otherwise the progress bar's \r
                    # line and the abort message run together.
                    #
                    print()
                    raise SystemExit("Aborting: record %d failed after %d "
                                     "attempts (last error: %s)"
                                     % (record_no, CHUNK_RETRIES, e))
                print()
                print("  Record %d attempt %d/%d failed (%s), retrying"
                      % (record_no, attempt, CHUNK_RETRIES, e))

        pct = int((record_no + 1) * 100 / num_records)
        if pct != last_pct:
            sys.stdout.write("\r  [%-50s] %d%% (%d/%d)"
                             % ("=" * (pct // 2),
                                pct, record_no + 1, num_records))
            sys.stdout.flush()
            last_pct = pct
    print()


def phase_perform(master, slave_id, old_version, new_version):
    print("Phase 4: requesting upgrade and waiting for SOCO to come back")
    write_int32(master, REG_STATUS, STATUS_UPGRADE, slave_id)

    deadline = time.monotonic() + UPGRADE_TIMEOUT_SEC
    print("  Polling version reg until SOCO reports v%s (timeout %ds) ..."
          % (version_int_to_str(new_version), UPGRADE_TIMEOUT_SEC))

    while True:
        time.sleep(UPGRADE_POLL_INTERVAL_SEC)

        # Three legitimate responses while SOCO flashes + reboots:
        #   - the new version (success),
        #   - the old version (still flashing / not yet rebooted),
        #   - no response (briefly during reboot).
        # Anything else means SOCO came up with the wrong firmware
        # version, which is a hard error - bail out immediately rather
        # than wait for the 120 s timeout.
        #
        try:
            current = read_int32(master, REG_VERSION, slave_id)
        except IOError:
            current = None

        elapsed = UPGRADE_TIMEOUT_SEC - max(0, deadline - time.monotonic())
        # %-15s pads / left-aligns to 15 chars so the line width is
        # constant whether 'current' is 'vX.YZ' (5 chars) or
        # '(no response)' (13 chars). Without this, switching from the
        # longer string to the shorter one leaves stale chars on the
        # terminal because the cursor only \r-rewinds to column 0.
        #
        sys.stdout.write("\r  t+%05.1fs  current=%-15s"
                         % (elapsed,
                            "v" + version_int_to_str(current)
                            if current is not None else "(no response)"))
        sys.stdout.flush()

        if current == new_version:
            print()
            print("  Version matches. Upgrade succeeded.")
            return

        if current is not None and current != old_version:
            print()
            raise SystemExit(
                "Aborting: SOCO reports v%s after upgrade but client "
                "expected v%s"
                % (version_int_to_str(current),
                   version_int_to_str(new_version)))

        if time.monotonic() > deadline:
            print()
            raise SystemExit("Aborting: SOCO did not report v%s within "
                             "%ds" % (version_int_to_str(new_version),
                                      UPGRADE_TIMEOUT_SEC))


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    args = parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    if not os.path.isfile(args.file):
        sys.exit("Error: file not found: %s" % args.file)

    try:
        new_version = version_from_filename(args.file)
    except ValueError as e:
        sys.exit("Error: %s" % e)

    with open(args.file, "rb") as f:
        file_bytes = f.read()

    print("=" * 60)
    print("SOCO FOTA Client (TEMS emulator)")
    print("=" * 60)
    print("File:        %s (%d bytes)" % (args.file, len(file_bytes)))
    print("New version: v%s (encoded %d)"
          % (version_int_to_str(new_version), new_version))
    print("Port:        %s, %d %s%s%d, slave %d"
          % (args.port, args.baud, "8", args.parity, int(args.stopbits),
             args.id))
    print("=" * 60)

    master = ModbusSerialClient(method='rtu',
                                port=args.port,
                                baudrate=args.baud,
                                stopbits=int(args.stopbits),
                                parity=str(args.parity),
                                timeout=args.timeout)
    if not master.connect():
        sys.exit("Error: failed to open %s" % args.port)

    try:
        old_version = phase_version_check(master, args.id, new_version)
        phase_prepare(master, args.id)
        phase_transfer(master, args.id, file_bytes)
        phase_perform(master, args.id, old_version, new_version)
        print()
        print("Done.")
    finally:
        master.close()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
