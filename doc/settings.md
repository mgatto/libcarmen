# Settings File -- Guide for Implementers

This note is for people building front-ends or language bindings on top of
`libcarmen-c`. It explains how the settings file is located and read, and the
common conventions for where a C library/application should look for such a
file.

## The API

```c
CarmenCaseSettings carmen_case_settings_default(void);
int carmen_case_settings_load(CarmenCaseSettings *out, const char *toml_path);
```

- `carmen_case_settings_default()` returns a fully-valid settings struct. Use
  it when you don't want a file at all, or as the base you then override.
- `carmen_case_settings_load(&out, path)` fills `out` with the defaults, then
  applies any keys found in the TOML file at `path`, clamping out-of-range
  values and ignoring unknown keys. It returns `1` on success and `0` on
  failure (NULL args, file cannot be opened, or parse error).

The file format and every key are documented in
[`examples/settings.default.toml`](../examples/settings.default.toml), which is
a copy-and-edit template whose values match `carmen_case_settings_default()`.

## Where does the file have to be?

**Anywhere you want -- the library imposes no location.** `carmen_case_settings_load()`
passes `toml_path` directly to `fopen()`, so standard C path rules apply:

- An **absolute path** (e.g. `/etc/carmen/settings.toml`, `C:\carmen\settings.toml`)
  is used exactly as given.
- A **relative path** (e.g. `settings.toml`, `config/game.toml`) is resolved
  against the **current working directory of the running process** -- i.e.
  wherever the user launched the program from. It is *not* relative to
  `libcarmen.a`/`.dylib`/`.so`, and *not* relative to the executable's own
  directory.

The library does **not** search any default directory, look up an environment
variable, or assume a filename. If you pass `NULL`, no file is read and you get
the compiled-in defaults. **Locating the file is entirely the caller's job.**

This is a deliberate design choice: the core engine stays free of I/O policy so
it can be embedded equally in a CLI, a GUI, a web/WASM build, or a language
binding, each of which has very different ideas about where config belongs.

The bundled demo simply forwards its second CLI argument to the loader:

```sh
# run from the repo root so the relative path resolves
./build/trail_demo en examples/settings.default.toml
```

## Conventions in C for locating a settings file

Since the library leaves the decision to you, here are the conventions a
front-end or binding typically follows. Most robust apps combine several of
these in a precedence order (explicit flag > env var > user dir > system dir >
built-in defaults).

1. **Explicit path from the user (highest precedence).**
   A `--config <path>` flag or an equivalent argument. Always honor this first;
   it makes debugging and scripting trivial. This is what the demo does.

2. **Environment variable override.**
   e.g. `CARMEN_CONFIG=/path/to/settings.toml`. Cheap to implement with
   `getenv()` and very common for servers, containers, and CI.

3. **Per-user configuration directory (the usual default for desktop apps).**
   - **Linux/BSD -- XDG Base Directory spec:** look in
     `$XDG_CONFIG_HOME/<app>/settings.toml`, falling back to
     `~/.config/<app>/settings.toml` when `XDG_CONFIG_HOME` is unset.
   - **macOS:** `~/Library/Application Support/<app>/settings.toml`
     (some tools also just reuse the XDG layout).
   - **Windows:** `%APPDATA%\<app>\settings.toml` (roaming) or
     `%LOCALAPPDATA%\...`. Obtain via `SHGetKnownFolderPath` / `getenv("APPDATA")`.

4. **System-wide configuration (for services / multi-user installs).**
   - **Unix:** `/etc/<app>/settings.toml` (often with a per-user file layered on
     top of the system one).
   - **Windows:** `%PROGRAMDATA%\<app>\`.

5. **Current working directory.**
   Simple and handy for development and self-contained/"portable" apps
   (`./settings.toml`). Fragile for installed software because the CWD is
   whatever the user happened to be in.

6. **Next to the executable ("portable app" style).**
   Resolve the binary's own directory and read a sibling file. This is *not*
   automatic in C; you compute the executable path yourself:
   - Linux: `readlink("/proc/self/exe", ...)`
   - macOS: `_NSGetExecutablePath(...)`
   - Windows: `GetModuleFileNameW(...)`
   Then `dirname()` it and append the filename.

Note that `argv[0]` is **not** a reliable way to find the executable's directory
(it may be a bare name resolved via `PATH`, or spoofed), which is why the
platform-specific calls above are preferred for option 6.

### A typical precedence helper (sketch)

A binding might resolve the path once and hand the result to the library:

```c
/* Pseudocode: first existing candidate wins; NULL => use defaults. */
const char *resolve_settings_path(const char *cli_flag) {
    if (cli_flag)                    return cli_flag;                 /* 1 */
    const char *env = getenv("CARMEN_CONFIG");
    if (env)                         return env;                      /* 2 */
    /* 3: build $XDG_CONFIG_HOME/carmen/settings.toml (or ~/.config/...) */
    /* 4: fall back to /etc/carmen/settings.toml                          */
    return NULL; /* nothing found -> carmen_case_settings_default()     */
}
```

The key point for `libcarmen-c` specifically: implement whichever of these fits
your platform and app style, then pass the final string (or `NULL`) to
`carmen_case_settings_load()`. The library will not second-guess you.
