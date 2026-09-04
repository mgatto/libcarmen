# Clean API Checklist

"Clean API" isn't a formally standardized term, but for `libcarmen` it means: a public interface with a minimal, consistently-named surface of plain-data types and pure/side-effecting functions that are clearly distinguished, fully documented for ownership/lifetime/error semantics, and free of leaked implementation, platform, or dependency details -- such that a caller (especially one in another language, per the [Project Goal](../README.md#project-goal)) can use it correctly from the header alone, and it never crashes on bad input.

This doc is a concrete checklist for auditing any header under `include/carmen/` -- when adding a new module or reviewing a change to an existing one, walk through each item below. It complements, rather than replaces, `AGENTS.md` and `.cursor/rules/c-safety.mdc` / `.cursor/rules/cross-platform-abi.mdc`, which state most of these rules normatively; this doc explains the intent and points at the codebase's own examples.

## 1. Minimal, well-defined surface area

Only what callers need is exposed; everything else stays internal, with no accessor exported for it. Example: `CarmenCity.sphere` in [`include/carmen/city.h`](../include/carmen/city.h) is explicitly commented as "Seed-only metadata... not part of the public API surface; no accessors are exported for it."

- [ ] Every new public symbol is declared in the matching `include/carmen/<module>.h` and marked `CARMEN_API`.
- [ ] Internal-only fields or helpers have no exported accessor, and are commented as such if their presence in a public struct might otherwise look like an oversight.

## 2. Plain data, opaque behavior

Domain structs (`CarmenCity`, `CarmenSite`, `CarmenConnection`, ...) stay plain data with no embedded bookkeeping for whatever container holds them. Example: `CarmenCity` structs live in `GameWorld.storage[]`, but the hash map stores `{key, slot_index}` entries, not the cities themselves -- see "Key Design Decisions" in the [README](../README.md#key-design-decisions).

- [ ] A new struct does not embed hash-map, list, or other container bookkeeping fields.
- [ ] Code outside the owning module's `.c` file never reaches into a container's internals (e.g. `GameWorld.storage[]`) to get at a struct directly.

## 3. Consistent, predictable naming and shape

One naming scheme, applied without exception, so callers can guess a symbol's name and signature correctly: `Carmen`-prefixed `PascalCase` types, `carmen_<module>_` `snake_case` functions, `CARMEN_` `SCREAMING_SNAKE_CASE` constants and macros (`AGENTS.md`'s "Naming" section).

- [ ] New types, functions, and constants follow the existing prefix/case conventions exactly.
- [ ] A function operating on `CarmenFoo` is named `carmen_foo_...`, not `carmen_do_foo_...` or similar.

## 4. Clear separation of queries and actions

A function's name and signature should tell you whether it can mutate state. Example: [`include/carmen/session.h`](../include/carmen/session.h) groups functions under `/* Queries (read-only, for UI) */` and documents `carmen_session_travel`, `carmen_session_investigate`, `carmen_session_issue_warrant`, and `carmen_session_arrest` as the only state-mutating actions.

- [ ] Read-only functions take `const` pointers and are grouped/commented as queries.
- [ ] State-mutating functions are named as verbs/actions and are not mixed in with query accessors without a clear separating comment.

## 5. Explicit, documented ownership and lifetime

Every returned pointer's doc comment states whether the caller owns it (must free it) or borrows it (owned by the library/parent struct), and if borrowed, how long it stays valid. Example: `carmen_session_investigate` in [`include/carmen/session.h`](../include/carmen/session.h) documents "The returned pointer points into the session's notebook and remains valid for the lifetime of the session."

- [ ] Every function returning a pointer has a doc comment stating ownership (caller-owned vs. borrowed).
- [ ] If borrowed, the comment states the pointer's valid lifetime (e.g. "for the lifetime of the world", "until the next call to X").

## 6. Graceful, structured error handling

The library never `assert()`/`abort()`/crashes on bad *caller* input -- every public function guards NULL/range input at the top and fails gracefully, using a distinct negative return code per failure reason (not one collapsed `-1` for everything). Example: `carmen_session_travel` in [`include/carmen/session.h`](../include/carmen/session.h) documents four distinct codes (`-1` no connection, `-2` out of time, `-3` not PLAYING, `-4` move limit reached).

- [ ] Every public function guards NULL/invalid arguments at the top and returns a documented failure value instead of crashing.
- [ ] Distinct failure reasons get distinct return codes, each documented at the declaration.
- [ ] `assert()` is reserved for internal invariants (bugs in the library itself), never for validating caller input.

## 7. No leaked dependencies or platform differences

Compiler/platform quirks are funneled through one seam, never scattered `#ifdef`s or vendored types in public headers. Example: [`include/carmen/carmen_export.h`](../include/carmen/carmen_export.h) is the single place that branches on `_WIN32`/`__GNUC__` to define `CARMEN_API`; no other public header touches platform macros directly.

- [ ] No public header contains a `#ifdef _WIN32` (or similar) outside `carmen_export.h`.
- [ ] No public header exposes a vendored type (`stb_ds`, `cJSON`, `toml-c`) directly in a struct or function signature.

## 8. ABI stability for cross-language use

Struct fields a binding might read by offset use fixed-width types (`int32_t`, not bare `int`), since width isn't guaranteed across platforms/compilers -- see `.cursor/rules/cross-platform-abi.mdc`.

- [ ] New struct fields intended for field-by-field introspection from a binding use `<stdint.h>` fixed-width types.
- [ ] No inline compiler-specific attribute or pragma is added to a domain header (`city.h`, `session.h`, ...) -- only to `carmen_export.h`.

## 9. No embedded UI or I/O

The core exposes mechanism, not policy: no `printf`-driven prompts, terminal control, or other UI/I/O baked into library logic (aside from the explicitly-named `_print` debug helpers, e.g. `carmen_city_print` in [`include/carmen/city.h`](../include/carmen/city.h)). Rendering and input belong to a front-end like `examples/trail_demo.c`.

- [ ] New library logic doesn't call `printf`/`scanf`/terminal control functions except in a function whose name makes clear it's a debug/demo print helper.
- [ ] A front-end feature request is satisfied by adding a query/action to the core API, not by reaching around it.

## 10. Documented at the point of use

Every public function has a short `/* ... */` doc comment above its declaration describing behavior, ownership of any returned pointer, and the meaning of any non-obvious return value -- so the header alone is sufficient documentation without reading the implementation. Nearly every declaration in [`include/carmen/session.h`](../include/carmen/session.h) follows this pattern; use it as the reference example when documenting a new function.

- [ ] Every new/changed public function has a doc comment covering behavior, pointer ownership (if applicable), and return-value meaning.
- [ ] The umbrella header [`include/carmen/carmen.h`](../include/carmen/carmen.h) and both `Makefile` (`PUBLIC_HEADERS`, `LIB_SRCS`) and `CMakeLists.txt` are updated when a new module's header is added.
