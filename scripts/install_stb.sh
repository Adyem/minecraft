#!/usr/bin/env bash
# Downloads stb_truetype.h into vendor/ if not already present
set -e
DEST="$(dirname "$0")/../vendor/stb_truetype.h"
if [ -f "$DEST" ]; then
    echo "stb_truetype.h already present."
    exit 0
fi
URL="https://raw.githubusercontent.com/nothings/stb/master/stb_truetype.h"
if command -v curl >/dev/null 2>&1; then
    curl -fsSL "$URL" -o "$DEST"
elif command -v wget >/dev/null 2>&1; then
    wget -q "$URL" -O "$DEST"
else
    echo "ERROR: curl or wget required." >&2
    exit 1
fi
echo "Downloaded stb_truetype.h to vendor/"
