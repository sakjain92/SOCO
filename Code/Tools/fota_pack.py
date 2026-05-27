#!/usr/bin/env python3
"""
Inf: Packs an IAR-produced Intel HEX file into an encrypted FOTA binary
     for SOCO firmware upgrade over Modbus.

     Strips the bootloader (first 8KB) and the last two flash pages
     (FOTA metadata + calibration), encrypts the application firmware
     with a seeded 16-bit XOR PRNG, prepends a 6-byte header, and writes
     the result to a .bin file.

Inp: --file      Path to the Intel HEX file (e.g. DIN Rail Meter.hex)
     --version   Firmware version in X.YZ format (e.g. 2.16)
     --output-dir Directory for the output file (default: current dir)

Ret: SOCO_T1_<X.YZ>.bin in the output directory

Usage:
    python3 fota_pack.py --file "Project/Debug/Exe/DIN Rail Meter.hex" --version 2.16
    python3 fota_pack.py --file firmware.hex --version 2.16 --output-dir /tmp
"""

import argparse
import os
import re
import struct
import sys


# ---------------------------------------------------------------------------
# Constants — must match FlagDef_DIN.h exactly
# ---------------------------------------------------------------------------

FLASH_BASE          = 0x08000000
BOOTLOADER_SIZE     = 0x2000        # 8KB (pages 0-3).
                                    # NOTE: the bootloader currently uses ~1.7 KB
                                    # so this can be shrunk to 0x1000 (4 KB) to
                                    # reclaim 4 KB for the app. If you do, keep
                                    # this in lockstep with BOOT_FLASH_SIZE /
                                    # BOOT_APP_ADDRESS in boot_defs.h and the
                                    # APP_ROM start address in Kwh373Meter.icf.
APP_START           = FLASH_BASE + BOOTLOADER_SIZE  # 0x08002000
APP_END             = 0x0801F000    # exclusive — page 62 (FOTA) and 63 (cal) excluded
FOTA_CHUNK_SIZE     = 200
FOTA_HEADER_SIZE    = 6
FOTA_SECRET_KEY     = 0x4A7B
FOTA_SEED_MIXER     = 0x9E37
FOTA_PRNG_MUL       = 25173
FOTA_PRNG_INC       = 13849
FOTA_MAX_FW_SIZE    = 96 * 1024     # EEPROM staging capacity (0x8000-0x20000).
                                    # NOTE: if a future firmware exceeds 96 KB we
                                    # can extend the staging window by reclaiming
                                    # free EEPROM at the lower end (0x0000-0x7FFF
                                    # currently holds runtime config/logs — audit
                                    # usage and lower FOTA_STAGING_START to grow
                                    # this). The flash app region is 116 KB
                                    # (0x08002000-0x0801EFFF) so flash is not the
                                    # bottleneck — EEPROM staging is.


# ---------------------------------------------------------------------------
# Intel HEX parser (no external dependencies)
# ---------------------------------------------------------------------------

def parse_hex_file(path):
    """
    Inf: Parse an Intel HEX file into a dict of {address: byte_value}.
    Inp: path - file path to the .hex file
    Ret: dict mapping absolute addresses to byte values
    """
    data = {}
    base_addr = 0

    with open(path, 'r') as f:
        for line_no, line in enumerate(f, 1):
            line = line.strip()
            if not line.startswith(':'):
                continue

            if len(line) < 11:
                raise ValueError("Line %d: too short" % line_no)

            try:
                raw = bytes.fromhex(line[1:])
            except ValueError:
                raise ValueError("Line %d: invalid hex characters" % line_no)

            byte_count = raw[0]
            address    = (raw[1] << 8) | raw[2]
            rec_type   = raw[3]
            payload    = raw[4 : 4 + byte_count]
            checksum   = raw[4 + byte_count]

            # Verify checksum
            calc_sum = sum(raw[:-1]) & 0xFF
            if (calc_sum + checksum) & 0xFF != 0:
                raise ValueError("Line %d: checksum mismatch" % line_no)

            if len(payload) != byte_count:
                raise ValueError("Line %d: byte count mismatch" % line_no)

            if rec_type == 0x00:    # Data record
                for i, b in enumerate(payload):
                    data[base_addr + address + i] = b
            elif rec_type == 0x01:  # EOF
                break
            elif rec_type == 0x02:  # Extended segment address
                base_addr = ((payload[0] << 8) | payload[1]) << 4
            elif rec_type == 0x04:  # Extended linear address
                base_addr = ((payload[0] << 8) | payload[1]) << 16
            # Skip other record types (0x03, 0x05)

    return data


