# SOCO FOTA Simulator

Tools for developing and testing TEMS-side firmware over-the-air (FOTA)
upgrade code without needing a physical SOCO controller. Includes a
ready-to-run SOCO emulator (binary), a reference TEMS-side client
(Python), sample firmware files, and a software loopback helper for
hardware-free testing.

## What's in this directory

| File                  | Role                                                |
|-----------------------|-----------------------------------------------------|
| `soco_fota_server`    | SOCO emulator. A self-contained Linux x86_64 binary that pretends to be a SOCO controller during a FOTA upgrade. |
| `soco_fota_client.py` | Reference TEMS-side client (Python). Drives a complete upgrade against `soco_fota_server` (or a real SOCO). |
| `SOCO_T1_2.17.bin`, `SOCO_T1_2.21.bin` | Sample packed firmware files ready to flash. |
| `SOCO_FOTA.png`       | Flowchart of the end-to-end FOTA upgrade sequence. |
| `softwareLoopback.sh` | Optional helper that creates a pair of linked virtual serial ports so the client and server can talk without RS-485 hardware. |

## How it fits in

The SOCO FOTA upgrade flow is a 3-tier system:

```
[Customer file server] --HTTP/SFTP--> [TEMS] --Modbus RTU/RS-485--> [SOCO]
```

`soco_fota_server` plays the **right-hand box** — a SOCO controller. It
speaks Modbus RTU as a slave: reports its current firmware version,
receives a firmware file in chunks, simulates flashing, and reports
the configured upgrade version after a brief delay.

`soco_fota_client.py` plays the **middle box** — TEMS. It reads the
current version, requests a transfer, sends the firmware file
record-by-record, requests an upgrade, and polls until the new
version is reported.

Run them on opposite ends of the same Modbus segment (loopback or real
RS-485) to exercise the full upgrade flow end-to-end.

## Prerequisites

- Linux x86_64
- Python 3.8 or newer (for `soco_fota_client.py` only — the server is
  a standalone binary)
- `socat`, only if you want to use the software loopback helper

```
sudo apt install python3 python3-pip python3-venv socat
```

For the client, install pinned dependencies. A virtualenv is
recommended so the pinned versions don't conflict with anything else
on the system:

```
python3 -m venv venv
source venv/bin/activate
pip install 'pymodbus==3.0.2' pyserial pyserial-asyncio
```

The pinned `pymodbus==3.0.2` is intentional — newer versions have
incompatible APIs for the `WriteFileRecord` hooks the client relies on.

## Quickstart (no hardware required)

Three terminals.

### Terminal 1 — software loopback

Creates a pair of linked virtual serial ports. Leave running for the
duration of the test.

```
./softwareLoopback.sh /dev/ttyS10 /dev/ttyS11
```

You'll be prompted for sudo (creating PTYs needs root). The script
sets the PTY permissions to `666` so the server and client can open
them without sudo themselves.

### Terminal 2 — start the SOCO emulator

```
./soco_fota_server                     \
    --port /dev/ttyS11                 \
    --id 1                             \
    --baud 19200                       \
    --parity N                         \
    --stopbits one                     \
    --version 2.13                     \
    --upgrade-version 2.21
```

The slave starts at v2.13 and listens for Modbus requests. After a
successful upgrade it reports `--upgrade-version` — set it to the
version of the firmware file you are flashing (here, `SOCO_T1_2.21.bin`).

### Terminal 3 — run the upgrade

(Activate the venv first if you set one up.)

```
python3 soco_fota_client.py            \
    --file SOCO_T1_2.21.bin            \
    --port /dev/ttyS10                 \
    --id 1                             \
    --baud 19200                       \
    --parity N                         \
    --stopbits one
```

You should see all four phases run in about 15 seconds: version
check, prepare, chunked transfer with progress bar, then
upgrade-and-poll with a ticking countdown until the slave reports the
new version.

## Using real RS-485 hardware

Skip the loopback. Plug USB-to-RS485 converters into the host, attach
the emulator and the client to opposite ends of the bus, and adjust
`--port` to match the real device names (e.g. `/dev/ttyUSB0`).

If you only have one converter, you can use it to drive a real SOCO
controller from the client — just point `--port` at the converter and
skip the server. Conversely, you can point your own TEMS-side code at
the emulator binary instead of the reference client.

## Tool reference

### `soco_fota_server`

```
./soco_fota_server          \
    --port /dev/ttyUSB0     \    # required
    --id 1                  \    # default 1
    --baud 19200            \    # default 19200
    --parity N              \    # N | E | O, default N
    --stopbits one          \    # one | two, default one
    --timeout 2             \    # seconds, default 2
    --version 2.13          \    # initial firmware version, default 2.13
    --upgrade-version 2.21  \    # version reported after a successful upgrade
    [--debug]                    # verbose protocol logging
```

Behaviour:

- Reports the firmware version on holding register `50007` (Int32,
  scaled by ×100; v2.13 is reported as `213`). It reports `--version`
  until an upgrade completes, then `--upgrade-version`.
- Hosts an upgrade-status state machine on holding register `40001`
  (Int32): `0` idle, `1` ready, `127` error.
- On a write of `1` to `40001`: resets the transfer state and sets
  the status to `1` (ready). The client still polls `40001` for `1`
  (its first read happens a couple of seconds after the write).
- Accepts firmware in 200-byte chunks via Modbus function `0x15`
  (Write File Record), file number `1`, reference type `0x06`,
  exactly 100 registers per record, sequential record numbers from
  `0`. Out-of-order or oversized records are rejected. The emulator
  does not inspect, decrypt, or checksum the record payload — image
  validation is the real device's job.
