#!/bin/sh
# Refresh vendor/fribidi from an upstream release tarball.
set -eu

VERSION=1.0.16
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/fribidi-vendor-$$"
TARBALL="${TMP}/fribidi-${VERSION}.tar.xz"

mkdir -p "$TMP"
curl -L "https://github.com/fribidi/fribidi/releases/download/v${VERSION}/fribidi-${VERSION}.tar.xz" \
  -o "$TARBALL"
tar -xJf "$TARBALL" -C "$TMP"

SRC="$TMP/fribidi-${VERSION}"
DEST="$ROOT/vendor/fribidi"

rm -rf "$DEST/lib"
cp "$SRC/COPYING" "$DEST/"
cp -R "$SRC/lib" "$DEST/"
rm -f "$DEST/lib/Makefile."* "$DEST/lib/meson.build" "$DEST/lib/Headers.mk" \
      "$DEST/lib/fribidi.def" "$DEST/lib/fribidi-config.h.in"

cat > "$DEST/config.h" <<'EOF'
/* Minimal config for vendored GNU FriBidi — see vendor/fribidi/README.vendor */
#ifndef FRIBIDI_VENDORED_CONFIG_H
#define FRIBIDI_VENDORED_CONFIG_H

#define HAVE_STRINGIZE 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define STDC_HEADERS 1

#endif
EOF

cat > "$DEST/lib/fribidi-config.h" <<'EOF'
/* Vendored from GNU FriBidi 1.0.16 — see vendor/fribidi/README.vendor */
#ifndef FRIBIDI_CONFIG_H
#define FRIBIDI_CONFIG_H

#define FRIBIDI "fribidi"
#define FRIBIDI_NAME "GNU FriBidi"
#define FRIBIDI_BUGREPORT "https://github.com/fribidi/fribidi/issues/new"

#define FRIBIDI_VERSION "1.0.16"
#define FRIBIDI_MAJOR_VERSION 1
#define FRIBIDI_MINOR_VERSION 0
#define FRIBIDI_MICRO_VERSION 16
#define FRIBIDI_INTERFACE_VERSION 4
#define FRIBIDI_INTERFACE_VERSION_STRING "4"

#define FRIBIDI_SIZEOF_INT 4

#undef FRIBIDI_BUILT_WITH_MSVC

#endif /* FRIBIDI_CONFIG_H */
EOF

rm -rf "$TMP"
echo "Vendored FriBidi ${VERSION} into vendor/fribidi/"
