#define STB_DS_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stb_ds.h"
#include "carmen/game_world.h"
#include "carmen/utf8.h"

/* stb_ds string hash map entry: maps city ID string -> slot in storage[] */
typedef struct {
    char *key;
    int   value;
} CityMapEntry;

/* ------------------------------------------------------------------ helpers */

static CarmenIndexEntry *find_or_create_index(CarmenIndexEntry *index,
                                              int *count, const char *key,
                                              int max_entries)
{
    for (int i = 0; i < *count; i++) {
        if (strcmp(index[i].key, key) == 0)
            return &index[i];
    }
    if (*count >= max_entries)
        return NULL;
    CarmenIndexEntry *entry = &index[(*count)++];
    memset(entry, 0, sizeof(*entry));
    carmen_utf8_copy(entry->key, CARMEN_MAX_NAME_LEN, key);
    return entry;
}

static void index_add(CarmenIndexEntry *entry, int slot)
{
    if (entry && entry->count < CARMEN_MAX_IDS_PER_INDEX)
        entry->slots[entry->count++] = slot;
}

static int city_slot(const CarmenWorld *w, const CarmenCity *c)
{
    return (int)(c - w->storage);
}

static CityMapEntry *map_ptr(CarmenWorld *w)
{
    return (CityMapEntry *)w->city_map;
}

/* -------------------------------------------------------------- lifecycle */

void carmen_world_init(CarmenWorld *w)
{
    if (!w) return;
    memset(w, 0, sizeof(*w));
    w->city_map = NULL;
}

void carmen_world_destroy(CarmenWorld *w)
{
    if (!w) return;
    CityMapEntry *m = map_ptr(w);
    shfree(m);
    w->city_map   = NULL;
    w->city_count = 0;
}

CarmenWorld *carmen_world_create(void)
{
    CarmenWorld *w = malloc(sizeof(*w));
    if (w) carmen_world_init(w);
    return w;
}

void carmen_world_free(CarmenWorld *w)
{
    if (!w) return;
    carmen_world_destroy(w);
    free(w);
}

/* ------------------------------------------------------------- add / find */

CarmenCity *carmen_world_add_city(CarmenWorld *w, const char *id,
                                  const char *name, const char *local_name,
                                  const char *country, const char *continent,
                                  double lat, double lon)
{
    if (!w || !id || !name || !country || !continent)
        return NULL;
    if (w->city_count >= CARMEN_MAX_CITIES)
        return NULL;

    int slot       = w->city_count;
    CarmenCity *c  = &w->storage[slot];
    carmen_city_init(c, id, name, local_name, country, continent, lat, lon);
    w->city_count++;

    CityMapEntry *m = map_ptr(w);
    shput(m, id, slot);
    w->city_map = m;

    CarmenIndexEntry *ci = find_or_create_index(
        w->continent_index, &w->continent_count,
        continent, CARMEN_MAX_INDEX_ENTRIES);
    index_add(ci, slot);

    CarmenIndexEntry *co = find_or_create_index(
        w->country_index, &w->country_count,
        country, CARMEN_MAX_INDEX_ENTRIES);
    index_add(co, slot);

    return c;
}

CarmenCity *carmen_world_find(CarmenWorld *w, const char *id)
{
    if (!w || !id) return NULL;
    CityMapEntry *m = map_ptr(w);
    ptrdiff_t idx = shgeti(m, id);
    if (idx < 0) return NULL;
    return &w->storage[m[idx].value];
}

/* ----------------------------------------------------------- index queries */

int carmen_world_cities_in_continent(CarmenWorld *w, const char *continent,
                                     CarmenCity **out, int max_out)
{
    if (!w || !continent || !out || max_out <= 0) return 0;
    int count = 0;
    for (int i = 0; i < w->continent_count && count < max_out; i++) {
        if (strcmp(w->continent_index[i].key, continent) == 0) {
            CarmenIndexEntry *e = &w->continent_index[i];
            for (int j = 0; j < e->count && count < max_out; j++)
                out[count++] = &w->storage[e->slots[j]];
            break;
        }
    }
    return count;
}

int carmen_world_destinations_from(CarmenWorld *w, const char *city_id,
                                   CarmenCity **out, int max_out)
{
    if (!w || !city_id || !out || max_out <= 0) return 0;
    CarmenCity *c = carmen_world_find(w, city_id);
    if (!c) return 0;

    int count = 0;
    for (int i = 0; i < c->connection_count && count < max_out; i++) {
        CarmenCity *dest = carmen_world_find(
            w, c->connections[i].destination_id);
        if (dest)
            out[count++] = dest;
    }
    return count;
}

/* -------------------------------------------------------------------- BFS */

typedef struct {
    int slot;
    int depth;
} BFSEntry;

