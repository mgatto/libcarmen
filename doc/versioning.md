# Versioning & ABI policy

## Single source of truth

The top-level `VERSION` file is the single authority for the library version. It contains one `MAJOR.MINOR.PATCH` line, e.g. `0.9.0`. Everything else is derived from it and must never be edited by hand:

- **Makefile** reads it (`VERSION := $(shell cat VERSION)`, `SOMAJOR` = the major component).
- **CMakeLists.txt** reads it and sets `project(carmen VERSION ...)` from it.
- **`carmen_version.h`** is generated into the build directory by `tools/gen_version.sh`; it is not committed (see `.gitignore`), and the `CARMEN_VERSION_MAJOR/MINOR/PATCH/STRING` macros it defines are what consumer code reads at compile time.

Bump the version by editing `VERSION` only; `make` (or `cmake --build`) regenerates everything else.

## What a bump means (semver → ABI)

- **Major (`X`.`y`.`z`)** — an ABI-breaking change: removing/renaming a public function or type, changing a public struct's layout, changing a macro/enumerator's value, dropping `CARMEN_API` symbols. `SOMAJOR`/`SOVERSION` (and therefore the SONAME/`install_name`) must change, so a major bump is a rebuild-the-world event for bindings and binaries.
- **Minor (`x`.`Y`.`z`)** — additive and ABI-compatible: new functions, new constants, new struct *fields only if* appended in a way that preserves existing offsets. `SOVERSION` does not change; consumers of the previous minor can drop in the new library without recompiling.
- **Patch (`x`.`y`.`Z`)** — bug fixes with no API or ABI change.

Because the make and CMake builds both key `SOVERSION`/`SOMAJOR` off the major component, changing the major number is the single mechanism that bumps the SONAME (`libcarmen.so.0` → `libcarmen.so.1`) and `install_name` (`@rpath/libcarmen.0.dylib` → `@rpath/libcarmen.1.dylib`) in lockstep.

## What counts as the ABI (and why it matters)

Cross-language bindings (Ruby/Python/Rust/Node/WASM) read the library from the header and load the shared library at runtime. `include/carmen/` is an ABI contract, not just an API: the exact `offsetof` of every field a caller introspects, the `sizeof` of every public struct, the values of every `CARMEN_*` constant and enum, and the set of exported `CARMEN_API` symbols. These rules mirror `.cursor/rules/cross-platform-abi.mdc`; when a change is additive under that rule it is minor, and when it is not it is major.

## Guards against drift

Two checks fail the build/suite if the version renderings disagree (stale generated header, a broken parse, a hand-edited literal):

- `make version-check` — runs as part of `make test`; compares `VERSION`, the generated header, and the Makefile's resolved `$(VERSION)`.
- CTest `version_check` — compares `VERSION`, the generated header, and CMake's `PROJECT_VERSION`.
- `make verify-soname` — asserts the shared library's recorded SONAME/`install_name` is the version-derived `.$(SOMAJOR)` form, so Make output is byte-for-byte the same recorded name CMake emits (run in CI for both Linux and macOS; see `.cursor/rules/build-parity.mdc`).

## Release notes

Tags `vX.Y.Z` must match the `VERSION` file (`release.yml` fails the workflow otherwise), and the changelog's "Breaking changes" section must be non-empty exactly when the major component changed.