- On a write of `2` to `40001`: accepts the transfer, simulates a
  brief flash + reboot during which it still reports the old version,
  then reports `--upgrade-version` on `50007`.

### `soco_fota_client.py`

```
python3 soco_fota_client.py \
    --file <X.YZ.bin>       \    # required: packed firmware
    --port /dev/ttyS10      \    # required
    --id 1                  \    # default 1
    --baud 19200            \    # default 19200
    --parity N              \    # N | E | O
    --stopbits one          \
    --timeout 2             \
    [--debug]
```

The new version is parsed from the filename (`X.YZ.bin`). The client
refuses to flash if the new version is not strictly greater than the
slave's current version. Per-chunk transfer is attempted up to 3
times before aborting. After the upgrade is requested, the client
polls the version register for up to 120 s — if the slave reports a
version that is neither the old one nor the expected new one, the
client aborts immediately rather than wait for the timeout.

### `softwareLoopback.sh`

```
./softwareLoopback.sh [/dev/ttyS10] [/dev/ttyS11]
```

Creates a pair of linked virtual TTYs using `socat`. Defaults are
`/dev/ttyS10` and `/dev/ttyS11`. Both PTYs are created world-readable
and -writable so unprivileged processes can use them.

## Firmware files

Firmware files are produced and provided by the Procom team. Treat them
as opaque binaries — read the bytes, send them as described under
"Modbus protocol summary" below, and don't try to interpret or modify
the contents.

The filename always follows the pattern `X.YZ.bin` or
`SOCO_T1_X.YZ.bin` (e.g. `SOCO_T1_2.21.bin`). The reference client uses
this to derive the expected version after the upgrade. If you need a
firmware build for a new
version, contact the Procom team.

## Modbus protocol summary

| Action                  | Function | Address | Type   | Value / size                |
|-------------------------|----------|---------|--------|------------------------------|
| Read firmware version   | `0x03`   | 50007   | Int32  | version × 100               |
| Read upgrade status     | `0x03`   | 40001   | Int32  | state machine               |
| Request file transfer   | `0x10`   | 40001   | Int32  | write `1`                   |
| Send firmware chunk     | `0x15`   | n/a     | record | file=1, ref=0x06, 100 regs  |
| Request upgrade         | `0x10`   | 40001   | Int32  | write `2`                   |

The register addresses in this document are **1-based** (matching the
SOCO Modbus table). On the wire, a Modbus master must use the **0-based**
equivalents: version `50007` → `50006`, status `40001` → `40000`. The
reference `soco_fota_client.py` and `soco_fota_server` already use these
0-based wire addresses.

The full upgrade flow:

1. **Version check.** TEMS reads register `50007`. Confirms the new
   firmware version is greater than the slave's current version.
2. **Prepare.** TEMS writes `1` to register `40001`, then polls `40001`
   every ~2 s for up to 10 s, waiting for the value to read back as
   `1` (ready).
3. **Transfer.** TEMS pads the firmware file to a multiple of 200
   bytes with `0x00` and sends it record-by-record via function
   `0x15`. File number `1`, reference type `0x06`, 100 registers per
   record, record numbers sequential from `0`. Up to 3 attempts per
   record.
4. **Upgrade.** TEMS writes `2` to register `40001`. Polls `50007`
   every ~2 s for up to 120 s. During the simulated flash + reboot
   window the slave returns its old version (or briefly nothing); a
   version that is neither the old one nor the expected new one is a
   hard error.

## Troubleshooting

**`could not open port` / `permission denied`**

The user running the tools needs read/write access to the serial
port. With software loopback this is already handled (the script
chmods the PTYs to `666`). For real RS-485, either run with `sudo`,
add yourself to the `dialout` group
(`sudo usermod -a -G dialout $USER`, then log out and back in), or
chmod the device file directly.

**Client says `Aborting: SOCO did not become ready within 10s`**

The server is not responding to the status-register write. Check:
- Both ends of the loopback are linked (`ls -l /dev/ttyS10
  /dev/ttyS11` should show two distinct PTYs).
- Both sides agree on baud / parity / stop bits / slave ID.
- The server is running and printed its `SOCO FOTA emulator on ...`
  startup line before the client starts.

**Client gets stuck during chunked transfer**

Per-chunk retry is 3 attempts. If they all fail, the run aborts.
Re-run with `--debug` on both sides to see the raw frame exchange.
Common causes: baud mismatch, lost SOF on a noisy bus, slave-side
buffer overflow.

**Client says `SOCO reports v<X> after upgrade but client expected v<Y>`**

The slave came up reporting a version that doesn't match the one
encoded in the file's name. Most likely the firmware file was renamed
or you picked up the wrong file. Re-fetch the file from the Procom team
if it looks tampered with.

**`Filename '...' is not in X.YZ.bin form`**

The reference client extracts the new version from the filename. Pass
a file named like `2.21.bin` or `SOCO_T1_2.21.bin` — an optional
`SOCO_T1_` prefix, one or more digits, dot, two digits, `.bin`.

**Binary won't run: `cannot execute binary file: Exec format error`**

The binary is Linux x86_64 only. For ARM or other architectures,
contact the Procom team for a build targeting your platform.

**Binary won't run: `version GLIBC_2.XX not found`**

The build host had a newer glibc than the run host. Either upgrade
the run host or request a build on a host with an older glibc.