def extract_region(hex_data, start, end):
    """
    Inf: Extract a contiguous byte region from parsed hex data.
         Missing bytes within the range are filled with 0xFF (erased flash).
    Inp: hex_data - dict from parse_hex_file
         start    - first address (inclusive)
         end      - last address (exclusive)
    Ret: bytearray of length (end - start)
    """
    size = end - start
    buf = bytearray(b'\xFF' * size)
    for addr, val in hex_data.items():
        if start <= addr < end:
            buf[addr - start] = val
    return buf


# ---------------------------------------------------------------------------
# CRC-16 — matches CRCCalculation() in DataManagment.c
# ---------------------------------------------------------------------------

def crc16(data_bytes, seed=0xFFFF):
    """
    Inf: CRC-16 over raw bytes, matching the SOCO CRCCalculation() which
         processes uint16_t words (little-endian) with polynomial 0xA001.
    Inp: data_bytes - bytes/bytearray, length must be even
         seed       - initial CRC value
    Ret: 16-bit CRC value
    """
    if len(data_bytes) % 2 != 0:
        raise ValueError("CRC input length must be even (got %d)" % len(data_bytes))

    crc = seed
    for i in range(0, len(data_bytes), 2):
        word = data_bytes[i] | (data_bytes[i + 1] << 8)  # LE uint16_t
        crc ^= word
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc


# ---------------------------------------------------------------------------
# PRNG encryption — matches Comm.c decrypt loop
# ---------------------------------------------------------------------------

def encrypt(plaintext, seed):
    """
    Inf: Encrypt (or decrypt) firmware using 16-bit LCG XOR.
    Inp: plaintext - bytearray, length must be even
         seed      - 16-bit PRNG seed
    Ret: bytearray of encrypted data (same length)
    """
    if len(plaintext) % 2 != 0:
        raise ValueError("Plaintext length must be even (got %d)" % len(plaintext))

    state = seed
    out = bytearray(len(plaintext))

    for i in range(0, len(plaintext), 2):
        state = (state * FOTA_PRNG_MUL + FOTA_PRNG_INC) & 0xFFFF
        word = plaintext[i] | (plaintext[i + 1] << 8)  # LE uint16_t
        enc = word ^ state
        out[i]     = enc & 0xFF
        out[i + 1] = (enc >> 8) & 0xFF

    return out


# ---------------------------------------------------------------------------
# Version parsing
# ---------------------------------------------------------------------------

