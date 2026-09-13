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
# .github/workflows/release.yml, which the tag push triggers. Push with:
#
#   git push origin HEAD
#   git push origin v<version>

set -eu

exec npx commit-and-tag-version "$@"
