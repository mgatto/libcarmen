#include <string.h>
#include "carmen/session.h"
#include "carmen/utf8.h"

/* ------------------------------------------------------------ helpers */

static const CarmenConnection *find_connection(const CarmenCity *city,
                                               const char *dest_id)
{
    for (int i = 0; i < city->connection_count; i++)
        if (strcmp(city->connections[i].destination_id, dest_id) == 0)
            return &city->connections[i];
    return NULL;
}

/* Returns the trail index of city_id in the active case, or -1. */
static int trail_index_of(const CarmenCase *c, const char *city_id)
{
    for (int i = 0; i < c->trail_len; i++)
        if (strcmp(c->trail[i], city_id) == 0)
            return i;
    return -1;
}

/* Probability (out of 100) that investigation yields the "good" clue
   pointing to the next trail city. */
static int positive_clue_chance(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return 90;
        case CARMEN_DIFFICULTY_MEDIUM: return 60;
        case CARMEN_DIFFICULTY_HARD:   return 35;
    }
    return 60;
}

/* --------------------------------------------------------- lifecycle */

int carmen_session_start(CarmenSession *s, CarmenWorld *w,
                         const CarmenCaseSettings *settings)
{
    if (!s || !w || !settings) return 0;

    memset(s, 0, sizeof(*s));
    s->world              = w;
    s->warrant_villain_idx = -1;

    if (!carmen_case_generate(&s->active_case, w, settings))
        return 0;

    s->status             = CARMEN_STATUS_PLAYING;
    s->time_remaining_hrs = s->active_case.time_budget_hrs;
    carmen_utf8_copy(s->current_city_id, CARMEN_MAX_NAME_LEN,
                     s->active_case.origin_id);
    return 1;
}

void carmen_session_reset(CarmenSession *s)
{
    if (!s) return;
    CarmenWorld *w = s->world;
    memset(s, 0, sizeof(*s));
    s->world = w;
    s->warrant_villain_idx = -1;
}

/* ----------------------------------------------------------- queries */

CarmenSessionStatus carmen_session_status(const CarmenSession *s)
{
    if (!s) return CARMEN_STATUS_LOST_TIME;
    return s->status;
}

const CarmenCity *carmen_session_current_city(const CarmenSession *s)
{
    if (!s || !s->world) return NULL;
    return carmen_world_find(s->world, s->current_city_id);
}

const CarmenCase *carmen_session_case(const CarmenSession *s)
{
    if (!s) return NULL;
    return &s->active_case;
}

int carmen_session_time_remaining(const CarmenSession *s)
{
    if (!s) return 0;
    return s->time_remaining_hrs;
}

int carmen_session_moves(const CarmenSession *s)
{
    if (!s) return 0;
    return s->moves;
}

/* ----------------------------------------------------------- actions */

int carmen_session_travel(CarmenSession *s, const char *dest_id)
{
    if (!s || !dest_id) return -3;
    if (s->status != CARMEN_STATUS_PLAYING) return -3;

    if (strcmp(s->current_city_id, dest_id) == 0) return -1;

    const CarmenCity *cur = carmen_session_current_city(s);
    if (!cur) return -1;

    const CarmenConnection *conn = find_connection(cur, dest_id);
    if (!conn) return -1;

    int hrs = carmen_connection_travel_hrs(conn);
    s->time_remaining_hrs -= hrs;
    s->moves++;

    carmen_utf8_copy(s->current_city_id, CARMEN_MAX_NAME_LEN, dest_id);

    if (s->time_remaining_hrs <= 0) {
        s->status = CARMEN_STATUS_LOST_TIME;
        return -2;
    }
    return 0;
}

