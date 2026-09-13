#!/bin/sh
# smoke-sdk.sh -- extract a libcarmen SDK archive and prove a downstream
# program links and runs against it through each advertised path:
# raw -lcarmen, pkg-config, and (if the archive ships it) CMake find_package.
#
# Usage: smoke-sdk.sh <archive.tar.gz> <static|shared>
#
# Used by .github/workflows/ci.yml after sdk_package / make sdk. The consumer
# sources and project live in cmake/smoke/ (main.c + CMakeLists.txt).

set -eu

archive=${1:?usage: smoke-sdk.sh <archive.tar.gz> <static|shared>}
flavor=${2:?usage: smoke-sdk.sh <archive.tar.gz> <static|shared>}

case "$flavor" in
    static|shared) ;;
    *) echo "error: flavor must be 'static' or 'shared'" >&2; exit 2 ;;
esac

CC=${CC:-cc}
case "$(uname -s)" in
    Darwin) LOADER_VAR=DYLD_LIBRARY_PATH ;;
    *)      LOADER_VAR=LD_LIBRARY_PATH ;;
esac

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SRC="$SCRIPT_DIR/../cmake/smoke/main.c"
SMOKE_DIR="$SCRIPT_DIR/../cmake/smoke"

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

tar -xzf "$archive" -C "$tmp"

sdk_root=""
for d in "$tmp"/*/; do sdk_root="${d%/}"; break; done
[ -n "$sdk_root" ] || { echo "error: no top-level SDK folder in $archive" >&2; exit 1; }

echo "smoke: SDK root = $sdk_root (flavor $flavor)"

export PKG_CONFIG_PATH="$sdk_root/lib/pkgconfig"

# Run a freshly linked binary, putting the extracted shared library on the
# loader path when the archive is the shared flavor.
run_smoke()
{
    if [ "$flavor" = shared ]; then
        env "$LOADER_VAR=$sdk_root/lib" "$@"
    else
        "$@"
    fi
}

# --- raw -lcarmen -----------------------------------------------------------
if [ "$flavor" = static ]; then
    "$CC" "$SRC" -I"$sdk_root/include" -L"$sdk_root/lib" -lcarmen -lm -o "$tmp/smoke_raw"
else
    "$CC" "$SRC" -I"$sdk_root/include" -L"$sdk_root/lib" -lcarmen -o "$tmp/smoke_raw"
fi
run_smoke "$tmp/smoke_raw"

# --- pkg-config -------------------------------------------------------------
if [ "$flavor" = static ]; then
    # shellcheck disable=SC2046
    "$CC" "$SRC" $(pkg-config --static --cflags --libs carmen) -o "$tmp/smoke_pc"
else
    # shellcheck disable=SC2046
    "$CC" "$SRC" $(pkg-config --cflags --libs carmen) -o "$tmp/smoke_pc"
fi
run_smoke "$tmp/smoke_pc"

# --- find_package(carmen) ---------------------------------------------------
# Only the CMake-built SDK ships lib/cmake/carmen; the Make-built archive
# covers just the two paths above.
if [ -f "$sdk_root/lib/cmake/carmen/carmenConfig.cmake" ]; then
    cmake -S "$SMOKE_DIR" -B "$tmp/smoke_build" -DCMAKE_PREFIX_PATH="$sdk_root"
    cmake --build "$tmp/smoke_build" -j
    run_smoke "$tmp/smoke_build/carmen_smoke"
else
    echo "smoke: no lib/cmake/carmen in archive; skipping find_package"
fi

echo "smoke: OK"
