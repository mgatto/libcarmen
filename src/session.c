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

static int trail_index_of(const CarmenCase *c, const char *city_id)
{
    for (int i = 0; i < c->trail_len; i++)
        if (strcmp(c->trail[i], city_id) == 0)
            return i;
    return -1;
}

/*
 * How many identity clues a warrant requires: exactly the number the case
 * seeded into the trail (normally CARMEN_IDENTITY_CLUES). Each is found by
 * investigating one identity-clue site, so this is always collectable.
 */
static int warrant_evidence_target(const CarmenCase *c)
{
    return c->identity_clue_count;
}

/*
 * Record a villain id-clue key into the evidence set, deduplicated by key so
 * re-investigating the same identity site does not count twice. No-op once the
 * evidence array is full.
 */
static void collect_evidence(CarmenSession *s, const char *id_clue_key)
{
    for (int i = 0; i < s->evidence_count; i++)
        if (strcmp(s->evidence[i], id_clue_key) == 0)
            return;
    if (s->evidence_count >= FITNA_MAX_ID_CLUES) return;
    carmen_utf8_copy(s->evidence[s->evidence_count++], CARMEN_MAX_CLUE_LEN,
                     id_clue_key);
}

static void record_visit(CarmenSession *s, const char *city_id)
{
    int cap = s->settings.visited_history_size;
    if (cap <= 0 || cap > CARMEN_MAX_VISITED) cap = CARMEN_MAX_VISITED;
    if (s->visited_count >= cap) return;
    carmen_utf8_copy(s->visited[s->visited_count++], CARMEN_MAX_NAME_LEN,
                     city_id);
}

/* --------------------------------------------------------- lifecycle */

int carmen_session_start(CarmenSession *s, CarmenWorld *w,
                         const CarmenCaseSettings *settings)
{
    if (!s || !w || !settings) return 0;

    memset(s, 0, sizeof(*s));
    s->world              = w;
    s->settings           = *settings;
    s->warrant_villain_idx = -1;

    if (!carmen_case_generate(&s->active_case, w, settings))
        return 0;

    s->status             = CARMEN_STATUS_PLAYING;
    s->time_remaining_hrs = s->active_case.time_budget_hrs;
    carmen_utf8_copy(s->current_city_id, CARMEN_MAX_NAME_LEN,
                     s->active_case.origin_id);
    record_visit(s, s->active_case.origin_id);
    return 1;
}

int carmen_session_reset(CarmenSession *s)
{
    if (!s) return 0;

    /* Copy the world pointer and settings out first: carmen_session_start()
       memsets *s before reading its settings argument, so passing &s->settings
       directly would hand it a buffer it is about to zero. */
    CarmenWorld *w = s->world;
    CarmenCaseSettings settings = s->settings;
    return carmen_session_start(s, w, &settings);
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

static int score_base_for(CarmenDifficulty d)
{
    switch (d) {
        case CARMEN_DIFFICULTY_EASY:   return CARMEN_SCORE_BASE_EASY;
        case CARMEN_DIFFICULTY_MEDIUM: return CARMEN_SCORE_BASE_MEDIUM;
        case CARMEN_DIFFICULTY_HARD:   return CARMEN_SCORE_BASE_HARD;
    }
    return CARMEN_SCORE_BASE_MEDIUM;
}

int carmen_session_score(const CarmenSession *s)
{
    if (!s || s->status != CARMEN_STATUS_WON) return 0;

    /* Inputs are bounded by game logic (time budget derives from difficulty
       plus trail travel; a win only follows a bounded number of moves), so
       the additive arithmetic stays well within int range. */
    int base  = score_base_for(s->active_case.difficulty);
    int score = base + s->time_remaining_hrs * CARMEN_SCORE_TIME_WEIGHT
                     - s->moves * CARMEN_SCORE_MOVE_PENALTY;
    if (score < base) score = base;
    return score;
}

const FitnaVillain *carmen_session_villain(const CarmenSession *s)
{
    if (!s) return NULL;
    return s->active_case.villain;
}

int carmen_session_active_sites(const CarmenSession *s,
                                int *out_indices, int max_out)
{
    if (!s || !out_indices || max_out <= 0) return 0;

    const CarmenCase *cas = &s->active_case;
    int tidx = trail_index_of(cas, s->current_city_id);
    if (tidx < 0) return 0;

    const CarmenTrailStop *stop = &cas->stops[tidx];
    int count = stop->site_count < max_out ? stop->site_count : max_out;
    for (int i = 0; i < count; i++)
        out_indices[i] = stop->sites[i].site_idx;
    return count;
}

int carmen_session_visited_count(const CarmenSession *s)
{
    if (!s) return 0;
    return s->visited_count;
}

const char *carmen_session_visited_at(const CarmenSession *s, int index)
{
    if (!s || index < 0 || index >= s->visited_count) return NULL;
    return s->visited[index];
}

int carmen_session_notebook_count(const CarmenSession *s)
{
    if (!s) return 0;
    return s->notebook_count;
}

const CarmenClue *carmen_session_notebook_at(const CarmenSession *s, int index)
{
    if (!s || index < 0 || index >= s->notebook_count) return NULL;
    return &s->notebook[index];
}

int carmen_session_evidence_count(const CarmenSession *s)
{
    if (!s) return 0;
    return s->evidence_count;
}

const char *carmen_session_evidence_at(const CarmenSession *s, int index)
{
    if (!s || index < 0 || index >= s->evidence_count) return NULL;
    return s->evidence[index];
}

int carmen_session_evidence_required(const CarmenSession *s)
{
    if (!s) return 0;
    return warrant_evidence_target(&s->active_case);
}

bool carmen_session_can_issue_warrant(const CarmenSession *s)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING) return false;
    return s->evidence_count >= warrant_evidence_target(&s->active_case);
}

