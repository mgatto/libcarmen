#ifndef CARMEN_CITY_H
#define CARMEN_CITY_H

#include <stdint.h>
#include "carmen_export.h"
#include "site.h"
#include "connection.h"

#define CARMEN_MAX_SITES       6
#define CARMEN_MAX_CONNECTIONS 10

typedef struct {
    char             id[CARMEN_MAX_NAME_LEN];
    char             name[CARMEN_MAX_NAME_LEN];
    char             local_name[CARMEN_MAX_NAME_LEN];
    char             country[CARMEN_MAX_NAME_LEN];
    char             continent[CARMEN_MAX_NAME_LEN];
    /* Internal cultural sub-sphere grouping id (e.g. "maghreb").  Seed-only
       metadata, not display text and not part of the public API surface;
       no accessors are exported for it.  Empty when unset. */
    char             sphere[CARMEN_MAX_NAME_LEN];
    double           latitude;
    double           longitude;
    CarmenSite       sites[CARMEN_MAX_SITES];
    int32_t          site_count;        /* fixed-width for cross-platform ABI */
    CarmenConnection connections[CARMEN_MAX_CONNECTIONS];
    int32_t          connection_count;  /* fixed-width for cross-platform ABI */
    /* Inbound clue pool: targetless descriptor strings that point to this
       city.  Reused by any source city routing here; a random subset is
       assigned at case generation with target_city_id set at runtime. */
    char             inbound_clues[CARMEN_MAX_INBOUND_CLUES][CARMEN_MAX_CLUE_LEN];
    int32_t          inbound_clue_count; /* fixed-width for cross-platform ABI */
} CarmenCity;

CARMEN_API void              carmen_city_init(CarmenCity *c, const char *id,
                                              const char *name,
                                              const char *local_name,
                                              const char *country,
                                              const char *continent,
                                              double latitude,
                                              double longitude);
CARMEN_API void              carmen_city_add_site(CarmenCity *c,
                                                  const CarmenSite *s);
CARMEN_API void              carmen_city_add_connection(
                                  CarmenCity *c,
                                  const CarmenConnection *conn);
CARMEN_API int               carmen_city_has_connection_to(
                                  const CarmenCity *c,
                                  const char *dest_id);
CARMEN_API int               carmen_city_sites_of_type(
                                  const CarmenCity *c, const char *type,
                                  const CarmenSite **out, int max_out);

/* Append a targetless descriptor clue to this city's inbound pool. */
CARMEN_API void              carmen_city_add_inbound_clue(CarmenCity *c,
                                                          const char *clue_text);
/* Return a random inbound descriptor string, or NULL if the pool is empty or
   c is NULL.  The returned pointer is borrowed: it points into c's inbound
   clue array and remains valid as long as no new inbound clue is added to c
   (i.e. valid for the city's lifetime when the pool is not being mutated).
   Callers must not free the pointer. */
CARMEN_API const char       *carmen_city_random_inbound_clue(
                                  const CarmenCity *c);

/* Write "Name, Country (Continent)" into buf. */
CARMEN_API int  carmen_city_to_string(const CarmenCity *c,
                                      char *buf, int buf_size);
/* Write "<City:id Name, Country (Continent) sites=N connections=N>" into buf. */
CARMEN_API int  carmen_city_detail_to_string(const CarmenCity *c,
                                             char *buf, int buf_size);
CARMEN_API void carmen_city_print(const CarmenCity *c);
CARMEN_API void carmen_city_print_detail(const CarmenCity *c);

#endif
