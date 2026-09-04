#!/bin/sh
# check_version.sh -- fail if the three version renderings disagree.
#
# The version lives in exactly one place (the top-level VERSION file). Both
# build systems derive their value from it, and both regenerate the public
# carmen_version.h header from it. This check is the backstop: it takes the
# authority, the generated header, and the *caller's* resolved version string,
# and fails with a distinct message when any one disagrees -- catching a stale
# generated header, a broken parse in one build system, or a hand-edited
# literal that has drifted.
#
#   usage: check_version.sh <version-file> <generated-header> <resolved-version>

set -eu

if [ $# -ne 3 ]; then
    echo "usage: check_version.sh <version-file> <generated-header> <resolved-version>" >&2
    exit 1
fi

VERSION_FILE=$1
HEADER=$2
RESOLVED=$3

AUTHORITY=$(tr -d '[:space:]' < "$VERSION_FILE")

# Header value comes from CARMEN_VERSION_STRING "X.Y.Z".
HEADER_VERSION=$(sed -n 's/^#define CARMEN_VERSION_STRING "\([^"]*\)".*/\1/p' "$HEADER")

if [ -z "$HEADER_VERSION" ]; then
    echo "FAIL: could not read CARMEN_VERSION_STRING from $HEADER" >&2
    exit 1
fi

if [ "$RESOLVED" != "$AUTHORITY" ]; then
    echo "FAIL: build-system version '$RESOLVED' differs from VERSION file '$AUTHORITY'" >&2
    exit 1
fi

if [ "$HEADER_VERSION" != "$AUTHORITY" ]; then
    echo "FAIL: generated header '$HEADER_VERSION' differs from VERSION file '$AUTHORITY' (stale header?)" >&2
    exit 1
fi

echo "ok: version consistent ($AUTHORITY)"
