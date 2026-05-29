#!/usr/bin/env bash
# Flash Goose Eye firmware to a connected XIAO ESP32-S3 (auto-detect USB port).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/firmware"

if ! command -v pio >/dev/null 2>&1; then
  echo "PlatformIO (pio) not found. Install: https://platformio.org/install/cli"
  exit 1
fi

PORT="${UPLOAD_PORT:-}"
if [ -z "$PORT" ]; then
  if [[ "$(uname -s)" == "Darwin" ]]; then
    PORT="$(ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true)"
  else
    PORT="$(ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null | head -1 || true)"
  fi
fi
if [ -z "$PORT" ]; then
  echo "No ESP32 USB port found. Plug in the XIAO via USB-C and retry."
  echo "Override: UPLOAD_PORT=/dev/cu.usbmodemXXXX ./scripts/flash-firmware.sh"
  exit 1
fi

FW_MAJOR=$(grep 'FIRMWARE_VERSION_MAJOR' platformio.ini | grep -o '[0-9]*')
FW_MINOR=$(grep 'FIRMWARE_VERSION_MINOR' platformio.ini | grep -o '[0-9]*')
FW_PATCH=$(grep 'FIRMWARE_VERSION_PATCH' platformio.ini | grep -o '[0-9]*')
FW_VERSION="${FW_MAJOR}.${FW_MINOR}.${FW_PATCH}"

echo "Uploading firmware v${FW_VERSION} to $PORT ..."
pio run -t upload --upload-port "$PORT"
echo "Done. Device should advertise as GooseEye (or your configured name)."
