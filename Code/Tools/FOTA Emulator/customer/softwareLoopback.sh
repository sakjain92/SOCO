#!/bin/bash
#
# Creates a pair of linked PTYs so the FOTA client and server can talk to
# each other without any physical USB-to-RS485 hardware.
#
# Example:
#   sudo ./softwareLoopback.sh /dev/ttyS10 /dev/ttyS11
#
# Then run the server with --port /dev/ttyS11 and the client with
# --port /dev/ttyS10. Leave this script running until the test ends.
#
# Requires socat (sudo apt install socat).

port1="${1:-/dev/ttyS10}"
port2="${2:-/dev/ttyS11}"

if ! command -v socat >/dev/null 2>&1; then
    echo "Error: socat is not installed. Install with 'sudo apt install socat'." >&2
    exit 1
fi

echo "Keep this script running until test ends"

# mode=666 makes the PTYs world-readable/writable so the unprivileged
# soco_fota_server.py / soco_fota_client.py processes can open them
# without a separate chmod step.
#
exec sudo socat PTY,link="$port1",mode=666 PTY,link="$port2",mode=666
