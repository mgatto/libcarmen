#include <stdbool.h>
#include <string.h>

#include "carmen/clue.h"
#include "carmen/connection.h"
#include "carmen/game_world.h"

#define CARMEN_TARGET_DEGREE 3
#define CARMEN_GEN_MAX_RESTARTS 100
#define CARMEN_GEN_MAX_MATCH_STEPS 4096

static void shuffle_ints(int *arr, int n)
{
    for (int j = n - 1; j > 0; j--) {
        int k   = carmen_random() % (j + 1);
        int tmp = arr[j];
        arr[j]  = arr[k];
        arr[k]  = tmp;
    }
}

static int add_flight_edge(CarmenWorld *w, int ia, int ib)
{
    if (ia == ib || ia < 0 || ib < 0) return 0;
    if (ia >= w->city_count || ib >= w->city_count) return 0;

    CarmenCity *a = &w->storage[ia];
    CarmenCity *b = &w->storage[ib];
    if (carmen_city_has_connection_to(a, b->id)) return 0;
    if (a->connection_count >= CARMEN_TARGET_DEGREE) return 0;
    if (b->connection_count >= CARMEN_TARGET_DEGREE) return 0;

    int km = carmen_geo_distance_km(a->latitude, a->longitude,
                                    b->latitude, b->longitude);
    CarmenConnection conn;
    carmen_connection_init(&conn, b->id, km, "flight");
    carmen_city_add_connection(a, &conn);
    carmen_connection_init(&conn, a->id, km, "flight");
    carmen_city_add_connection(b, &conn);
    return 1;
}

static int same_sphere(const CarmenCity *a, const CarmenCity *b)
{
    if (!a->sphere[0] || !b->sphere[0]) return 0;
    return strcmp(a->sphere, b->sphere) == 0;
}

static int graph_is_connected(CarmenWorld *w)
{
    if (w->city_count <= 1) return 1;

    bool visited[CARMEN_MAX_CITIES] = {false};
    int  queue[CARMEN_MAX_CITIES];
    int  qh = 0, qt = 0;
    int  seen = 0;

    visited[0] = true;
    queue[qt++] = 0;
    seen = 1;

    while (qh < qt) {
        int slot = queue[qh++];
        CarmenCity *c = &w->storage[slot];
        for (int i = 0; i < c->connection_count; i++) {
            CarmenCity *nb = carmen_world_find(w, c->connections[i].destination_id);
            if (!nb) continue;
            int ns = (int)(nb - w->storage);
            if (ns < 0 || ns >= w->city_count) continue;
            if (!visited[ns]) {
                visited[ns] = true;
                queue[qt++] = ns;
                seen++;
            }
        }
    }
    return seen == w->city_count;
}

static int degrees_all_target(const CarmenWorld *w)
{
    for (int i = 0; i < w->city_count; i++)
        if (w->storage[i].connection_count != CARMEN_TARGET_DEGREE)
            return 0;
    return 1;
}

static void seed_in_sphere(CarmenWorld *w)
{
    int used[CARMEN_MAX_CITIES] = {0};

    for (int i = 0; i < w->city_count; i++) {
        if (used[i]) continue;
        if (!w->storage[i].sphere[0]) continue;

        int members[CARMEN_MAX_CITIES];
        int nm = 0;
        for (int j = 0; j < w->city_count; j++) {
            if (strcmp(w->storage[i].sphere, w->storage[j].sphere) == 0)
                members[nm++] = j;
        }
        for (int k = 0; k < nm; k++)
            used[members[k]] = 1;

        shuffle_ints(members, nm);
        if (nm == 3) {
            add_flight_edge(w, members[0], members[1]);
        } else if (nm >= 4) {
            /* Matching: pair (0,1), (2,3), ... leaving at most one leftover. */
            int pairs = nm / 2;
            if (pairs > 2) pairs = 2; /* arab_east of 4 -> two edges */
            for (int p = 0; p < pairs; p++)
                add_flight_edge(w, members[2 * p], members[2 * p + 1]);
        }
    }
}

static int match_cross_sphere(CarmenWorld *w)
{
    int remaining[CARMEN_MAX_CITIES];
    for (int i = 0; i < w->city_count; i++) {
        remaining[i] = CARMEN_TARGET_DEGREE - w->storage[i].connection_count;
        if (remaining[i] < 0) return 0;
    }

    for (int step = 0; step < CARMEN_GEN_MAX_MATCH_STEPS; step++) {
        int total = 0;
        for (int i = 0; i < w->city_count; i++)
            total += remaining[i];
        if (total == 0) return 1;
        if (total == 1) return 0;

        int a_cands[CARMEN_MAX_CITIES];
        int na = 0;
        for (int i = 0; i < w->city_count; i++)
            if (remaining[i] > 0)
                a_cands[na++] = i;
        if (na == 0) return 1;

        int a = a_cands[carmen_random() % na];

        int b_cands[CARMEN_MAX_CITIES];
        int nb = 0;
        for (int j = 0; j < w->city_count; j++) {
            if (j == a || remaining[j] <= 0) continue;
            if (same_sphere(&w->storage[a], &w->storage[j])) continue;
            if (carmen_city_has_connection_to(&w->storage[a], w->storage[j].id))
                continue;
            b_cands[nb++] = j;
        }
        if (nb == 0) return 0;

        int b = b_cands[carmen_random() % nb];
        if (!add_flight_edge(w, a, b)) return 0;
        remaining[a]--;
        remaining[b]--;
    }
    return 0;
}

static int try_generate(CarmenWorld *w)
{
    carmen_world_clear_connections(w);
    seed_in_sphere(w);
    if (!match_cross_sphere(w)) return 0;
    if (!degrees_all_target(w)) return 0;
    if (!graph_is_connected(w)) return 0;
    return 1;
}

/*
 * Last-resort 3-regular connected graph: a Hamiltonian cycle plus a
 * perfect matching of opposite vertices. Ignores spheres; always
 * succeeds for even n >= 4.
 */
static void generate_cycle_plus_matching(CarmenWorld *w)
{
    const int n = w->city_count;
    carmen_world_clear_connections(w);
    for (int i = 0; i < n; i++)
        add_flight_edge(w, i, (i + 1) % n);
    const int hop = n / 2;
    for (int i = 0; i < hop; i++)
        add_flight_edge(w, i, i + hop);
}

void carmen_world_generate_connections(CarmenWorld *w)
{
    if (!w) return;
    /* A simple 3-regular graph exists only for even n >= 4. */
    if (w->city_count < 4 || (w->city_count % 2) != 0)
        return;

    for (int r = 0; r < CARMEN_GEN_MAX_RESTARTS; r++) {
        if (try_generate(w))
            return;
    }
    generate_cycle_plus_matching(w);
}
