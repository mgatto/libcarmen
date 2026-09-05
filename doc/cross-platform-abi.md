# Cross-Platform ABI Rules

Per `README.md`, every public header is meant to be bound from Ruby, Python, Rust, Go, Node, and WebAssembly. That means every struct in `include/carmen/*.h` is an implicit ABI contract for those bindings, not just a C API.

## Fixed-width types in any struct a binding reads field-by-field

Use `int32_t`/`uint32_t`/etc. from `<stdint.h>`, never bare `int`/`long`, for struct fields a binding introspects directly. `int`'s width isn't guaranteed across platforms/compilers, and a binding that hardcodes a field offset will silently misread the struct.

```c
// BAD: width and signedness aren't guaranteed across platforms
struct CarmenConnection { int distance_km; };

// GOOD
struct CarmenConnection { int32_t distance_km; };
```

## When *not* to reach for fixed-width types

The inverse rule matters as much as the positive one, and keeps bare `int` from being treated as a defect:

- **Scalars that never cross the FFI boundary stay `int`.** Function parameters and return values (indices, counts, buffer sizes, distances) and internal locals are passed in registers or stack slots; no binding reads their offset, so `int` costs nothing and reads as "no ABI contract here."
- **Use `size_t` only where the value really denotes an object size.** This codebase's fixed `CARMEN_MAX_*` buffers keep virtually everything small and bounded; don't introduce `size_t` (or `int32_t` casts) for counts already proven sub-`INT_MAX`.
- **Do not use `unsigned`/`uint32_t` for counts or indices.** The API's error convention is a negative sentinel (`-1`, `-2`, ...), and count fields are compared against it. Unsigned counts re-introduce comparison/wraparound bugs and break that convention. Reserve `uint*_t` for values whose *semantic* is genuinely unsigned (e.g. a UTF-8 code point in `utf8.h`), not merely "this number is never negative."

`int32_t` is `typedef int` on every supported target (Linux, macOS, Windows x64, wasm32), so a broad `int` → `int32_t` rename produces identical codegen and buys nothing at runtime; the only place the distinction matters is the field-offset contract addressed above.

## Document ownership and lifetime for every returned pointer

State explicitly in the doc comment above the declaration: does the caller own the returned pointer (must free it) or borrow it (owned by the library/parent struct)? If borrowed, does it stay valid across the next call into the library (e.g. does a `CarmenCity *` from a lookup survive a subsequent `carmen_city_add_site` on the same city, or a `carmen_world_*` mutation)?

## No inline compiler-specific attributes or pragmas

Confine platform/compiler differences to a single compatibility header, following the `CARMEN_API` pattern in `carmen_export.h`. Don't add a GCC-only `__attribute__` or an MSVC-only `#pragma` inline in a domain header like `city.h`.

## No scattered `#ifdef` platform branches in headers

If platform behavior must differ, funnel it through one macro (like `CARMEN_API`) defined in one place, not repeated `#ifdef _WIN32` blocks across multiple headers.

## A claimed platform is a CI-tested platform

Don't add "supports MSVC 2019+" (or any other platform/compiler claim) to `README.md` unless `.github/workflows/ci.yml` actually builds and runs the test suite on it. See `doc/build-parity.md`.

## Avoid locale-dependent libc calls

Don't rely on `toupper`/`atof`/similar locale-sensitive functions where behavior must be identical across the host locales of Linux CI, a Windows consumer, and a macOS consumer. Prefer the project's own `utf8.h` helpers or explicit, locale-independent logic.
