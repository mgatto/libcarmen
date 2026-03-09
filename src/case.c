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

static int in_trail(const char trail[][CARMEN_MAX_NAME_LEN], int len,
                    const char *id)
{
    for (int i = 0; i < len; i++)
        if (strcmp(trail[i], id) == 0)
            return 1;
    return 0;
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

static int build_trail(CarmenWorld *w,
                       char trail[][CARMEN_MAX_NAME_LEN],
                       int target_len, int max_hops)
{
    int start = carmen_random() % w->city_count;
    carmen_utf8_copy(trail[0], CARMEN_MAX_NAME_LEN,
                     w->storage[start].id);

    for (int i = 1; i < target_len; i++) {
        CarmenCity *cur = carmen_world_find(w, trail[i - 1]);
        if (!cur)
            return i;

        CarmenCity *reach[CARMEN_MAX_CITIES];
        int n = carmen_world_reachable_within(w, trail[i - 1], max_hops,
                                              reach, CARMEN_MAX_CITIES);

        /* Tier 1: neighbors with >= 2 sites covering the candidate */
        const char *cand[CARMEN_MAX_CITIES];
        int nc = 0;
        for (int j = 0; j < n; j++)
            if (!in_trail(trail, i, reach[j]->id) &&
                sites_covering(cur, reach[j]->id) >= 2)
                cand[nc++] = reach[j]->id;

        /* Tier 2: neighbors with >= 1 site covering the candidate */
        if (nc == 0)
            for (int j = 0; j < n; j++)
                if (!in_trail(trail, i, reach[j]->id) &&
                    sites_covering(cur, reach[j]->id) >= 1)
                    cand[nc++] = reach[j]->id;

        /* Tier 3: any reachable city not already in trail */
        if (nc == 0)
            for (int j = 0; j < n; j++)
                if (!in_trail(trail, i, reach[j]->id))
                    cand[nc++] = reach[j]->id;

        if (nc == 0)
            return i;

        carmen_utf8_copy(trail[i], CARMEN_MAX_NAME_LEN,
                         cand[carmen_random() % nc]);
    }
    return target_len;
}

/* Sum per-leg travel hours along the trail, respecting each connection's
   transport mode speed. */
static int compute_trail_travel_hrs(CarmenWorld *w,
                                    char trail[][CARMEN_MAX_NAME_LEN],
                                    int trail_len)
{
    int total_hrs = 0;
    for (int i = 1; i < trail_len; i++) {
        CarmenCity *prev = carmen_world_find(w, trail[i - 1]);
        if (!prev) continue;
        for (int c = 0; c < prev->connection_count; c++) {
            if (strcmp(prev->connections[c].destination_id, trail[i]) == 0) {
                total_hrs += carmen_connection_travel_hrs(
                                 &prev->connections[c]);
                break;
            }
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
