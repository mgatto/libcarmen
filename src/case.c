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

static int time_budget_hrs_for(CarmenDifficulty d, int trail_travel_hrs)
{
    int base;
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   base = 168; break;
        case CARMEN_DIFFICULTY_MEDIUM: base = 120; break;
        case CARMEN_DIFFICULTY_HARD:   base =  72; break;
        default:                       base = 120; break;
    }
    return base + trail_travel_hrs;
}

static int city_slot(const CarmenWorld *w, const CarmenCity *c)
{
    return (int)(c - w->storage);
}

/* Build a trail of directly-connected cities (max_hops = 1). */
static int build_trail(CarmenWorld *w,
                       char trail[][CARMEN_MAX_NAME_LEN],
                       int target_len)
{
    int start = carmen_random() % w->city_count;
    CarmenCity *cur = &w->storage[start];
    carmen_utf8_copy(trail[0], CARMEN_MAX_NAME_LEN, cur->id);

    bool on_trail[CARMEN_MAX_CITIES] = {false};
    on_trail[start] = true;

    for (int i = 1; i < target_len; i++) {
        CarmenCity *reach[CARMEN_MAX_CITIES];
        int n = carmen_world_reachable_within(w, cur->id, 1,
                                              reach, CARMEN_MAX_CITIES);

        const char *cand[CARMEN_MAX_CITIES];
        int nc = 0;
        for (int j = 0; j < n; j++) {
            if (!on_trail[city_slot(w, reach[j])])
                cand[nc++] = reach[j]->id;
        }

        if (nc == 0)
            return i;

        const char *chosen = cand[carmen_random() % nc];
        carmen_utf8_copy(trail[i], CARMEN_MAX_NAME_LEN, chosen);

        cur = carmen_world_find(w, chosen);
        if (!cur)
            return i;
        on_trail[city_slot(w, cur)] = true;
    }
    return target_len;
}

static int site_has_positive_to(const CarmenSite *s, const char *target)
{
    for (int k = 0; k < s->clue_count; k++)
        if (s->clues[k].type == CARMEN_CLUE_POSITIVE &&
            strcmp(s->clues[k].target_city_id, target) == 0)
            return 1;
    return 0;
}

static void shuffle(int *arr, int n)
{
    for (int j = n - 1; j > 0; j--) {
        int k = carmen_random() % (j + 1);
        int tmp = arr[j]; arr[j] = arr[k]; arr[k] = tmp;
    }
}

/*
 * For each trail stop, select up to CARMEN_TRAIL_SITES sites and assign
 * one deterministic clue per site.  Non-hideout stops get 2 positive
 * clues pointing to the next trail city and 1 herring or negative.
 * Sites with matching clues are preferred for the positive slots.
 * The hideout stop stores site indices only (evidence, not clues).
 */
