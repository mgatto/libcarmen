#ifndef CARMEN_SEED_HELPERS_H
#define CARMEN_SEED_HELPERS_H

#include <stddef.h>
#include "carmen/game_world.h"

/* Add an investigation site to a city.  Sites no longer carry clues:
   clues are drawn at case-generation time from destination cities'
   inbound clue pools (see add_inbound_clues below). */
static inline void add_site(CarmenWorld *w, const char *city_id,
                             const char *site_id, const char *name,
                             const char *type)
{
    if (!w || !city_id || !site_id || !name || !type) return;
    CarmenCity *c = carmen_world_find(w, city_id);
    if (!c) return;
    CarmenSite s;
    carmen_site_init(&s, site_id, name, type);
    carmen_city_add_site(c, &s);
}

/* Populate a city's inbound clue pool with targetless descriptor keys.
   These describe (point to) the city and are reused by any source city
   routed here; target_city_id is assigned at runtime. */
static inline void add_inbound_clues(CarmenWorld *w, const char *city_id,
                                     const char *const *keys, int count)
{
    if (!w || !city_id || !keys) return;
    CarmenCity *c = carmen_world_find(w, city_id);
    if (!c) return;
    for (int i = 0; i < count; i++)
        carmen_city_add_inbound_clue(c, keys[i]);
}

static inline void add_route(CarmenWorld *w, const char *from,
                              const char *to, int km, const char *mode)
{
    if (!w || !from || !to || !mode) return;
    CarmenCity *fc = carmen_world_find(w, from);
    CarmenCity *tc = carmen_world_find(w, to);
    if (!fc || !tc) return;

    CarmenConnection conn;
    carmen_connection_init(&conn, to, km, mode);
    carmen_city_add_connection(fc, &conn);

    if (!carmen_city_has_connection_to(tc, from)) {
        carmen_connection_init(&conn, from, km, mode);
        carmen_city_add_connection(tc, &conn);
    }
}

#endif
