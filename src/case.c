#include <stdbool.h>
#include <string.h>
#include "carmen/case.h"
#include "carmen/utf8.h"

#define CASE_MAX_RETRIES 20

static int trail_length_for(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return 3;
        case CARMEN_DIFFICULTY_MEDIUM: return 5;
        case CARMEN_DIFFICULTY_HARD:   return 7;
    }
    return 5;
}

static int default_hops_for(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return 1;
        case CARMEN_DIFFICULTY_MEDIUM: return 2;
        case CARMEN_DIFFICULTY_HARD:   return 3;
    }
    return 1;
}

/* Returns the time budget in hours. Base allowance by difficulty plus
   the estimated travel time along the trail (accounting for each leg's
   transport mode: flight 800 kph, train 200 kph, boat 60 kph). */
static int time_budget_hrs_for(CarmenDifficulty d, int trail_travel_hrs)
{
    int base;
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   base = 168; break; /* 7 days */
        case CARMEN_DIFFICULTY_MEDIUM: base = 120; break; /* 5 days */
        case CARMEN_DIFFICULTY_HARD:   base =  72; break; /* 3 days */
        default:                       base = 120; break;
    }
    return base + trail_travel_hrs;
}

static int sites_covering(const CarmenCity *city, const char *target)
{
    int count = 0;
    for (int s = 0; s < city->site_count; s++) {
        const CarmenSite *site = &city->sites[s];
        for (int c = 0; c < site->clue_count; c++) {
            if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
                strcmp(site->clues[c].target_city_id, target) == 0) {
                count++;
                break;
            }
        }
    }
    return count;
}

static int city_slot(const CarmenWorld *w, const CarmenCity *c)
{
    return (int)(c - w->storage);
}

static int build_trail(CarmenWorld *w,
                       char trail[][CARMEN_MAX_NAME_LEN],
                       int target_len, int max_hops)
{
    int start = carmen_random() % w->city_count;
    CarmenCity *cur = &w->storage[start];
    carmen_utf8_copy(trail[0], CARMEN_MAX_NAME_LEN, cur->id);

    bool on_trail[CARMEN_MAX_CITIES] = {false};
    on_trail[start] = true;

    for (int i = 1; i < target_len; i++) {
        CarmenCity *reach[CARMEN_MAX_CITIES];
        int n = carmen_world_reachable_within(w, cur->id, max_hops,
                                              reach, CARMEN_MAX_CITIES);

        /* Single pass: compute coverage and filter out trail duplicates */
        int coverage[CARMEN_MAX_CITIES];
        int valid[CARMEN_MAX_CITIES];
        int nv = 0;
        for (int j = 0; j < n; j++) {
            if (on_trail[city_slot(w, reach[j])])
                continue;
            coverage[nv] = sites_covering(cur, reach[j]->id);
            valid[nv]    = j;
            nv++;
        }

        if (nv == 0)
            return i;

        /* Pick from best available tier */
        const char *cand[CARMEN_MAX_CITIES];
        int nc = 0;

        for (int j = 0; j < nv; j++)
            if (coverage[j] >= 2)
                cand[nc++] = reach[valid[j]]->id;

        if (nc == 0)
            for (int j = 0; j < nv; j++)
                if (coverage[j] >= 1)
                    cand[nc++] = reach[valid[j]]->id;

        if (nc == 0)
            for (int j = 0; j < nv; j++)
                cand[nc++] = reach[valid[j]]->id;

        const char *chosen = cand[carmen_random() % nc];
        carmen_utf8_copy(trail[i], CARMEN_MAX_NAME_LEN, chosen);

        cur = carmen_world_find(w, chosen);
        if (!cur)
            return i;
        on_trail[city_slot(w, cur)] = true;
    }
    return target_len;
}

/* Travel hours for a single direct connection between two cities. */
static int direct_leg_hrs(const CarmenCity *from, const char *to_id)
{
    for (int c = 0; c < from->connection_count; c++)
        if (strcmp(from->connections[c].destination_id, to_id) == 0)
            return carmen_connection_travel_hrs(&from->connections[c]);
    return 0;
}

/* Sum travel hours along the trail. For multi-hop legs (consecutive trail
   cities not directly connected), walks the shortest path between them
   and sums each hop's travel time. */
static int compute_trail_travel_hrs(CarmenWorld *w,
                                    char trail[][CARMEN_MAX_NAME_LEN],
                                    int trail_len)
{
    int total_hrs = 0;
    for (int i = 1; i < trail_len; i++) {
        CarmenCity *prev = carmen_world_find(w, trail[i - 1]);
        if (!prev) continue;

        int hrs = direct_leg_hrs(prev, trail[i]);
        if (hrs > 0) {
            total_hrs += hrs;
            continue;
        }

        /* Not directly connected -- walk shortest path */
        const char *path[CARMEN_MAX_CITIES];
        int hops = carmen_world_shortest_path(w, trail[i - 1], trail[i],
                                              path, CARMEN_MAX_CITIES);
        if (hops <= 0) continue;
        for (int h = 0; h < hops; h++) {
            CarmenCity *step = carmen_world_find(w, path[h]);
            if (step)
                total_hrs += direct_leg_hrs(step, path[h + 1]);
        }
    }
    return total_hrs;
}

int carmen_case_generate(CarmenCase *c, CarmenWorld *w,
                         const CarmenCaseSettings *settings)
{
    if (!c || !w || !settings) return 0;

    memset(c, 0, sizeof(*c));

    CarmenDifficulty diff = settings->difficulty;
    int hops = settings->max_trail_hops > 0
             ? settings->max_trail_hops
             : default_hops_for(diff);
    int target_len = trail_length_for(diff);

    if (target_len > CARMEN_MAX_TRAIL)
        target_len = CARMEN_MAX_TRAIL;

    int built = 0;
    for (int attempt = 0; attempt < CASE_MAX_RETRIES; attempt++) {
        built = build_trail(w, c->trail, target_len, hops);
        if (built == target_len)
            break;
    }
    if (built < target_len)
        return 0;

    c->trail_len      = target_len;
    c->difficulty      = diff;
    c->max_trail_hops  = hops;
    carmen_utf8_copy(c->origin_id, CARMEN_MAX_NAME_LEN, c->trail[0]);
    carmen_utf8_copy(c->hideout_id, CARMEN_MAX_NAME_LEN,
                     c->trail[target_len - 1]);

    c->villain = &FITNA_VILLAINS[carmen_random() % FITNA_VILLAIN_COUNT];

    int art_idx = carmen_random() % CARMEN_ARTIFACT_COUNT;
    c->artifact = CARMEN_ARTIFACTS[art_idx];

    int travel_hrs = compute_trail_travel_hrs(w, c->trail, c->trail_len);
    c->time_budget_hrs = time_budget_hrs_for(diff, travel_hrs);

    return 1;
}
