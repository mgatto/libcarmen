# libcarmen

Portable C17 core engine for Carmen Sandiego-style world exploration games, starting with an [Islamicate world](#built-in-world) and extensible to other cultural realms — zero external deps, clean C ABI, FFI- and WebAssembly-ready.

## Project Goal

`libcarmen` is a **hobby project**: a small, portable, dependency-light **core engine** designed to be cleanly wrapped by other languages and frameworks. The C17 core owns all game logic and state; front-ends and language bindings simply drive it through a stable, UI-agnostic API.

The public interface is intentionally shaped for wrapping:

- **Plain-data structs and a C ABI** -- easy to bind from Ruby (FFI), Python (ctypes/cffi), Rust, Go (cgo), Node (N-API), or compile to WebAssembly via Emscripten.
- **Clear separation of queries and actions** -- read-only query functions for rendering the UI, and discrete action functions (`travel`, `investigate`, `issue_warrant`, `arrest`) for input.
- **Built-in i18n** -- user-facing strings are looked up via locale files, so wrappers and UIs stay language-neutral.
- **No embedded UI or I/O in the core** -- the terminal demo is just one front-end; the same library is meant to back native, web, and TUI clients equally.

Ruby and Python wrappers (and other language bindings and front-ends) are intended to build on top of this core rather than reimplement it.

## Built-in Islamicate World

The built-in world covers 22 important Muslim cities globally, aiming for a tasteful balance of the historical and the modern — from classical centers of learning and trade to present-day capitals and cultural hubs. They span the breadth of the Islamicate world: Istanbul (Süleymaniye Mosque, Topkapı Palace), Fez (Al-Qarawiyyin, the ancient medina), Samarkand (the Registan, Bibi-Khanum Mosque), Zanzibar (Stone Town, Hamamni Persian Baths), Lahore (Badshahi Mosque, Lahore Fort), Kuala Lumpur (Petronas Towers), and sixteen others — each with up to four investigable sites across types: market, mosque, museum, landmark, and park. The selection is a starting point, not a canon; the preset system ([`presets/islamic.jsonc`](presets/islamic.jsonc)) is designed for adding worlds rooted in other cultural geographies.

The criminal organization behind each case is **FITNA** — a roster of 16 thieves defined in [`include/carmen/villain.h`](include/carmen/villain.h) with names drawn from Arabic, Persian, and Urdu traditions: Qamar Samarkandi ("The Moon"), Layla Lapis ("Midnight Blue"), Tariq al-Tariq ("The Morning Star"), Soraya Samum ("The Sandstorm"), Rumi the Riddle ("The Poet"), and eleven others. Each villain carries four identity clues the player collects at the hideout in order to issue a warrant.

## Data Model

The static world (built once, read-only during play):

```C
GameWorld          -- stb_ds string hash map (city id -> slot) with secondary index arrays
  City             -- struct: id, name, local_name, country, continent, lat/lon, sites[], connections[], inbound_clues[]
    Site           -- struct: id, name, site_type  (clues are drawn at case-gen time from the city's inbound_clues[] pool)
    Connection     -- struct: destination_id, distance_km, transport_mode
```

The scenario and play state (generated per game from the world plus settings):

```C
CaseSettings       -- difficulty + optional overrides (trail length, time budget, move limit, ...)
Case               -- one generated scenario, built from the world + settings
  Villain          -- culprit picked from the FITNA_VILLAINS[] catalog (id clues, alias, gender)
  Artifact         -- stolen item from the CARMEN_ARTIFACTS[] catalog (origin city = crime scene)
  trail[]          -- ordered chain of connected cities from origin to hideout
  stops[]          -- per-city active sites and their assigned clues
Session            -- live play state over a Case
  current city, visited history, time/moves remaining
  notebook[]       -- clues gathered by investigating sites
  evidence[]       -- villain identity clues collected at the hideout
  warrant          -- issued villain index (gates a valid arrest)
  status           -- PLAYING / WON / LOST_* outcome
```

The core exposes read-only queries for rendering (current city, connections, active sites, notebook, evidence, time/moves) and discrete actions for input (`travel`, `investigate`, `issue_warrant`, `arrest`).

## Key Design Decisions

- **stb_ds** (header-only, MIT/public domain) for O(1) city lookup by string ID via `shput`/`shgeti` string hash map macros
- All City structs live in a contiguous backing array (`GameWorld.storage[]`) -- the stb_ds hash map stores `{key, slot_index}` entries, not the cities themselves
- Fixed-capacity arrays with `MAX_*` constants for sites, connections, clues
- Secondary indices for continent/country use simple linear arrays (6 continents, ~20 countries -- scan is effectively O(1))
- BFS uses stack-allocated queues and visited arrays -- no dynamic memory
- City struct is plain data -- no hash-table bookkeeping embedded in it

## C Language Features Used

- `<stdbool.h>` for `bool`/`true`/`false`
- Designated compound literals: `(BFSEntry){slot, depth}`
- Mixed declarations and code (C99+)
- `-std=c17 -Wall -Wextra -pedantic` clean

## Dependencies

All third-party code is vendored under `vendor/` -- there are no external dependencies to install.

- **stb_ds.h** (`vendor/stb/`, MIT / public domain) -- string hash map / dynamic arrays
- **cJSON** (`vendor/cjson/`, MIT) -- parses the locale JSON files
- **toml-c** (`vendor/toml-c/`, MIT) -- parses optional case settings files
- **utf8.h** (`vendor/utf8/`, public domain) -- UTF-8 helpers
- **Unity** (`vendor/unity/`, MIT) -- unit test framework (test builds only)
- Any C17-compliant compiler (GCC 8+, Clang 7+, MSVC 2019+) -- Linux and macOS are built and tested with both Make and CMake in CI; Windows/MSVC and WebAssembly (emscripten) are built and tested with CMake in CI (see `.github/workflows/ci.yml`)

## Building

```sh
make               # static library + demo binary (build/trail_demo)
make lib           # static + shared library
make test          # run all unit tests
make test-sanitize # run all unit tests under ASan + UBSan (GCC/Clang only)
make coverage      # test coverage report (requires lcov)
make package       # self-contained macOS demo tarball (libcarmen-demo-<version>-macos-<arch>.tar.gz)
```

Or with CMake (also how the WebAssembly and Windows/MSVC builds are configured -- see [Building](#building) below and `.github/workflows/ci.yml`):

```sh
cmake -S . -B build && cmake --build build -j && ctest --test-dir build

# ASan + UBSan (GCC/Clang only; ignored on MSVC and when cross-compiling)
cmake -S . -B build-sanitize -DCARMEN_SANITIZE=address,undefined
cmake --build build-sanitize -j && ctest --test-dir build-sanitize
```

Or manually:

```sh
# 1. Build the world generator and compile the preset into C.  The built-in
#    world lives in presets/islamic.jsonc; gen_world validates it and emits C.
cc -std=c17 -O2 -Iinclude -Ivendor/cjson \
   tools/gen_world.c vendor/cjson/cJSON.c -o gen_world
./gen_world presets/islamic.jsonc world_islamic_generated.c

# 2. Build the demo (note -Isrc so the generated file finds seed_helpers.h).
cc -std=c17 -Wall -Wextra -pedantic -O2 \
   -Iinclude -Isrc -Ivendor/stb -Ivendor/utf8 -Ivendor/cjson -Ivendor/toml-c \
   src/utf8.c src/site.c src/connection.c src/city.c src/game_world.c \
   world_islamic_generated.c src/villain.c src/artifact.c src/case.c \
   src/session.c src/settings.c src/i18n.c vendor/cjson/cJSON.c \
   examples/trail_demo.c -o trail_demo
```

## Running

Run from the repository root -- the demo loads its locale file via the relative path `locales/<locale>.json`:

```sh
./build/trail_demo              # default locale "en"
./build/trail_demo es           # use locales/es.json
./build/trail_demo en case.toml # optional case settings file
```

## Installing

```sh
make install              # installs to /usr/local
make install PREFIX=/opt  # custom prefix
make uninstall
```

After installing, link with `-lcarmen` or use pkg-config:

```sh
cc $(pkg-config --cflags --libs libcarmen) my_game.c -o my_game
```

Consumer code includes the library with the `carmen/` prefix:

```c
#include <carmen/carmen.h>
```

## Project Layout

```BASH
Makefile                   Build rules (static/shared lib, tests, coverage, install)
LICENSE                    MIT license
carmen.pc.in               pkg-config template
include/carmen/
  carmen.h                 Umbrella header (includes everything below)
  carmen_version.h         Version macros (CARMEN_VERSION_MAJOR/MINOR/PATCH)
  carmen_export.h          Symbol visibility / DLL export macros
  utf8.h                   UTF-8 helper API
  clue.h                   Clue struct
  site.h                   Site struct and API
  connection.h             Connection struct and API
  city.h                   City composite struct and API
  game_world.h             GameWorld hash map + indices + BFS API
  world_islamic.h          Built-in world builder API (impl generated at build time)
  villain.h                Villain roster and API
  artifact.h               Stolen artifact struct and API
  case.h                   Case (villain + trail + settings) API
  session.h                Play session state and actions
  settings.h               Case settings (difficulty, TOML loading)
  save.h                   Session JSON save/load (serialize/restore)
  i18n.h                   Locale loading and string lookup
src/
  utf8.c                   UTF-8 helpers
  site.c                   Site implementation
  connection.c             Connection implementation
  city.c                   City implementation
  game_world.c             Hash map, secondary indices, BFS, shortest path
  seed_helpers.h           Inline helpers used by the generated world builder
  villain.c                Villain roster
  artifact.c               Stolen artifact implementation
  case.c                   Case generation (villain, trail, briefing)
  session.c                Session state and actions (travel/investigate/warrant/arrest)
  settings.c               Case settings + TOML loading
  save.c                   Session JSON serialization (save/load, cJSON)
  i18n.c                   Locale JSON loading and string lookup
presets/
  islamic.jsonc            Built-in world data (single source of truth, JSONC)
tools/
  gen_world.c              Host tool: validates a preset and emits it as C
  CMakeLists.txt           Standalone build for the host tool (used when cross-compiling)
examples/
  trail_demo.c             main() demo driver (interactive terminal game)
locales/
  en.json                  English locale strings
test/
  test_site.c              Site unit tests
  test_connection.c        Connection unit tests
  test_city.c              City unit tests
  test_game_world.c        GameWorld unit tests
  test_carmen_scenarios.c  Full-world integration tests
  test_artifact.c          Artifact unit tests
  test_case.c              Case unit tests
  test_session.c           Session unit tests
  test_save.c              Session save/load (JSON) tests
  test_settings.c          Settings unit tests
  test_villain.c           Villain unit tests
  test_world_islamic.c     Built-in world golden-value tests
  test_i18n.c              i18n loading (file + buffer) tests
  fixtures/                Invalid presets the generator must reject
vendor/
  stb/stb_ds.h             Hash map / dynamic array library (vendored)
  unity/                   Unity test framework (vendored)
```

## License

MIT -- see [LICENSE](LICENSE).
