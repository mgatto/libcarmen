#include <stdbool.h>
#include <string.h>
#include "carmen/case.h"
#include "carmen/utf8.h"
#include "carmen/i18n.h"

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

/*
 * Correct positive clues per non-hideout stop, derived from difficulty.
 * The remaining active sites carry herrings/negatives: EASY 3/0/0,
 * MEDIUM 2/1/0, HARD 1/1/1 (with 3 active sites).
 */
static int positive_clues_for(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return 3;
        case CARMEN_DIFFICULTY_MEDIUM: return 2;
        case CARMEN_DIFFICULTY_HARD:   return 1;
    }
    return 2;
}

static int negative_clues_for(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return 0;
        case CARMEN_DIFFICULTY_MEDIUM: return 0;
        case CARMEN_DIFFICULTY_HARD:   return 1;
    }
    return 0;
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

static void shuffle(int *arr, int n)
{
    for (int j = n - 1; j > 0; j--) {
        int k = carmen_random() % (j + 1);
        int tmp = arr[j]; arr[j] = arr[k]; arr[k] = tmp;
    }
}

/*
 * Randomized DFS: extend trail[] from depth until target_len, never
 * revisiting a city. Neighbors are shuffled so each search is a random
 * simple path, not a fixed DFS order. Returns target_len on success.
 */
static int dfs_trail(CarmenWorld *w,
                     char trail[][CARMEN_MAX_NAME_LEN],
                     bool on_trail[CARMEN_MAX_CITIES],
                     int depth, int target_len)
{
    if (depth == target_len)
        return target_len;

    CarmenCity *cur = carmen_world_find(w, trail[depth - 1]);
    if (!cur)
        return depth;

    int cand[CARMEN_MAX_CONNECTIONS];
    int nc = 0;
    for (int k = 0; k < cur->connection_count; k++) {
        CarmenCity *nb = carmen_world_find(w, cur->connections[k].destination_id);
        if (!nb)
            continue;
        int slot = city_slot(w, nb);
        if (slot < 0 || slot >= w->city_count)
            continue;
        if (!on_trail[slot])
            cand[nc++] = slot;
    }
    shuffle(cand, nc);

    for (int i = 0; i < nc; i++) {
        int slot = cand[i];
        carmen_utf8_copy(trail[depth], CARMEN_MAX_NAME_LEN, w->storage[slot].id);
        on_trail[slot] = true;
        if (dfs_trail(w, trail, on_trail, depth + 1, target_len) == target_len)
            return target_len;
        on_trail[slot] = false;
    }
    return depth;
}

/*
 * Build a simple path of directly-connected cities starting at the given
 * city slot. Returns the number of cities placed (== target_len on full
 * success, less if no simple path of that length exists).
 */
static int build_trail_from(CarmenWorld *w,
                            char trail[][CARMEN_MAX_NAME_LEN],
                            int target_len,
                            int start)
{
    if (start < 0 || start >= w->city_count)
        return 0;

    CarmenCity *cur = &w->storage[start];
    carmen_utf8_copy(trail[0], CARMEN_MAX_NAME_LEN, cur->id);

    bool on_trail[CARMEN_MAX_CITIES] = {false};
    on_trail[start] = true;

    return dfs_trail(w, trail, on_trail, 1, target_len);
}

/*
 * Build a positive clue whose text is drawn from the destination city's
 * inbound pool (targetless in seed data) and whose target_city_id is
 * assigned here, at runtime.  pool_pick indexes into the destination's
 * pool (callers vary it to spread distinct descriptors across sites).
 */
static void positive_clue_from(CarmenClue *out, const CarmenCity *dest,
                               const char *target_id, int pool_pick)
{
    memset(out, 0, sizeof(*out));
    const char *text = NULL;
    if (dest && dest->inbound_clue_count > 0)
        text = dest->inbound_clues[pool_pick % dest->inbound_clue_count];
    carmen_utf8_copy(out->text, CARMEN_MAX_CLUE_LEN,
                     text ? text : "clue.generic.positive");
    carmen_utf8_copy(out->target_city_id, CARMEN_MAX_NAME_LEN, target_id);
    out->type = CARMEN_CLUE_POSITIVE;
}

static void negative_clue(CarmenClue *out)
{
    memset(out, 0, sizeof(*out));
    carmen_utf8_copy(out->text, CARMEN_MAX_CLUE_LEN, "clue.generic.negative");
    out->type = CARMEN_CLUE_NEGATIVE;
}

