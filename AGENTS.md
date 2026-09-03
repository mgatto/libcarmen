# AGENTS.md

Guidance for AI agents working in this repository.

See the root [README.md](README.md) for the project overview, goals, data model, and build instructions. See `doc/tier1-core-game-engine.md`, `doc/tier2-important-for-demo.md`, and `doc/tier3-nice-to-have.md` for the feature roadmap, and `doc/future/` for longer-range design notes.

## Markdown / prose style

- Do not inject artificial line breaks into paragraphs. Write each paragraph and list item as a single continuous line and let the editor soft-wrap it. Avoid hard-wrapping prose at 80 (or any other) columns.
- Hard line breaks are only for genuine structural boundaries: separate paragraphs, list items, headings, and code blocks.

## C style and conventions

Match the existing code exactly rather than introducing a new style. Concretely:

- **Standard**: C17, and the build must stay `-std=c17 -Wall -Wextra -pedantic` clean with zero warnings. Don't use compiler- or platform-specific extensions in `src/`/`include/` (vendored code under `vendor/` is exempt).
- **Naming**: types are `PascalCase` prefixed with `Carmen` (`CarmenCity`, `CarmenSession`); functions are `snake_case` prefixed with `carmen_<module>_` (`carmen_city_add_site`); constants and macros are `SCREAMING_SNAKE_CASE` prefixed with `CARMEN_` (`CARMEN_MAX_SITES`); header guards are `CARMEN_<FILE>_H`.
- **Formatting**: a `.clang-format` at the repo root encodes the observed style (4-space indent, no tabs, function-opening braces on their own line, `if`/`for`/`while`/`struct` braces attached, aligned consecutive struct fields/assignments/macros, one-line guard clauses like `if (!c) return;`, one-line `case X: return Y;`). Run `clang-format -style=file` on lines you touch rather than by hand. Don't run it over whole files or the whole tree -- a lot of existing multi-parameter function declarations were wrapped by hand for readability in a way `clang-format` won't reproduce exactly, so a blanket reformat would create a huge, unrelated diff. When in doubt, match the surrounding code over the tool's output. **Enforced by clang-format** - Run before committing!
- **Memory**: no dynamic allocation in core game logic -- use fixed-capacity arrays sized by `CARMEN_MAX_*` constants, and always bounds-check before writing into them (see `carmen_city_add_site` in `src/city.c` for the pattern). The only heap use is `carmen_world_create`/`carmen_world_free` (because `CarmenWorld` is large) and vendored libraries (`stb_ds`, `cJSON`). If you add a new fixed-capacity array, add a matching "respects max" test.
- **Const-correctness**: parameters and locals that aren't mutated must be `const`, including pointer targets (`const CarmenCity *`, not `CarmenCity *`, for read-only accessors).
- **Plain data**: domain structs (`CarmenCity`, `CarmenSite`, `CarmenConnection`, ...) stay plain data with no embedded bookkeeping for whatever container holds them (see "Key Design Decisions" in the README). Don't reach into `GameWorld.storage[]` or hash-map internals from outside `game_world.c`.
- **Public API surface**: every function callers are meant to use is declared in the matching `include/carmen/<module>.h`, marked `CARMEN_API`, and documented with a short `/* ... */` comment above the declaration describing behavior, ownership of any returned pointer, and the meaning of any non-obvious return value (especially negative error codes). Keep the umbrella header `include/carmen/carmen.h` and both `Makefile` (`PUBLIC_HEADERS`, `LIB_SRCS`) and `CMakeLists.txt` in sync when adding a new module.
- **Error handling**: this is a library, not an application -- never `assert()`/`abort()`/crash on bad *caller* input. Guard every public function with NULL/range checks at the top that fail gracefully (return `0`, `NULL`, or a documented negative code) as in `carmen_city_add_site` and `carmen_session_travel`. Use a distinct negative return code per failure reason (see `carmen_session_travel`'s `-1`/`-2`/`-3`/`-4` and its doc comment) rather than collapsing every failure into `-1`; document each code where the function is declared.
- **i18n**: user-facing strings go through `locales/*.json` and `i18n.c` lookups, never hardcoded literals in library code (see `locales/en.json` and how `session.c`/`case.c` reference clue/message keys).
- **Comments**: avoid narrating obvious code (no "// increment the counter"). Reserve comments for intent, invariants, and non-obvious trade-offs the code can't express on its own -- e.g. why a cap exists, why a struct is heap- vs. stack-allocated, why a magic number was chosen (see the block comment above `warrant_evidence_target` in `src/session.c` for the target style).
- **Commit style**: this repo uses Conventional Commits (`feat:`, `fix:`, `chore:`, ...) for commit subject lines; follow that when asked to commit.

## Testing

- Tests use the vendored Unity framework. Each `src/<module>.c` has a matching `test/test_<module>.c`; new modules need a new test file wired into `TEST_BINS`/`COV_BINS` in the `Makefile` (and the CMake test registration) plus `PUBLIC_HEADERS`/`LIB_SRCS` if it's a new library source.
- Follow the existing test shape: small, single-behavior test functions named `test_<function>_<behavior>`, grouped under a `/* --- carmen_module_function --- */` banner comment matching the function under test, and registered explicitly with `RUN_TEST(...)` in `main()` (no auto-discovery).
- For every public function you add or change, cover: the happy path, each documented NULL/invalid-argument guard clause, boundary conditions (empty/zero, exactly-at-capacity, over-capacity -- see `test_city_add_site_respects_max`), and every distinct return code / status value it can produce. Add tests for randomized behavior with a seeded `srand()` for determinism (see `test_city_random_inbound_clue_is_among_added`).
- When fixing a bug, add a regression test that fails on the old code and passes on the fix.
- Favor clear, readable test code over clever/compact test code -- tests double as usage examples for library consumers, so prefer explicit setup and descriptive assertions over shared magic fixtures.
- Aim for thorough coverage of new and changed code, not just the happy path; genuinely unreachable defensive code (e.g. an impossible NULL check) doesn't need a contrived test to hit it.

## Codegen

`presets/islamic.jsonc` is the single source of truth for the built-in world. `make` automatically builds `tools/gen_world` and runs it to produce `build/gen/world_islamic_generated.c`; you never edit that generated file directly. If you change the preset, just re-run `make`.

## Running a single test suite

Each test binary is built to `build/test_<module>` (or `build/test/<module>` depending on `Makefile` variable). After `make test` has built them you can re-run one suite directly:

```sh
./build/test/test_session   # run only session tests
```

Check the `TEST_BINS` variable in the Makefile for the exact paths.

## Before finishing a task

1. Build cleanly: `make` (and `make lib` if you touched anything that affects the shared library) with no new warnings.
2. `make test` -- all suites must pass.
3. For logic changes, also run `make test-sanitize` (ASan + UBSan; GCC/Clang only) to catch memory and UB issues.
4. If you added/changed library logic, run `make coverage` (requires `lcov`) and skim the report for the files you touched to catch untested branches, especially new error paths.
5. Update `README.md` / `doc/tier*.md` when you change the public API surface, project layout, or feature status they describe.