int carmen_session_connections(const CarmenSession *s,
                               const CarmenConnection **out, int max_out)
{
    if (!s || !out || max_out <= 0) return 0;

    const CarmenCity *city = carmen_session_current_city(s);
    if (!city) return 0;

    int count = city->connection_count < max_out ? city->connection_count
                                                  : max_out;
    for (int i = 0; i < count; i++)
        out[i] = &city->connections[i];
    return count;
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
    record_visit(s, dest_id);

    if (s->time_remaining_hrs <= 0) {
        s->status = CARMEN_STATUS_LOST_TIME;
        return -2;
    }
    if (s->settings.move_limit > 0 && s->moves >= s->settings.move_limit) {
        s->status = CARMEN_STATUS_LOST_MOVES;
        return -4;
    }
    return 0;
}

const CarmenClue *carmen_session_investigate(CarmenSession *s, int site_idx)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING) return NULL;

    const CarmenCity *city = carmen_session_current_city(s);
    if (!city) return NULL;

    const CarmenCase *cas = &s->active_case;
    int tidx = trail_index_of(cas, s->current_city_id);

    /* Off-trail: always return a negative clue */
    if (tidx < 0) {
        if (site_idx < 0 || site_idx >= city->site_count) return NULL;
        if (s->notebook_count >= CARMEN_MAX_NOTEBOOK) return NULL;
        CarmenClue neg;
        memset(&neg, 0, sizeof(neg));
        carmen_utf8_copy(neg.text, CARMEN_MAX_CLUE_LEN,
                         "clue.generic.negative");
        neg.type = CARMEN_CLUE_NEGATIVE;
        s->notebook[s->notebook_count] = neg;
        return &s->notebook[s->notebook_count++];
    }

    /* On-trail: look up the pre-assigned clue for this site */
    const CarmenTrailStop *stop = &cas->stops[tidx];

    int active_idx = -1;
    for (int i = 0; i < stop->site_count; i++) {
        if (stop->sites[i].site_idx == site_idx) {
            active_idx = i;
            break;
        }
    }
    if (active_idx < 0) return NULL;

    /* On-trail stop (hideout included): return the deterministic pre-assigned
       clue. An identity clue also records suspect evidence, deduplicated so
       revisiting the same site never double-counts. */
    if (s->notebook_count >= CARMEN_MAX_NOTEBOOK) return NULL;
    const CarmenClue *clue = &stop->sites[active_idx].clue;
    if (clue->type == CARMEN_CLUE_IDENTITY)
        collect_evidence(s, clue->text);
    s->notebook[s->notebook_count] = *clue;
    return &s->notebook[s->notebook_count++];
}

int carmen_session_issue_warrant(CarmenSession *s, int villain_idx)
{
    if (!s || s->status != CARMEN_STATUS_PLAYING) return -1;
    if (villain_idx < 0 || villain_idx >= FITNA_VILLAIN_COUNT) return -1;
    if (s->evidence_count < warrant_evidence_target(&s->active_case)) return -2;
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
