#ifndef CARMEN_SEED_HELPERS_H
#define CARMEN_SEED_HELPERS_H

#include <stddef.h>
#include "carmen/game_world.h"

typedef struct {
    const char    *text;
    const char    *target;
    CarmenClueType type;     /* 0 = CARMEN_CLUE_POSITIVE when omitted */
} ClueData;

static inline void add_site(CarmenWorld *w, const char *city_id,
                             const char *name, const char *type,
                             const ClueData *clues, int clue_count)
{
    if (!w || !city_id || !name || !type || !clues || clue_count <= 0) return;
    CarmenCity *c = carmen_world_find(w, city_id);
    if (!c) return;
    CarmenSite s;
    carmen_site_init(&s, name, type);
    for (int i = 0; i < clue_count; i++)
        carmen_site_add_clue(&s, clues[i].text, clues[i].target, clues[i].type);
    carmen_city_add_site(c, &s);
}

static inline void add_route(CarmenWorld *w, const char *from,
                              const char *to, int km)
{
    if (!w || !from || !to) return;
    CarmenCity *fc = carmen_world_find(w, from);
    CarmenCity *tc = carmen_world_find(w, to);
    if (!fc || !tc) return;

    CarmenConnection conn;
    carmen_connection_init(&conn, to, km, "flight");
    carmen_city_add_connection(fc, &conn);

    if (!carmen_city_has_connection_to(tc, from)) {
        carmen_connection_init(&conn, from, km, "flight");
        carmen_city_add_connection(tc, &conn);
    }
}

#endif
