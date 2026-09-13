#!/bin/sh
# release.sh -- thin wrapper around commit-and-tag-version (see .versionrc.json).
#
# The top-level VERSION file is the single source of truth for the version
# (see doc/versioning.md). commit-and-tag-version reads the current version
# from VERSION, derives the semver bump from the conventional commits since the
# last tag, writes the new version back to VERSION, regenerates CHANGELOG.md,
# runs `make test && make verify-soname` (configured as the `precommit`
# lifecycle hook, so the new version is validated before anything is committed
# or tagged), then commits and tags vX.Y.Z.
#
# This wrapper passes its arguments straight through, so any
# commit-and-tag-version flag works, e.g.:
#
#   scripts/release.sh --dry-run      preview the bump + changelog
#   scripts/release.sh                bump/commit/tag locally
#   scripts/release.sh --release-as minor   force a minor bump (pre-1.0 default treats feat as patch)
#
# Nothing is pushed: Building/publishing the release packages is left to
# .github/workflows/release.yml, which the tag push triggers. Push the new tag
# by name (and HEAD) -- never `git push --tags` or `--follow-tags`, which would
# also push any locally rewritten older tag and trip a protected/immutable
# remote tag:
#
#   git push origin HEAD
#   git push origin v<version>

set -eu

# Guard the release so the new tag always lands on the tip of `main` and the
# changelog is diffed against the correct previous tag. A tag is not "on a
# branch" -- it points at a commit -- so the tag is only on main's history if
# we cut it from a checked-out `main`. If an earlier release left the previous
# tag on an orphan/reachable-from-nowhere commit (or never tagged it), the
# describe probe below falls back to an older tag and commit-and-tag-version
# silently generates the changelog against the wrong comparison; refuse loudly
# instead of shipping a broken link/range.

branch="$(git symbolic-ref --short -q HEAD 2>/dev/null || true)"
if [ "$branch" != "main" ]; then
    echo "release.sh: must be run from main (currently: ${branch:-a detached HEAD})" >&2
    exit 1
fi

current_tag="v$(cat VERSION)"
latest_tag="$(git describe --tags --abbrev=0 2>/dev/null || true)"
if [ -n "$latest_tag" ] && [ "$latest_tag" != "$current_tag" ]; then
    echo "release.sh: VERSION ($current_tag) does not match the latest tag reachable from main ($latest_tag)" >&2
    echo "release.sh: the changelog would be generated against the wrong tag; fix the tag before releasing" >&2
    exit 1
fi

exec npx commit-and-tag-version "$@"
