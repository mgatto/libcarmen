#ifndef CARMEN_GAME_WORLD_H
#define CARMEN_GAME_WORLD_H

#include "carmen/carmen_export.h"
#include "carmen/city.h"

#define CARMEN_MAX_CITIES         64
#define CARMEN_MAX_INDEX_ENTRIES   16
#define CARMEN_MAX_IDS_PER_INDEX   32

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
                                             const char *country,
                                             const char *continent,
                                             double lat, double lon);
CARMEN_API CarmenCity *carmen_world_find(CarmenWorld *w, const char *id);

CARMEN_API int   carmen_world_cities_in_continent(CarmenWorld *w,
                                                   const char *continent,
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

/* Find city by ID, pick a random site, return a clue.
   Returns NULL if the city doesn't exist or has no clues. */
CARMEN_API const CarmenClue *carmen_world_random_clue(CarmenWorld *w,
                                                      const char *city_id);

CARMEN_API int   carmen_world_continent_count(const CarmenWorld *w);

/* Write one "  continent\n" line per continent into buf.  Returns the
   number of characters that would have been written (excluding '\0'). */
CARMEN_API int  carmen_world_continents_to_string(const CarmenWorld *w,
                                                   char *buf, int buf_size);
CARMEN_API void carmen_world_print_continents(const CarmenWorld *w);

#endif
