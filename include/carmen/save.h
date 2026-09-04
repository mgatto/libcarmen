#ifndef CARMEN_SAVE_H
#define CARMEN_SAVE_H

#include <stddef.h>
#include "carmen_export.h"
#include "session.h"

/* Save schema version emitted by carmen_session_save(). carmen_session_load()
   accepts this exact version and rejects anything else with -3. Version 3
   drops the obsolete "hideout_investigated_sites" field (identity evidence is
   now seeded across the trail as CARMEN_CLUE_IDENTITY clues) and persists the
   per-case generated connection graph as an "edges" array; v1/v2 saves are
   rejected. */
#define CARMEN_SAVE_SCHEMA_VERSION 3

/* Upper bound on a save file carmen_session_load_file() will read, as a guard
   against pathological inputs. Comfortably larger than any real session, whose
   serialization is a few KB. */
#define CARMEN_SAVE_MAX_FILE_SIZE (1024 * 1024)

/*
 * Serialize a session to a JSON string (schema CARMEN_SAVE_SCHEMA_VERSION).
 *
 * Follows the snprintf / carmen_*_to_string convention: writes at most
 * buf_size bytes into buf (always NUL-terminating when buf is non-NULL and
 * buf_size > 0) and returns the number of bytes that would have been written
 * excluding the NUL. Pass a NULL buf (or buf_size 0) to query the required
 * length, then allocate len + 1 bytes and call again. This buffer-based shape
 * (rather than returning a malloc'd string) avoids handing callers memory they
 * would have to free with a matching allocator across an FFI/DLL boundary.
 *
 * Returns 0 if s is NULL or serialization fails (e.g. out of memory), in which
 * case buf is only touched to write a NUL terminator.
 *
 * The world pointer is NOT serialized; the generated connection graph is
 * stored as an "edges" array of {from, to, km, mode} (each undirected pair
 * once). On load the caller re-attaches a world (see carmen_session_load)
 * and that world's connections are rewritten from the saved edges. Only
 * string ids for the villain, artifact and cities are stored, so a save is
 * portable across processes as long as it is loaded against a world
 * containing the same city ids and the same built-in villain/artifact
 * catalogs.
 */
CARMEN_API int carmen_session_save(const CarmenSession *s, char *buf, int buf_size);

/*
 * Serialize a session and write it to path (see carmen_session_save for the
 * format). Overwrites any existing file.
 *
 * Returns 1 on success, or a negative code on failure:
 *   -1  s or path is NULL
 *   -2  serialization failed (out of memory)
 *   -3  the file could not be opened for writing
 *   -4  a write error occurred
 */
CARMEN_API int carmen_session_save_file(const CarmenSession *s, const char *path);

/*
 * Load a session from a JSON buffer produced by carmen_session_save().
 *
 * On success *s is fully overwritten with the restored session and its world
 * pointer is set to w; the case, trail and current-city ids are resolved
 * against w at play time, so w must contain the same city ids the save was
 * made against. The villain and artifact are re-resolved from the built-in
 * catalogs by id. The world's connection graph is cleared and rebuilt from
 * the saved "edges" array. On any failure *s is left unchanged.
 *
 * len is the number of bytes in json (json need not be NUL-terminated).
 *
 * Returns 1 on success, or a negative code on failure:
 *   -1  s, w or json is NULL
 *   -2  json is not valid JSON, or its root is not an object
 *   -3  missing or unsupported schema_version
 *   -4  a required field is missing, malformed, or out of range
 *   -5  the stored villain id is not in the built-in catalog
 *   -6  the stored artifact id is not in the built-in catalog
 *   -7  a referenced city id (current city or a trail city) is not in w
 */
CARMEN_API int carmen_session_load(CarmenSession *s, CarmenWorld *w, const char *json, size_t len);

/*
 * Load a session from a file written by carmen_session_save_file().
 *
 * Behaves like carmen_session_load() (same success semantics and the same
 * -1..-7 codes for the file's contents), with two additional file-level codes:
 *   -8  s or w is NULL, path is NULL, or the file could not be opened/read
 *   -9  the file is empty or larger than CARMEN_SAVE_MAX_FILE_SIZE
 */
CARMEN_API int carmen_session_load_file(CarmenSession *s, CarmenWorld *w, const char *path);

#endif