const CarmenClue *carmen_session_investigate(CarmenSession *s, int site_idx)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING) return NULL;

    const CarmenCity *city = carmen_session_current_city(s);
    if (!city || site_idx < 0 || site_idx >= city->site_count) return NULL;

    const CarmenSite *site = &city->sites[site_idx];
    const CarmenCase *cas  = &s->active_case;

    /* At the hideout, collect villain identity evidence (once per site) */
    unsigned int site_bit = 1u << site_idx;
    if (strcmp(s->current_city_id, cas->hideout_id) == 0 &&
        cas->villain && s->evidence_count < FITNA_MAX_ID_CLUES &&
        !(s->hideout_investigated_sites & site_bit)) {
        s->hideout_investigated_sites |= site_bit;
        carmen_utf8_copy(
            s->evidence[s->evidence_count], CARMEN_MAX_CLUE_LEN,
            cas->villain->id_clues[s->evidence_count]);
        s->evidence_count++;
    }

    /* Determine the "correct" next trail city (if we're on the trail) */
    int tidx = trail_index_of(cas, s->current_city_id);
    const char *next_trail_id = NULL;
    if (tidx >= 0 && tidx < cas->trail_len - 1)
        next_trail_id = cas->trail[tidx + 1];

    const CarmenClue *chosen = NULL;

    if (next_trail_id) {
        /* Look for a positive clue pointing to the next trail city */
        const CarmenClue *good = NULL;
        for (int c = 0; c < site->clue_count; c++) {
            if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
                strcmp(site->clues[c].target_city_id, next_trail_id) == 0) {
                good = &site->clues[c];
                break;
            }
        }

        if (good && (carmen_random() % 100) < positive_clue_chance(cas->difficulty)) {
            chosen = good;
        }
    }

    if (!chosen) {
        const CarmenClue *negatives[CARMEN_MAX_CLUES];
        const CarmenClue *herrings[CARMEN_MAX_CLUES];
        int nn = 0, nh = 0;

        for (int c = 0; c < site->clue_count; c++) {
            if (site->clues[c].type == CARMEN_CLUE_NEGATIVE) {
                negatives[nn++] = &site->clues[c];
            } else if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
                       (!next_trail_id ||
                        strcmp(site->clues[c].target_city_id, next_trail_id) != 0)) {
                herrings[nh++] = &site->clues[c];
            }
        }

        if (nn > 0 && nh > 0)
            chosen = (carmen_random() % 2)
                   ? herrings[carmen_random() % nh]
                   : negatives[carmen_random() % nn];
        else if (nn > 0)
            chosen = negatives[carmen_random() % nn];
        else if (nh > 0)
            chosen = herrings[carmen_random() % nh];
        else if (site->clue_count > 0)
            chosen = &site->clues[carmen_random() % site->clue_count];
    }

    if (!chosen) return NULL;

    if (s->notebook_count >= CARMEN_MAX_NOTEBOOK) return NULL;

    s->notebook[s->notebook_count] = *chosen;
    s->notebook_count++;
    return &s->notebook[s->notebook_count - 1];
}

int carmen_session_issue_warrant(CarmenSession *s, int villain_idx)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING) return -1;
    if (villain_idx < 0 || villain_idx >= FITNA_VILLAIN_COUNT) return -1;
    s->warrant_villain_idx = villain_idx;
    return 0;
}

CarmenSessionStatus carmen_session_arrest(CarmenSession *s)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING)
        return s ? s->status : CARMEN_STATUS_LOST_TIME;

    if (strcmp(s->current_city_id, s->active_case.hideout_id) != 0)
        return CARMEN_STATUS_NOT_AT_HIDEOUT;

    if (s->warrant_villain_idx < 0) {
        s->status = CARMEN_STATUS_LOST_NO_WARRANT;
        return s->status;
    }

    const FitnaVillain *wanted =
        &FITNA_VILLAINS[s->warrant_villain_idx];
    if (strcmp(wanted->id, s->active_case.villain->id) == 0)
        s->status = CARMEN_STATUS_WON;
    else
        s->status = CARMEN_STATUS_LOST_WRONG_ARREST;

    return s->status;
}
