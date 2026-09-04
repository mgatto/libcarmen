#!/bin/sh
# gen_version.sh -- emit include/carmen/carmen_version.h from the VERSION file.
#
# This is the single place that turns a MAJOR.MINOR.PATCH version string into
# the CARMEN_VERSION_* preprocessor macros. Both build systems (Makefile and
# CMakeLists.txt) call it so the compiled-in version and the generated header
# can never disagree with the VERSION file by construction.
#
#   usage: gen_version.sh <version-file> <output-header>
#
# Exits non-zero (so the build fails) if the version file does not contain a
# single well-formed MAJOR.MINOR.PATCH line.

set -eu

if [ $# -ne 2 ]; then
    echo "usage: gen_version.sh <version-file> <output-header>" >&2
    exit 1
fi

VERSION_FILE=$1
OUT=$2

VERSION=$(tr -d '[:space:]' < "$VERSION_FILE")
if ! printf '%s' "$VERSION" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "gen_version.sh: malformed version '$VERSION' in $VERSION_FILE (want MAJOR.MINOR.PATCH)" >&2
    exit 1
fi

IFS=. read -r MAJOR MINOR PATCH <<EOF
$VERSION
EOF

{
    echo "#ifndef CARMEN_VERSION_H"
    echo "#define CARMEN_VERSION_H"
    echo ""
    echo "#define CARMEN_VERSION_MAJOR $MAJOR"
    echo "#define CARMEN_VERSION_MINOR $MINOR"
    echo "#define CARMEN_VERSION_PATCH $PATCH"
    echo "#define CARMEN_VERSION_STRING \"$VERSION\""
    echo ""
    echo "#endif"
} > "$OUT"
