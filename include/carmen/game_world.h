#ifndef CARMEN_GAME_WORLD_H
#define CARMEN_GAME_WORLD_H

#include "carmen_export.h"
#include "city.h"

#define CARMEN_MAX_CITIES         32
#define CARMEN_MAX_INDEX_ENTRIES   32
#define CARMEN_MAX_IDS_PER_INDEX   16

typedef struct {
    char key[CARMEN_MAX_NAME_LEN];
    int  slots[CARMEN_MAX_IDS_PER_INDEX];
    int  count;
} CarmenIndexEntry;

typedef struct {
    CarmenCity       storage[CARMEN_MAX_CITIES];
    int              city_count;
    void            *city_map;   /* opaque; backed by stb_ds string hash map */

    CarmenIndexEntry continent_index[CARMEN_MAX_INDEX_ENTRIES];
    int              continent_count;
    CarmenIndexEntry country_index[CARMEN_MAX_INDEX_ENTRIES];
    int              country_count;
} CarmenWorld;

CARMEN_API void         carmen_world_init(CarmenWorld *w);
CARMEN_API void         carmen_world_destroy(CarmenWorld *w);

/* Heap-allocate a CarmenWorld and initialise it.  Returns NULL on failure.
   Prefer this over stack-declaring CarmenWorld (which is ~1.4 MB) in
   WASM, threaded, or embedded environments.  Free with carmen_world_free(). */
CARMEN_API CarmenWorld *carmen_world_create(void);
CARMEN_API void         carmen_world_free(CarmenWorld *w);
CARMEN_API CarmenCity *carmen_world_add_city(CarmenWorld *w, const char *id,
                                             const char *name,
                                             const char *local_name,
                                             const char *country,
                                             const char *continent,
                                             double lat, double lon);
CARMEN_API CarmenCity *carmen_world_find(CarmenWorld *w, const char *id);

CARMEN_API int   carmen_world_cities_in_continent(CarmenWorld *w,
                                                   const char *continent,
                                                   CarmenCity **out,
                                                   int max_out);
CARMEN_API int   carmen_world_cities_in_country(CarmenWorld *w,
                                                  const char *country,
                                                  CarmenCity **out,
                                                  int max_out);
CARMEN_API int   carmen_world_destinations_from(CarmenWorld *w,
                                                const char *city_id,
                                                CarmenCity **out,
                                                int max_out);
CARMEN_API int   carmen_world_reachable_within(CarmenWorld *w,
                                               const char *city_id,
                                               int max_hops,
                                               CarmenCity **out,
                                               int max_out);

/* Returns hop count (path_len - 1), or -1 if no path.
   Writes city ID strings into out_path[]. */
CARMEN_API int   carmen_world_shortest_path(CarmenWorld *w,
                                            const char *from_id,
                                            const char *to_id,
                                            const char **out_path,
                                            int max_path);

/*
 * Wipe every city's connection list. Used before regenerating a per-case
 * graph and when restoring a saved graph onto a world. Does not touch
 * cities, sites, or inbound clue pools.
 */
CARMEN_API void carmen_world_clear_connections(CarmenWorld *w);

/*
 * Build a connected 3-regular undirected flight graph over w's cities.
 * Sphere-aware configuration model with restart: in-sphere seeding (~1
 * edge per city), then cross-sphere stub matching. Every edge is mode
 * "flight" with haversine kilometres. Uses carmen_random() so each call
 * yields a fresh map.
 *
 * No-ops (leaves existing connections) when the city count cannot support
 * a simple 3-regular graph: fewer than 4 cities, or an odd count.
 * Ownership of w is unchanged.
 */
CARMEN_API void carmen_world_generate_connections(CarmenWorld *w);

CARMEN_API int   carmen_world_continent_count(const CarmenWorld *w);

/* Write one "  continent\n" line per continent into buf.  Returns the
   number of characters that would have been written (excluding '\0'). */
CARMEN_API int  carmen_world_continents_to_string(const CarmenWorld *w,
                                                   char *buf, int buf_size);
CARMEN_API void carmen_world_print_continents(const CarmenWorld *w);

#endif
