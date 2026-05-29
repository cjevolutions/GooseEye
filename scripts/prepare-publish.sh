#!/usr/bin/env bash
# Sync monorepo firmware/ into gooseeye-hardware/firmware/ for GitHub publish.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SRC="$ROOT/firmware"
DEST="$(cd "$(dirname "$0")/.." && pwd)/firmware"

if [ ! -f "$SRC/platformio.ini" ]; then
  echo "Source firmware not found at $SRC"
  exit 1
fi

mkdir -p "$DEST"
rsync -a --delete \
  --exclude '.pio/' \
  --exclude '.vscode/' \
  "$SRC/" "$DEST/"

echo "Synced $SRC -> $DEST"
