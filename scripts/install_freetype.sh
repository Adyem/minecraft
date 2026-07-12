#!/usr/bin/env bash
set -euo pipefail

VERSION="${1:-2.14.3}"
PREFIX_INPUT="${2:-}"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"
if command -v cygpath >/dev/null 2>&1; then
    ROOT_DIR="$(cygpath -u "$ROOT_DIR")"
fi
if [ -z "$PREFIX_INPUT" ]; then
    PREFIX_INPUT="$ROOT_DIR/.deps/freetype/$VERSION"
fi

if command -v cygpath >/dev/null 2>&1; then
    PREFIX="$(cygpath -u "$PREFIX_INPUT")"
else
    PREFIX="$PREFIX_INPUT"
fi
if command -v cygpath >/dev/null 2>&1; then
    PREFIX_NATIVE="$(cygpath -m "$PREFIX")"
else
    PREFIX_NATIVE="$PREFIX"
fi

HEADER="$PREFIX/include/freetype2/ft2build.h"
LIB="$PREFIX/lib/libfreetype.a"

if [ -f "$HEADER" ] && [ -f "$LIB" ]; then
    printf 'FreeType %s already installed at %s\n' "$VERSION" "$PREFIX"
    exit 0
fi

TMP_DIR="$ROOT_DIR/.tmp/freetype-$VERSION.$$"
SRC_DIR="$TMP_DIR/freetype-$VERSION"
TARBALL="$TMP_DIR/freetype-$VERSION.tar.xz"

cleanup() {
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT

mkdir -p "$TMP_DIR"

URL="https://downloads.sourceforge.net/project/freetype/freetype2/$VERSION/freetype-$VERSION.tar.xz"
printf 'Downloading FreeType %s\n' "$VERSION"
if command -v curl >/dev/null 2>&1; then
    CURL_ARGS=(-fsSL)
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*)
            CURL_ARGS+=(--ssl-no-revoke)
            ;;
    esac
    if ! curl "${CURL_ARGS[@]}" "$URL" -o "$TARBALL"; then
        if command -v wget >/dev/null 2>&1; then
            wget -q "$URL" -O "$TARBALL"
        else
            exit 1
        fi
    fi
elif command -v wget >/dev/null 2>&1; then
    wget -q "$URL" -O "$TARBALL"
else
    echo "ERROR: curl or wget is required to download FreeType." >&2
    exit 1
fi

printf 'Extracting FreeType %s\n' "$VERSION"
tar -xf "$TARBALL" -C "$TMP_DIR"

if [ ! -x "$SRC_DIR/configure" ]; then
    echo "ERROR: FreeType configure script not found after extraction." >&2
    exit 1
fi

mkdir -p "$PREFIX"
mkdir -p "$SRC_DIR/objs/.libs"

if command -v mingw32-make >/dev/null 2>&1; then
    MAKE_BIN=mingw32-make
elif command -v make >/dev/null 2>&1; then
    MAKE_BIN=make
else
    echo "ERROR: make is required to build FreeType." >&2
    exit 1
fi

printf 'Configuring FreeType %s\n' "$VERSION"
(
    cd "$SRC_DIR"
    rm -f builds/unix/config.mk
    if PWD_WIN="$(pwd -W 2>/dev/null)"; then
        export PWD="$PWD_WIN"
    fi
    CC="${CC:-gcc}" ./configure --prefix="$PREFIX_NATIVE" --enable-static --disable-shared
)

if [ -f "$SRC_DIR/builds/unix/unix-def.mk" ]; then
    sed -i "s#^TOP_DIR := .*#TOP_DIR := \$(shell cd \$(TOP_DIR); pwd -W 2>/dev/null || pwd)#" "$SRC_DIR/builds/unix/unix-def.mk"
    sed -i "s#^prefix       := .*#prefix       := $PREFIX_NATIVE#" "$SRC_DIR/builds/unix/unix-def.mk"
fi

printf 'Building FreeType %s\n' "$VERSION"
(
    cd "$SRC_DIR"
    if PWD_WIN="$(pwd -W 2>/dev/null)"; then
        export PWD="$PWD_WIN"
    fi
    "$MAKE_BIN" -j1
    "$MAKE_BIN" install
)

printf 'Installed FreeType %s to %s\n' "$VERSION" "$PREFIX"