def parse_version(version_str):
    """
    Inf: Parse version string "X.YZ" into integer X*100 + YZ.
    Inp: version_str - string like "2.16"
    Ret: integer version (e.g. 216)
    """
    m = re.match(r'^(\d+)\.(\d{2})$', version_str)
    if not m:
        raise ValueError(
            "Version must be in X.YZ format with exactly two decimal digits "
            "(e.g. 2.16), got '%s'" % version_str)
    major = int(m.group(1))
    minor = int(m.group(2))
    ver = major * 100 + minor
    if ver == 0:
        raise ValueError("Version 0.00 is not allowed")
    return ver


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Pack SOCO firmware HEX into encrypted FOTA binary.")
    parser.add_argument("--file", required=True,
                        help="Path to IAR Intel HEX file.")
    parser.add_argument("--version", required=True,
                        help="Firmware version in X.YZ format (e.g. 2.16).")
    parser.add_argument("--output-dir", default=".",
                        help="Output directory (default: current directory).")
    args = parser.parse_args()

    # --- Validate inputs ---

    if not os.path.isfile(args.file):
        sys.exit("Error: file not found: %s" % args.file)

    try:
        version_int = parse_version(args.version)
    except ValueError as e:
        sys.exit("Error: %s" % e)

    if not os.path.isdir(args.output_dir):
        sys.exit("Error: output directory does not exist: %s" % args.output_dir)

    # --- Parse HEX ---

    print("Parsing %s ..." % args.file)
    try:
        hex_data = parse_hex_file(args.file)
    except ValueError as e:
        sys.exit("Error: invalid HEX file: %s" % e)

    if not hex_data:
        sys.exit("Error: HEX file contains no data")

    min_addr = min(hex_data.keys())
    max_addr = max(hex_data.keys())
    print("  HEX range: 0x%08X - 0x%08X (%d bytes)"
          % (min_addr, max_addr, max_addr - min_addr + 1))

    # --- Validate HEX content ---

    if min_addr < FLASH_BASE:
        sys.exit("Error: HEX contains data below flash base (0x%08X)" % FLASH_BASE)

    # Check that app region has data
    app_addrs = [a for a in hex_data.keys() if APP_START <= a < APP_END]
    if not app_addrs:
        sys.exit("Error: HEX contains no data in application region "
                 "(0x%08X - 0x%08X)" % (APP_START, APP_END))

    # --- Extract application firmware ---

    # Find the actual end of code (last non-0xFF byte) to avoid trailing padding
    app_data = extract_region(hex_data, APP_START, APP_END)
    actual_end = len(app_data)
    while actual_end > 0 and app_data[actual_end - 1] == 0xFF:
        actual_end -= 1

    if actual_end == 0:
        sys.exit("Error: application region is empty (all 0xFF)")

    # Round up to even for uint16_t alignment
    if actual_end % 2 != 0:
        actual_end += 1

    firmware = app_data[:actual_end]

    print("  Application: 0x%08X - 0x%08X (%d bytes)"
          % (APP_START, APP_START + len(firmware), len(firmware)))

    # --- Pad to fill complete records ---

    # Total data in file = HEADER + firmware. Must be multiple of FOTA_CHUNK_SIZE.
    total_data = FOTA_HEADER_SIZE + len(firmware)
    pad_needed = (-total_data) % FOTA_CHUNK_SIZE
    if pad_needed:
        firmware += b'\x00' * pad_needed
        print("  Padded %d bytes to fill last record" % pad_needed)

    total_data = FOTA_HEADER_SIZE + len(firmware)
    total_records = total_data // FOTA_CHUNK_SIZE
    assert total_data % FOTA_CHUNK_SIZE == 0

    if len(firmware) > FOTA_MAX_FW_SIZE:
        sys.exit("Error: firmware (%d bytes) exceeds EEPROM staging capacity "
                 "(%d bytes)" % (len(firmware), FOTA_MAX_FW_SIZE))

    # --- Compute CRC of plaintext firmware ---

    plaintext_crc = crc16(firmware)

    # --- Encrypt ---

    seed = (FOTA_SECRET_KEY ^ (version_int * FOTA_SEED_MIXER)) & 0xFFFF
    encrypted = encrypt(firmware, seed)

    # --- Build header (little-endian) ---

    header = struct.pack('<HHH', total_records, version_int, plaintext_crc)
    assert len(header) == FOTA_HEADER_SIZE

    # --- Write output ---

    output_name = "SOCO_T1_%s.bin" % args.version
    output_path = os.path.join(args.output_dir, output_name)

    with open(output_path, 'wb') as f:
        f.write(header)
        f.write(encrypted)

    output_size = os.path.getsize(output_path)

    print()
    print("Output: %s" % output_path)
    print("  Version:       v%s (encoded %d)" % (args.version, version_int))
    print("  Firmware:      %d bytes (plaintext, padded)" % len(firmware))
    print("  Records:       %d (of %d bytes each)" % (total_records, FOTA_CHUNK_SIZE))
    print("  Plaintext CRC: 0x%04X" % plaintext_crc)
    print("  PRNG seed:     0x%04X" % seed)
    print("  File size:     %d bytes" % output_size)


if __name__ == "__main__":
    main()
