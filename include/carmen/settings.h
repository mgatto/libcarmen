#ifndef CARMEN_SETTINGS_H
#define CARMEN_SETTINGS_H

#include "carmen_export.h"
#include "case.h"

/*
 * Return a fully-populated, valid CarmenCaseSettings with sensible
 * defaults (medium difficulty, derived trail/time, derived active sites
 * and positive-clue ratio from difficulty, unlimited moves).
 */
CARMEN_API CarmenCaseSettings carmen_case_settings_default(void);

/*
 * Load settings from a human-editable TOML file.
 *
 * *out is first filled with carmen_case_settings_default(), then any
 * keys present in the file override the defaults. Out-of-range values
 * are clamped into their valid ranges (a warning is printed to stderr).
 * Unknown keys are ignored.
 *
 * PATH RESOLUTION: toml_path is passed straight to fopen(). An absolute
 * path is used as-is; a relative path is resolved against the running
 * process's current working directory -- NOT the location of the library
 * or of the calling executable. The library performs no search of any
 * default directory. Choosing where the file lives (a --config flag, an
 * environment variable, an OS config directory, etc.) is the caller's
 * responsibility; see doc/settings.md for common conventions.
 *
 * Returns 1 on success. Returns 0 (leaving *out untouched) if out or
 * toml_path is NULL, the file cannot be opened, or it fails to parse.
 */
CARMEN_API int carmen_case_settings_load(CarmenCaseSettings *out,
                                         const char *toml_path);

#endif
