#ifndef CARMEN_CASE_H
#define CARMEN_CASE_H

#include "carmen_export.h"
#include "artifact.h"
#include "villain.h"
#include "game_world.h"
#include "i18n.h"

typedef enum {
    CARMEN_DIFFICULTY_EASY,
    CARMEN_DIFFICULTY_MEDIUM,
    CARMEN_DIFFICULTY_HARD
} CarmenDifficulty;

#define CARMEN_MAX_TRAIL  8
#define CARMEN_TRAIL_SITES 3
#define CARMEN_MAX_VISITED 24

/*
 * The single knob for customizing game rules. Populate via
 * carmen_case_settings_default() and, optionally, carmen_case_settings_load().
 *
 * Override fields use 0 to mean "derive/unlimited"; count fields are
 * clamped into their valid ranges at load time.
 */
typedef struct {
    CarmenDifficulty difficulty;
    int  trail_length;            /* 0 = derive from difficulty; else clamped to [2, CARMEN_MAX_TRAIL] */
    int  time_budget_hrs;         /* 0 = derive from difficulty + trail travel time */
    int  active_sites_per_city;   /* clamped to [1, CARMEN_TRAIL_SITES] */
    int  positive_clues_per_stop; /* clamped to [1, active_sites_per_city] */
    int  move_limit;              /* 0 = unlimited */
    int  visited_history_size;    /* 0 = use full CARMEN_MAX_VISITED; else clamped to [1, CARMEN_MAX_VISITED] */
} CarmenCaseSettings;

typedef struct {
    int        site_idx;   /* index into city->sites[] */
    CarmenClue clue;       /* the single assigned clue  */
} CarmenTrailSite;

typedef struct {
    CarmenTrailSite sites[CARMEN_TRAIL_SITES];
    int             site_count; /* actual count (<= CARMEN_TRAIL_SITES) */
} CarmenTrailStop;

typedef struct {
    const FitnaVillain *villain;
    CarmenArtifact      artifact;
    char   trail[CARMEN_MAX_TRAIL][CARMEN_MAX_NAME_LEN];
    int    trail_len;
    char   origin_id[CARMEN_MAX_NAME_LEN];
    char   hideout_id[CARMEN_MAX_NAME_LEN];
    int    time_budget_hrs;
    CarmenDifficulty    difficulty;
    CarmenTrailStop     stops[CARMEN_MAX_TRAIL];
} CarmenCase;

/*
 * Generate a new case scenario.
 *
 * Picks a random stolen artifact and seeds the trail at that artifact's
 * origin city, so the crime scene the player starts in (trail[0] ==
 * origin_id) matches the case briefing. Builds a trail of directly-
 * connected cities whose length is determined by difficulty
 * (EASY=3, MEDIUM=5, HARD=7), picks a random villain, assigns
 * deterministic clues to 3 sites per trail stop (2 correct + 1
 * herring/negative), and sets a time budget based on difficulty and
 * total trail travel time.
 *
 * In worlds that contain none of the artifact origin cities, generation
 * falls back to a random start city and a random artifact (narrative may
 * not match).
 *
 * Returns 1 on success, 0 if trail generation failed after retries.
 */
CARMEN_API int carmen_case_generate(CarmenCase *c, CarmenWorld *w,
                                    const CarmenCaseSettings *settings);

/*
 * The villain behind this case. Returns NULL if c is NULL. Lets clients
 * read the culprit without dereferencing c->villain directly.
 */
CARMEN_API const FitnaVillain *carmen_case_villain(const CarmenCase *c);

/*
 * The artifact stolen in this case. Returns NULL if c is NULL. Lets
 * clients read the loot without reaching into c->artifact directly.
 */
CARMEN_API const CarmenArtifact *carmen_case_artifact(const CarmenCase *c);

/*
 * Compose a human-readable, localized case briefing such as
 * "Someone stole the Astrolabe of Isfahan from Isfahan." into buf, so
 * clients (UI, bindings, the demo) don't each reinvent the sentence.
 *
 * The artifact name and origin-city name are resolved through i18n; the
 * sentence structure comes from the "ui.briefing" locale key when present,
 * otherwise from a built-in English template. The template may contain the
 * tokens {artifact} and {city}, in any order, so translations can reorder
 * subject and object.
 *
 * If i18n is NULL, raw i18n keys are emitted (no translation). If w is
 * non-NULL it is used to resolve the origin city's display name; otherwise
 * the origin city id is used.
 *
 * Follows the snprintf/carmen_*_to_string convention: writes at most
 * buf_size bytes (always NUL-terminating when buf and buf_size > 0) and
 * returns the number of bytes that would have been written excluding the
 * NUL, or 0 if c is NULL.
 */
CARMEN_API int carmen_case_briefing_text(const CarmenCase *c, CarmenWorld *w,
                                         const CarmenI18n *i18n,
                                         char *buf, int buf_size);

#endif