int carmen_world_reachable_within(CarmenWorld *w, const char *city_id,
                                  int max_hops,
                                  CarmenCity **out, int max_out)
{
    if (!w || !city_id || !out || max_out <= 0) return 0;
    CarmenCity *origin = carmen_world_find(w, city_id);
    if (!origin) return 0;

    bool     visited[CARMEN_MAX_CITIES] = {false};
    BFSEntry queue[CARMEN_MAX_CITIES];
    int      q_head = 0, q_tail = 0;
    int      result_count = 0;

    int origin_slot      = city_slot(w, origin);
    visited[origin_slot] = true;
    queue[q_tail++]      = (BFSEntry){origin_slot, 0};

    while (q_head < q_tail) {
        BFSEntry cur = queue[q_head++];
        CarmenCity *cc = &w->storage[cur.slot];

        if (cur.slot != origin_slot && result_count < max_out)
            out[result_count++] = cc;

        if (cur.depth < max_hops) {
            for (int i = 0; i < cc->connection_count; i++) {
                CarmenCity *nb = carmen_world_find(
                    w, cc->connections[i].destination_id);
                if (!nb) continue;
                int ns = city_slot(w, nb);
                if (!visited[ns]) {
                    visited[ns] = true;
                    queue[q_tail++] = (BFSEntry){ns, cur.depth + 1};
                }
            }
        }
    }
    return result_count;
}

/* ---------------------------------------------------------- shortest path */

int carmen_world_shortest_path(CarmenWorld *w, const char *from_id,
                               const char *to_id,
                               const char **out_path, int max_path)
{
    if (!w || !from_id || !to_id || !out_path || max_path <= 0)
        return -1;

    CarmenCity *from_city = carmen_world_find(w, from_id);
    CarmenCity *to_city   = carmen_world_find(w, to_id);
    if (!from_city || !to_city) return -1;

    int from_slot = city_slot(w, from_city);
    int to_slot   = city_slot(w, to_city);

    if (from_slot == to_slot) {
        if (max_path >= 1)
            out_path[0] = from_city->id;
        return 0;
    }

    int  pred[CARMEN_MAX_CITIES];
    bool visited[CARMEN_MAX_CITIES] = {false};
    int  queue[CARMEN_MAX_CITIES];
    int  q_head = 0, q_tail = 0;

    memset(pred, -1, sizeof(pred));
    visited[from_slot] = true;
    queue[q_tail++]    = from_slot;

    bool found = false;
    while (q_head < q_tail && !found) {
        int cur_slot = queue[q_head++];
        CarmenCity *cc = &w->storage[cur_slot];

        for (int i = 0; i < cc->connection_count && !found; i++) {
            CarmenCity *nb = carmen_world_find(
                w, cc->connections[i].destination_id);
            if (!nb) continue;
            int ns = city_slot(w, nb);
            if (!visited[ns]) {
                visited[ns] = true;
                pred[ns]    = cur_slot;
                if (ns == to_slot)
                    found = true;
                else
                    queue[q_tail++] = ns;
            }
        }
    }

    if (!found) return -1;

    int path_slots[CARMEN_MAX_CITIES];
    int path_len = 0;
    for (int s = to_slot; s != -1; s = pred[s])
        path_slots[path_len++] = s;

    int out_len = 0;
    for (int i = path_len - 1; i >= 0 && out_len < max_path; i--)
        out_path[out_len++] = w->storage[path_slots[i]].id;

    return out_len - 1;
}

/* ----------------------------------------------------------- clue lookup */

const CarmenClue *carmen_world_random_clue(CarmenWorld *w,
                                           const char *city_id)
{
    if (!w || !city_id) return NULL;
    CarmenCity *c = carmen_world_find(w, city_id);
    if (!c) return NULL;
    return carmen_city_random_clue(c);
}

/* -------------------------------------------------------------- utilities */

int carmen_world_continent_count(const CarmenWorld *w)
{
    if (!w) return 0;
    return w->continent_count;
}

int carmen_world_continents_to_string(const CarmenWorld *w,
                                      char *buf, int buf_size)
{
    if (!w) return 0;
    int total = 0;
    for (int i = 0; i < w->continent_count; i++) {
        int n = snprintf(
            (buf && buf_size > total) ? buf + total : NULL,
            (buf && buf_size > total) ? (size_t)(buf_size - total) : 0,
            "  %s\n", w->continent_index[i].key);
        if (n > 0) total += n;
    }
    return total;
}

void carmen_world_print_continents(const CarmenWorld *w)
{
    if (!w) return;
    char buf[CARMEN_MAX_INDEX_ENTRIES * (CARMEN_MAX_NAME_LEN + 4)];
    carmen_world_continents_to_string(w, buf, (int)sizeof(buf));
    fputs(buf, stdout);
}
