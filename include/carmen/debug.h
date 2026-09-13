#ifndef CARMEN_DEBUG_H
#define CARMEN_DEBUG_H

/*
 * Debug print helpers.
 *
 * These are the library's only "mechanism that also does I/O": each writes a
 * human-readable representation of a domain struct to stdout, strictly as a
 * debugging aid (see doc/clean-api-checklist.md, "No embedded UI or I/O").
 *
 * Deliberately NOT included by the umbrella carmen.h -- a release consumer
 * keeps a clean, print-free API surface unless it opts in by including this
 * header explicitly.  The functions themselves are always compiled and
 * exported, so including this header is enough to call them in any build
 * (including a release build); there is no macro to gate or disable them.
 */

#include "carmen_export.h"
#include "site.h"
#include "connection.h"
#include "city.h"
#include "game_world.h"

/** Debug helper: print the site to stdout. */
CARMEN_API void carmen_site_print(const CarmenSite *s);

/** Debug helper: print the connection to stdout. */
CARMEN_API void carmen_connection_print(const CarmenConnection *c);

/** Debug helper: print the city to stdout. */
CARMEN_API void carmen_city_print(const CarmenCity *c);

/** Debug helper: print the city's details to stdout. */
CARMEN_API void carmen_city_print_detail(const CarmenCity *c);

/** Debug helper: print the world's continent list to stdout. */
CARMEN_API void carmen_world_print_continents(const CarmenWorld *w);

#endif
