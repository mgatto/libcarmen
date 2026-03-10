#ifndef CARMEN_CASE_H
#define CARMEN_CASE_H

#include "carmen_export.h"
#include "artifact.h"
#include "villain.h"
#include "game_world.h"

typedef enum {
    CARMEN_DIFFICULTY_EASY,
    CARMEN_DIFFICULTY_MEDIUM,
    CARMEN_DIFFICULTY_HARD
} CarmenDifficulty;

typedef struct {
    CarmenDifficulty difficulty;
    int  max_trail_hops;  /* ignored — trail always uses direct connections */
} CarmenCaseSettings;

#define CARMEN_MAX_TRAIL  8
#define CARMEN_TRAIL_SITES 3

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
 * Picks a random villain and artifact, builds a trail of directly-
 * connected cities whose length is determined by difficulty
 * (EASY=3, MEDIUM=5, HARD=7), assigns deterministic clues to 3 sites
 * per trail stop (2 correct + 1 herring/negative), and sets a time
 * budget based on difficulty and total trail travel time.
 *
 * Returns 1 on success, 0 if trail generation failed after retries.
 */
CARMEN_API int carmen_case_generate(CarmenCase *c, CarmenWorld *w,
                                    const CarmenCaseSettings *settings);

#endif