static void assign_trail_clues(CarmenCase *c, CarmenWorld *w)
{
    for (int i = 0; i < c->trail_len; i++) {
        CarmenCity *city = carmen_world_find(w, c->trail[i]);
        if (!city) continue;

        CarmenTrailStop *stop = &c->stops[i];
        int available = city->site_count;

        if (i < c->trail_len - 1) {
            const char *next_id = c->trail[i + 1];

            int match[CARMEN_MAX_SITES], nm = 0;
            int other[CARMEN_MAX_SITES], no = 0;
            for (int j = 0; j < available; j++) {
                if (site_has_positive_to(&city->sites[j], next_id))
                    match[nm++] = j;
                else
                    other[no++] = j;
            }
            shuffle(match, nm);
            shuffle(other, no);

            int sel[CARMEN_TRAIL_SITES], ns = 0;
            int mi = 0, oi = 0;

            while (ns < 2 && mi < nm) sel[ns++] = match[mi++];
            while (ns < 2 && oi < no) sel[ns++] = other[oi++];

            if (ns < CARMEN_TRAIL_SITES && oi < no)
                sel[ns++] = other[oi++];
            else if (ns < CARMEN_TRAIL_SITES && mi < nm)
                sel[ns++] = match[mi++];

            stop->site_count = ns;

            for (int j = 0; j < ns; j++) {
                stop->sites[j].site_idx = sel[j];
                const CarmenSite *site = &city->sites[sel[j]];

                if (j < 2) {
                    const CarmenClue *found = NULL;
                    for (int k = 0; k < site->clue_count; k++) {
                        if (site->clues[k].type == CARMEN_CLUE_POSITIVE &&
                            strcmp(site->clues[k].target_city_id,
                                   next_id) == 0) {
                            found = &site->clues[k];
                            break;
                        }
                    }
                    if (found) {
                        stop->sites[j].clue = *found;
                    } else {
                        CarmenClue synth;
                        memset(&synth, 0, sizeof(synth));
                        carmen_utf8_copy(synth.text, CARMEN_MAX_CLUE_LEN,
                                         "clue.generic.positive");
                        carmen_utf8_copy(synth.target_city_id,
                                         CARMEN_MAX_NAME_LEN, next_id);
                        synth.type = CARMEN_CLUE_POSITIVE;
                        stop->sites[j].clue = synth;
                    }
                } else {
                    const CarmenClue *neg = NULL;
                    const CarmenClue *herring = NULL;
                    for (int k = 0; k < site->clue_count; k++) {
                        if (site->clues[k].type == CARMEN_CLUE_NEGATIVE
                            && !neg)
                            neg = &site->clues[k];
                        else if (site->clues[k].type == CARMEN_CLUE_POSITIVE
                                 && strcmp(site->clues[k].target_city_id,
                                           next_id) != 0
                                 && !herring)
                            herring = &site->clues[k];
                    }
                    if (neg && herring)
                        stop->sites[j].clue = (carmen_random() % 2)
                                            ? *herring : *neg;
                    else if (neg)
                        stop->sites[j].clue = *neg;
                    else if (herring)
                        stop->sites[j].clue = *herring;
                    else {
                        CarmenClue synth;
                        memset(&synth, 0, sizeof(synth));
                        carmen_utf8_copy(synth.text, CARMEN_MAX_CLUE_LEN,
                                         "clue.generic.negative");
                        synth.type = CARMEN_CLUE_NEGATIVE;
                        stop->sites[j].clue = synth;
                    }
                }
            }
        } else {
            int indices[CARMEN_MAX_SITES];
            for (int j = 0; j < available; j++) indices[j] = j;
            int pick = available < CARMEN_TRAIL_SITES
                     ? available : CARMEN_TRAIL_SITES;
            for (int j = 0; j < pick; j++) {
                int k = j + carmen_random() % (available - j);
                int tmp = indices[j];
                indices[j] = indices[k];
                indices[k] = tmp;
            }
            stop->site_count = pick;
            for (int j = 0; j < pick; j++) {
                stop->sites[j].site_idx = indices[j];
                memset(&stop->sites[j].clue, 0, sizeof(CarmenClue));
            }
        }
    }
}

static int direct_leg_hrs(const CarmenCity *from, const char *to_id)
{
    for (int c = 0; c < from->connection_count; c++)
        if (strcmp(from->connections[c].destination_id, to_id) == 0)
            return carmen_connection_travel_hrs(&from->connections[c]);
    return 0;
}

static int compute_trail_travel_hrs(CarmenWorld *w,
                                    char trail[][CARMEN_MAX_NAME_LEN],
                                    int trail_len)
{
    int total_hrs = 0;
    for (int i = 1; i < trail_len; i++) {
        CarmenCity *prev = carmen_world_find(w, trail[i - 1]);
        if (prev)
            total_hrs += direct_leg_hrs(prev, trail[i]);
    }
    return total_hrs;
}

int carmen_case_generate(CarmenCase *c, CarmenWorld *w,
                         const CarmenCaseSettings *settings)
{
    if (!c || !w || !settings) return 0;

    memset(c, 0, sizeof(*c));

    CarmenDifficulty diff = settings->difficulty;
    int target_len = trail_length_for(diff);
    if (target_len > CARMEN_MAX_TRAIL)
        target_len = CARMEN_MAX_TRAIL;

    int built = 0;
    for (int attempt = 0; attempt < CASE_MAX_RETRIES; attempt++) {
        built = build_trail(w, c->trail, target_len);
        if (built == target_len)
            break;
    }
    if (built < target_len)
        return 0;

    c->trail_len   = target_len;
    c->difficulty   = diff;
    carmen_utf8_copy(c->origin_id, CARMEN_MAX_NAME_LEN, c->trail[0]);
    carmen_utf8_copy(c->hideout_id, CARMEN_MAX_NAME_LEN,
                     c->trail[target_len - 1]);

    c->villain = &FITNA_VILLAINS[carmen_random() % FITNA_VILLAIN_COUNT];

    int art_idx = carmen_random() % CARMEN_ARTIFACT_COUNT;
    c->artifact = CARMEN_ARTIFACTS[art_idx];

    assign_trail_clues(c, w);

    int travel_hrs = compute_trail_travel_hrs(w, c->trail, c->trail_len);
    c->time_budget_hrs = time_budget_hrs_for(diff, travel_hrs);

    return 1;
}
