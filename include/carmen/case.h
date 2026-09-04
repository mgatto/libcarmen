#ifndef CARMEN_CASE_H
#define CARMEN_CASE_H

#include <stdint.h>
#include "carmen_export.h"
#include "artifact.h"
#include "villain.h"
#include "game_world.h"
#include "i18n.h"

/* ABI note: CarmenDifficulty enum values fit in int32 and bindings may read
   a CarmenDifficulty struct field as a 32-bit signed integer.  The enumerators
   are guaranteed to remain EASY=0, MEDIUM=1, HARD=2. */
typedef enum {
    CARMEN_DIFFICULTY_EASY,
    CARMEN_DIFFICULTY_MEDIUM,
    CARMEN_DIFFICULTY_HARD
} CarmenDifficulty;

#define CARMEN_MAX_TRAIL  8
#define CARMEN_TRAIL_SITES 3
#define CARMEN_MAX_VISITED 24

/* Number of villain identity clues seeded into the trail (each in a distinct
   trail city, replacing a herring/negative site). This is the evidence total
   a warrant requires; see CarmenCase.identity_clue_count. */
#define CARMEN_IDENTITY_CLUES 3

/*
 * The single knob for customizing game rules. Populate via
 * carmen_case_settings_default() and, optionally, carmen_case_settings_load().
 *
 * Override fields use 0 to mean "derive/unlimited"; count fields are
 * clamped into their valid ranges at load time.
 */
typedef struct {
    CarmenDifficulty difficulty;
    int32_t trail_length;            /* 0 = derive from difficulty; else clamped to [2, CARMEN_MAX_TRAIL]; fixed-width for ABI */
    int32_t time_budget_hrs;         /* 0 = derive from difficulty + trail travel time; fixed-width for ABI */
    int32_t active_sites_per_city;   /* 0 = derive from difficulty; else clamped to [1, CARMEN_TRAIL_SITES]; fixed-width for ABI */
    int32_t positive_clues_per_stop; /* 0 = derive from difficulty; else clamped to [1, active_sites_per_city]; fixed-width for ABI */
    int32_t move_limit;              /* 0 = unlimited; fixed-width for ABI */
    int32_t visited_history_size;    /* 0 = use full CARMEN_MAX_VISITED; else clamped to [1, CARMEN_MAX_VISITED]; fixed-width for ABI */
} CarmenCaseSettings;

typedef struct {
    int32_t    site_idx;   /* index into city->sites[]; fixed-width for ABI */
    CarmenClue clue;       /* the single assigned clue */
} CarmenTrailSite;

typedef struct {
    CarmenTrailSite sites[CARMEN_TRAIL_SITES];
    int32_t         site_count; /* actual count (<= CARMEN_TRAIL_SITES); fixed-width for ABI */
} CarmenTrailStop;

typedef struct {
    /* Borrowed pointer into the static FITNA_VILLAINS[] catalog; valid for
       the program lifetime.  Bindings should use carmen_case_villain() rather
       than reading this field by offset (pointer width is platform-dependent). */
    const FitnaVillain *villain;
    CarmenArtifact      artifact;
    char   trail[CARMEN_MAX_TRAIL][CARMEN_MAX_NAME_LEN];
    int32_t trail_len;        /* fixed-width for ABI */
    char   origin_id[CARMEN_MAX_NAME_LEN];
    char   hideout_id[CARMEN_MAX_NAME_LEN];
    int32_t time_budget_hrs;  /* fixed-width for ABI */
    CarmenDifficulty    difficulty;
    CarmenTrailStop     stops[CARMEN_MAX_TRAIL];
    /* How many identity clues were actually seeded into the trail. Normally
       CARMEN_IDENTITY_CLUES (3); can be fewer only under degenerate custom
       settings that leave too few replaceable sites. Equals the evidence a
       warrant requires. Fixed-width for ABI. */
    int32_t identity_clue_count;
} CarmenCase;

/*
 * Generate a new case scenario.
 *
 * Picks a random stolen artifact and seeds the trail at that artifact's
 * origin city, so the crime scene the player starts in (trail[0] ==
 * origin_id) matches the case briefing. Builds a trail of directly-
 * connected cities whose length is determined by difficulty
 * (EASY=3, MEDIUM=5, HARD=7), picks a random villain, regenerates a
 * 3-regular flight graph over the world, assigns deterministic clues to
 * 3 sites per trail stop where the number of correct positive clues is
 * derived from difficulty (EASY=2/1/0, MEDIUM=2/1/0, HARD=1/1/1
 * positive/herring/negative; the rest are herrings/negatives, and the
 * hideout stop's sites are negatives), and sets a time budget based on
 * difficulty and total trail travel time.
 *
 * It then seeds CARMEN_IDENTITY_CLUES (3) villain identity clues into the
 * trail, each in a distinct random trail city (the hideout included),
 * overwriting a herring or negative site there (never a correct positive).
 * Investigating such a site yields suspect-description evidence. The number
 * actually placed is stored in identity_clue_count.
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
 * The villain behind this case.  Returns a borrowed pointer into the static
 * FITNA_VILLAINS[] catalog; valid for the program lifetime and never needs to
 * be freed.  Returns NULL if c is NULL.  Prefer this accessor over reading
 * c->villain directly (the embedded pointer's offset is platform-dependent).
 */
CARMEN_API const FitnaVillain *carmen_case_villain(const CarmenCase *c);

/*
 * The artifact stolen in this case.  Returns a borrowed pointer into
 * c->artifact (embedded in the case struct itself); valid for the lifetime of
 * the CarmenCase.  Returns NULL if c is NULL.  Prefer this accessor over
 * reaching into c->artifact directly.
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
