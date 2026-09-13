# Contributing to libcarmen

Thanks for your interest in contributing. This file is a map, not a rulebook: it points you at the documents that actually define the project's conventions so you don't have to hunt for them.

## Where to start

- [`README.md`](README.md) — project overview, goals, data model, and build instructions.
- [`AGENTS.md`](AGENTS.md) — the normative source of truth for C style and conventions, testing, codegen, and the pre-merge checklist. Start here before writing any code.

## Coding conventions

C style, naming, formatting (`.clang-format`), memory rules, const-correctness, error handling, i18n, and comment style all live in [`AGENTS.md`](AGENTS.md). A few topics are expanded in dedicated docs:

- [`doc/c-safety.md`](doc/c-safety.md) — no undefined behavior, bounds-checked fixed buffers, safe string handling, when to use `assert()`.
- [`doc/clean-api-checklist.md`](doc/clean-api-checklist.md) — checklist for auditing public headers before adding a module.
- [`doc/cross-platform-abi.md`](doc/cross-platform-abi.md) — fixed-width types, pointer ownership/lifetime, avoiding leaked platform details for FFI bindings.
- [`doc/build-parity.md`](doc/build-parity.md) — every source/header change must update the Makefile and CMakeLists.txt in the same commit.

## Testing

Testing rules and the authoritative "before finishing a task" checklist (build, `make test`, `make test-sanitize`, `make analyze`, `make coverage`) are in [`AGENTS.md`](AGENTS.md). Every `src/<module>.c` has a matching `test/test_<module>.c`; new modules need new test files wired into both build systems.

## Versioning

The top-level `VERSION` file is the single source of truth. See [`doc/versioning.md`](doc/versioning.md) for the semver + ABI policy and how to keep Make and CMake in lockstep.

## Roadmap and where to help

- [`doc/tier1-core-game-engine.md`](doc/tier1-core-game-engine.md), [`doc/tier2-important-for-demo.md`](doc/tier2-important-for-demo.md), [`doc/tier3-nice-to-have.md`](doc/tier3-nice-to-have.md) — the feature roadmap, in priority order.
- [`doc/future/`](doc/future/) — longer-range design notes and gap analyses.

## Commits and pull requests

This repository uses [Conventional Commits](https://www.conventionalcommits.org/) for commit subjects (`feat:`, `fix:`, `chore:`, ...). Before opening a PR, run the checks listed in [`AGENTS.md`](AGENTS.md)'s "Before finishing a task" section; CI runs the same suite across Linux, macOS, Windows, and WebAssembly.

## License

All contributions are under the MIT license (see [`LICENSE`](LICENSE)). Vendored third-party code (notably GNU FriBidi, LGPL-2.1+) keeps its own license under `vendor/`.