/*
 * For each trail stop, select up to active_sites sites and assign one
 * clue per site from the current city's outgoing edges:
 *   - positive_clues sites get a positive clue pointing to the next trail
 *     city, drawn from that city's inbound pool.
 *   - herring sites get a positive-looking clue pointing to a distinct
 *     wrong-but-connected neighbor (red herring).
 *   - remaining sites get a generic negative ("never saw anyone").
 * Counts are difficulty-driven (EASY 3/0/0, MEDIUM 2/1/0, HARD 1/1/1)
 * with positive_clues honoring the settings override. The hideout stop
 * stores site indices only (evidence, not clues).
 */
static void assign_trail_clues(CarmenCase *c, CarmenWorld *w,
                               int active_sites, int positive_clues)
{
    const int negative_want = negative_clues_for(c->difficulty);

    for (int i = 0; i < c->trail_len; i++) {
        CarmenCity *city = carmen_world_find(w, c->trail[i]);
        if (!city) continue;

        CarmenTrailStop *stop = &c->stops[i];
        int available = city->site_count;

        /* Active sites are a shuffled subset of the city's sites. */
        int sites[CARMEN_MAX_SITES];
        for (int j = 0; j < available; j++) sites[j] = j;
        shuffle(sites, available);
        int ns = available < active_sites ? available : active_sites;

        if (i == c->trail_len - 1) {
            /* Hideout: site indices only; evidence is gathered in-session. */
            stop->site_count = ns;
            for (int j = 0; j < ns; j++) {
                stop->sites[j].site_idx = sites[j];
                memset(&stop->sites[j].clue, 0, sizeof(CarmenClue));
            }
            continue;
        }

        const char *next_id = c->trail[i + 1];
        CarmenCity *next_city = carmen_world_find(w, next_id);

        /* Shuffled order over the next city's pool so multiple positive
           sites tend to surface different descriptors of the same city. */
        int pool_order[CARMEN_MAX_INBOUND_CLUES];
        int pool_n = next_city ? next_city->inbound_clue_count : 0;
        for (int j = 0; j < pool_n; j++) pool_order[j] = j;
        shuffle(pool_order, pool_n);

        /* Wrong-but-connected neighbors are the decoy destinations. */
        int wrong[CARMEN_MAX_CONNECTIONS], nw = 0;
        for (int k = 0; k < city->connection_count; k++)
            if (strcmp(city->connections[k].destination_id, next_id) != 0)
                wrong[nw++] = k;
        shuffle(wrong, nw);

        int npos = positive_clues;
        if (npos > ns) npos = ns;
        int nneg = negative_want;
        if (npos + nneg > ns) nneg = ns - npos;
        int nherr = ns - npos - nneg;
        if (nherr > nw) {
            nneg += nherr - nw;
            nherr = nw;
        }

        stop->site_count = ns;
        for (int j = 0; j < ns; j++) {
            stop->sites[j].site_idx = sites[j];

            if (j < npos) {
                int pick = pool_n > 0 ? pool_order[j % pool_n] : 0;
                positive_clue_from(&stop->sites[j].clue, next_city,
                                   next_id, pick);
            } else if (j < npos + nherr) {
                int d = j - npos;
                const char *wrong_id =
                    city->connections[wrong[d]].destination_id;
                CarmenCity *wc = carmen_world_find(w, wrong_id);
                positive_clue_from(&stop->sites[j].clue, wc, wrong_id,
                                   carmen_random());
            } else {
                negative_clue(&stop->sites[j].clue);
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

    carmen_world_generate_connections(w);

    CarmenDifficulty diff = settings->difficulty;
    int target_len = settings->trail_length > 0
                   ? settings->trail_length
                   : trail_length_for(diff);
    if (target_len > CARMEN_MAX_TRAIL)
        target_len = CARMEN_MAX_TRAIL;
    if (target_len < 2)
        target_len = 2;

    /*
     * World-aware artifact catalog: only artifacts whose origin city
     * actually exists in this world can drive an artifact-first trail.
     * Collect them up front so the intent is explicit -- an empty set is
     * precisely the condition that forces the random fallback below.
     */
    int usable[CARMEN_ARTIFACT_COUNT];
    int usable_count = 0;
    for (int i = 0; i < CARMEN_ARTIFACT_COUNT; i++) {
        if (carmen_world_find(w, CARMEN_ARTIFACTS[i].origin_city_id))
            usable[usable_count++] = i;
    }

    /*
     * Artifact-driven trail: pick a usable stolen artifact and seed the
     * trail at its origin city, so the crime scene the player starts in
     * matches the case briefing.  Retry with fresh artifacts until one of
     * their origin cities can seed a full-length trail.
     */
    int built = 0;
    int have_artifact = 0;
    for (int attempt = 0; usable_count > 0 && attempt < CASE_MAX_RETRIES;
         attempt++) {
        if (attempt > 0)
            carmen_world_generate_connections(w);
        const CarmenArtifact *art =
            &CARMEN_ARTIFACTS[usable[carmen_random() % usable_count]];
        CarmenCity *origin = carmen_world_find(w, art->origin_city_id);
        built = build_trail_from(w, c->trail, target_len,
                                 city_slot(w, origin));
        if (built == target_len) {
            c->artifact = *art;
            have_artifact = 1;
            break;
        }
    }

    /*
     * Fallback for worlds that contain none of the artifact origin cities
     * (usable_count == 0, e.g. synthetic test worlds): start from a random
     * city and pick a random artifact.  Narrative may not match here, but
     * generation still succeeds.
     */
    if (!have_artifact) {
        for (int attempt = 0; attempt < CASE_MAX_RETRIES; attempt++) {
            if (attempt > 0)
                carmen_world_generate_connections(w);
            built = build_trail_from(w, c->trail, target_len,
                                     carmen_random() % w->city_count);
            if (built == target_len)
                break;
        }
        if (built < target_len)
            return 0;
        c->artifact = CARMEN_ARTIFACTS[carmen_random() % CARMEN_ARTIFACT_COUNT];
    }

    c->trail_len   = target_len;
    c->difficulty   = diff;
    carmen_utf8_copy(c->origin_id, CARMEN_MAX_NAME_LEN, c->trail[0]);
    carmen_utf8_copy(c->hideout_id, CARMEN_MAX_NAME_LEN,
                     c->trail[target_len - 1]);

    c->villain = &FITNA_VILLAINS[carmen_random() % FITNA_VILLAIN_COUNT];

    int active_sites = settings->active_sites_per_city > 0
                     ? settings->active_sites_per_city : CARMEN_TRAIL_SITES;
    if (active_sites > CARMEN_TRAIL_SITES)
        active_sites = CARMEN_TRAIL_SITES;
    int positive_clues = settings->positive_clues_per_stop > 0
                       ? settings->positive_clues_per_stop
                       : positive_clues_for(diff);
    if (positive_clues > active_sites)
        positive_clues = active_sites;

    assign_trail_clues(c, w, active_sites, positive_clues);

    if (settings->time_budget_hrs > 0) {
        c->time_budget_hrs = settings->time_budget_hrs;
    } else {
        int travel_hrs = compute_trail_travel_hrs(w, c->trail, c->trail_len);
        c->time_budget_hrs = time_budget_hrs_for(diff, travel_hrs);
    }

    return 1;
}

const FitnaVillain *carmen_case_villain(const CarmenCase *c)
{
    if (!c) return NULL;
    return c->villain;
}

const CarmenArtifact *carmen_case_artifact(const CarmenCase *c)
{
    if (!c) return NULL;
    return &c->artifact;
}

/*
 * Append src to buf using snprintf-style bounds: only write while a byte
 * (plus room for the terminating NUL) fits, but always advance total so the
 * caller learns the full length that would have been produced.
 */
static int briefing_append(char *buf, int buf_size, int total, const char *src)
{
    for (; *src; src++, total++)
        if (buf && total < buf_size - 1)
            buf[total] = *src;
    return total;
}

#define CARMEN_BRIEFING_DEFAULT "Someone stole the {artifact} from {city}."

int carmen_case_briefing_text(const CarmenCase *c, CarmenWorld *w,
                              const CarmenI18n *i18n, char *buf, int buf_size)
{
    if (!c) return 0;

    const char *artifact = carmen_i18n_get(i18n, c->artifact.name);

    const char *city_key = c->origin_id;
    if (w) {
        const CarmenCity *origin = carmen_world_find(w, c->origin_id);
        if (origin)
            city_key = origin->name;
    }
    const char *city = carmen_i18n_get(i18n, city_key);

    /*
     * carmen_i18n_get() echoes the key back when it is missing, so fall
     * back to the built-in template unless a real one (with tokens) loaded.
     */
    const char *tmpl = carmen_i18n_get(i18n, "ui.briefing");
    if (!tmpl || !strstr(tmpl, "{artifact}"))
        tmpl = CARMEN_BRIEFING_DEFAULT;

    int total = 0;
    for (const char *r = tmpl; *r; ) {
        if (strncmp(r, "{artifact}", 10) == 0) {
            total = briefing_append(buf, buf_size, total, artifact);
            r += 10;
        } else if (strncmp(r, "{city}", 6) == 0) {
            total = briefing_append(buf, buf_size, total, city);
            r += 6;
        } else {
            if (buf && total < buf_size - 1)
                buf[total] = *r;
            total++;
            r++;
        }
    }
    if (buf && buf_size > 0)
        buf[total < buf_size ? total : buf_size - 1] = '\0';
    return total;
}
