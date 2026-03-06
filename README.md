# libcarmen-c

A "Hash Map of Composites" data structure for modeling the locations and
sites in "Where in the World is Carmen Sandiego?", implemented in C17.

Parallel implementation of [libcarmen](https://github.com/) (Ruby) and
[libcarmen-py](https://github.com/) (Python).

## Data Model

```
GameWorld          -- stb_ds string hash map (city id -> City*) with secondary index arrays
  City             -- struct: id, name, country, continent, lat/lon, sites[], connections[]
    Site           -- struct: name, site_type, clues[]
    Connection     -- struct: destination_id, distance_km, transport_mode
```

## Key Design Decisions

- **stb_ds** (header-only, MIT/public domain) for O(1) city lookup by string ID
  via `shput`/`shgeti` string hash map macros
- All City structs live in a contiguous backing array (`GameWorld.storage[]`)
  -- the stb_ds hash map stores `{key, slot_index}` entries, not the cities
  themselves
- Fixed-capacity arrays with `MAX_*` constants for sites, connections, clues
- Secondary indices for continent/country use simple linear arrays
  (6 continents, ~20 countries -- scan is effectively O(1))
- BFS uses stack-allocated queues and visited arrays -- no dynamic memory
- City struct is plain data -- no hash-table bookkeeping embedded in it

## C Language Features Used

- `<stdbool.h>` for `bool`/`true`/`false`
- Designated compound literals: `(BFSEntry){slot, depth}`
- Mixed declarations and code (C99+)
- `-std=c17 -Wall -Wextra -pedantic` clean

## Dependencies

- **stb_ds.h** (vendored in `vendor/stb/`, MIT / public domain)
- No other external dependencies
- Any C17-compliant compiler (GCC 8+, Clang 7+, MSVC 2019+)

## Building

```sh
make            # static library + demo binary
make lib        # static + shared library
make test       # run all unit tests
make coverage   # test coverage report (requires lcov)
```

Or manually:

```sh
cc -std=c17 -Wall -Wextra -pedantic -O2 -Iinclude -Ivendor/stb \
   src/site.c src/connection.c src/city.c src/game_world.c \
   src/seed_data.c examples/carmen.c -o carmen
```

## Running

```sh
./build/carmen
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

```
Makefile                   Build rules (static/shared lib, tests, coverage, install)
LICENSE                    MIT license
carmen.pc.in               pkg-config template
include/carmen/
  carmen.h                 Umbrella header (includes everything below)
  carmen_version.h         Version macros (CARMEN_VERSION_MAJOR/MINOR/PATCH)
  carmen_export.h          Symbol visibility / DLL export macros
  site.h                   Site struct and API
  connection.h             Connection struct and API
  city.h                   City composite struct and API
  game_world.h             GameWorld hash map + indices + BFS API
  seed_data.h              World builder API
src/
  site.c                   Site implementation
  connection.c             Connection implementation
  city.c                   City implementation
  game_world.c             Hash map, secondary indices, BFS, shortest path
  seed_data.c              22 cities, ~75 sites, ~60 clues, ~30 routes
examples/
  carmen.c                 main() demo driver
test/
  test_site.c              Site unit tests
  test_connection.c        Connection unit tests
  test_city.c              City unit tests
  test_game_world.c        GameWorld unit tests
  test_carmen_scenarios.c   Full-world integration tests
vendor/
  stb/stb_ds.h             Hash map / dynamic array library (vendored)
  unity/                   Unity test framework (vendored)
```

## License

MIT -- see [LICENSE](LICENSE).
