#!/bin/sh
# release.sh -- bump the version, verify it builds cleanly, and tag + push a release.
#
# The top-level VERSION file is the single source of truth for the version
# (see doc/versioning.md). This script performs the mechanical release steps
# around it:
#
#   1. validate a strict MAJOR.MINOR.PATCH argument
#   2. guard against uncommitted changes, a non-increasing version, and an
#      existing tag
#   3. write VERSION
#   4. run the full build + test suite via both Make and CMake so the new
#      version is proven to propagate (version-check, verify-soname, ctest)
#   5. commit VERSION, tag it v<version>, and (only with --push) push both
#
# Building/publishing the release packages is deliberately left to
# .github/workflows/release.yml, which the tag push triggers. This script only
# produces the version bump, commit, and tag.
#
#   usage: scripts/release.sh <MAJOR.MINOR.PATCH> [--push] [--dry-run] [--force]
#
#   --push, -p    push the branch and the tag after tagging (default: local only)
#   --dry-run, -n print every action without modifying anything
#   --force, -f   skip the "new version must be greater than current" check

set -eu

usage() {
    echo "usage: $0 <MAJOR.MINOR.PATCH> [--push] [--dry-run] [--force]" >&2
    exit 2
}

# new_gt_cur "$a" "$b" -- true iff a > b under numeric MAJOR.MINOR.PATCH compare.
new_gt_cur() {
    a=$1
    b=$2
    IFS=. read -r amaj amin apat <<EOF
$a
EOF
    IFS=. read -r bmaj bmin bpat <<EOF
$b
EOF
    [ "$amaj" -gt "$bmaj" ] && return 0
    [ "$amaj" -lt "$bmaj" ] && return 1
    [ "$amin" -gt "$bmin" ] && return 0
    [ "$amin" -lt "$bmin" ] && return 1
    [ "$apat" -gt "$bpat" ]
}

run_step() {
    echo "  + $*"
    if [ "$DRY_RUN" -eq 0 ]; then
        "$@"
    fi
}

# --------------------------------------------------------------------------- #
#  Argument parsing
# --------------------------------------------------------------------------- #

[ $# -ge 1 ] || usage

NEW_VERSION=""
PUSH=0
DRY_RUN=0
FORCE=0

for arg in "$@"; do
    case "$arg" in
        --push | -p)  PUSH=1 ;;
        --dry-run | -n) DRY_RUN=1 ;;
        --force | -f) FORCE=1 ;;
        -*) usage ;;
        *)
            if [ -n "$NEW_VERSION" ]; then
                echo "release.sh: unexpected extra argument '$arg'" >&2
                usage
            fi
            NEW_VERSION=$arg
            ;;
    esac
done

[ -n "$NEW_VERSION" ] || usage

# --------------------------------------------------------------------------- #
#  Validation / preflight guards
# --------------------------------------------------------------------------- #

if ! printf '%s' "$NEW_VERSION" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "release.sh: '$NEW_VERSION' is not MAJOR.MINOR.PATCH" >&2
    exit 1
fi

if [ ! -f VERSION ]; then
    echo "release.sh: run from the repository root (no VERSION file here)" >&2
    exit 1
fi

CUR_VERSION=$(tr -d '[:space:]' < VERSION)

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "release.sh: not inside a git working tree" >&2
    exit 1
fi

# Only staged/unstaged *tracked* changes matter here: the commit below stages
# nothing but VERSION, and untracked files are never added (see `git add VERSION`).
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "release.sh: working tree has uncommitted changes -- commit or stash them first" >&2
    exit 1
fi

if [ "$FORCE" -eq 0 ]; then
    if ! new_gt_cur "$NEW_VERSION" "$CUR_VERSION"; then
        echo "release.sh: $NEW_VERSION must be greater than the current version $CUR_VERSION (use --force to override)" >&2
        exit 1
    fi
elif [ "$NEW_VERSION" = "$CUR_VERSION" ]; then
    echo "release.sh: $NEW_VERSION is already the current version" >&2
    exit 1
fi

if git rev-parse -q --verify "refs/tags/v$NEW_VERSION" >/dev/null 2>&1; then
    echo "release.sh: tag v$NEW_VERSION already exists" >&2
    exit 1
fi

if [ "$PUSH" -eq 1 ] && ! git remote get-url origin >/dev/null 2>&1; then
    echo "release.sh: no 'origin' remote to push to" >&2
    exit 1
fi

echo "release.sh: bumping $CUR_VERSION -> $NEW_VERSION"

# --------------------------------------------------------------------------- #
#  Perform the release
# --------------------------------------------------------------------------- #

if [ "$DRY_RUN" -eq 0 ]; then
    printf '%s\n' "$NEW_VERSION" > VERSION
else
    echo "  + write VERSION = $NEW_VERSION"
fi

if [ "$DRY_RUN" -eq 0 ]; then
    if ! make test; then
        echo "release.sh: make test failed after the version bump; restoring VERSION" >&2
        git checkout -- VERSION
        exit 1
    fi
    if ! make verify-soname; then
        echo "release.sh: make verify-soname failed; restoring VERSION" >&2
        git checkout -- VERSION
        exit 1
    fi

    CMAKE_BUILD_DIR=build/release-check
    rm -rf "$CMAKE_BUILD_DIR"
    if ! cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug; then
        echo "release.sh: cmake configure failed; restoring VERSION" >&2
        git checkout -- VERSION
        exit 1
    fi
    if ! cmake --build "$CMAKE_BUILD_DIR" -j; then
        echo "release.sh: cmake build failed; restoring VERSION" >&2
        git checkout -- VERSION
        exit 1
    fi
    if ! ctest --test-dir "$CMAKE_BUILD_DIR" --output-on-failure; then
        echo "release.sh: ctest failed; restoring VERSION" >&2
        git checkout -- VERSION
        exit 1
    fi
else
    echo "  + make test"
    echo "  + make verify-soname"
    echo "  + cmake -S . -B build/release-check && cmake --build build/release-check -j && ctest --test-dir build/release-check"
fi

run_step git add VERSION
run_step git commit -m "chore: bump version to $NEW_VERSION"
run_step git tag -a "v$NEW_VERSION" -m "Release v$NEW_VERSION"

if [ "$PUSH" -eq 1 ]; then
    run_step git push origin HEAD
    run_step git push origin "v$NEW_VERSION"
else
    echo ""
    echo "Committed and tagged locally. To publish (triggers release.yml):"
    echo "  git push origin HEAD"
    echo "  git push origin v$NEW_VERSION"
fi

if [ "$DRY_RUN" -eq 1 ]; then
    echo "(dry run: nothing was changed)"
fi
