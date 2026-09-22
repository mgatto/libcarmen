#!/bin/sh
# package-demo-macos.sh -- build the macOS demo .dmg from a self-contained
# runnable bundle directory.
#
#   scripts/package-demo-macos.sh <staged_bundle_dir> <version> <output.dmg>
#
# The staged bundle dir must already contain trail_demo, the libcarmen shared
# dylib (real file + soname symlink), locales/, settings.toml, LICENSE, and a
# README.txt. This script then:
#   1. ad-hoc signs the dylibs low-to-high and the executable on top of them,
#   2. drops an /Applications symlink into the bundle for drag-install,
#   3. builds a compressed read-only UDZO image with hdiutil,
#   4. ad-hoc signs the .dmg itself.
#
# "Ad-hoc" (codesign --sign -) matters because it gives every Mach-O a valid
# code signature without an Apple Developer certificate: it removes the
# inconsistent-signature / "damaged app" and dyld library-validation errors,
# and makes `codesign --verify` pass. It does NOT satisfy Gatekeeper's
# "identified developer" requirement -- a quarantined download still needs a
# one-time right-click > Open (or `xattr -dr com.apple.quarantine .`); only a
# paid Developer ID + notarization removes that, which is out of scope here.
set -eu

if [ "$#" -ne 3 ]; then
    echo "usage: $0 <staged_bundle_dir> <version> <output.dmg>" >&2
    exit 2
fi

staged=$1
version=$2
out=$3

[ -d "$staged" ] || { echo "$0: staged bundle not found: $staged" >&2; exit 1; }
[ -f "$staged/trail_demo" ] || { echo "$0: trail_demo not found in $staged" >&2; exit 1; }
command -v codesign >/dev/null || { echo "$0: codesign not found" >&2; exit 1; }
command -v hdiutil >/dev/null || { echo "$0: hdiutil not found" >&2; exit 1; }

# Sign the shared library first, then the executable that links it. A library
# may be staged as a real file plus a soname symlink; sign the real file.
for dylib in "$staged"/libcarmen.*.dylib; do
    [ -e "$dylib" ] || continue
    if [ -L "$dylib" ]; then
        real=$( (cd "$staged" && readlink "$(basename "$dylib")") )
        case "$real" in
            /*) dylib="$real" ;;
            *)  dylib="$staged/$real" ;;
        esac
    fi
    codesign --force --sign - "$dylib"
done

codesign --force --sign - "$staged/trail_demo"

# Drag-install affordance shown in the mounted image.
ln -sf /Applications "$staged/Applications"

hdiutil create "$out" \
    -volname "libcarmen demo $version" \
    -srcfolder "$staged" \
    -ov \
    -format UDZO >/dev/null

codesign --force --sign - "$out"

echo "$0: image ready: $out"
