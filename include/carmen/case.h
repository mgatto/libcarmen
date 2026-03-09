#ifndef CARMEN_CASE_H
#define CARMEN_CASE_H

#include "carmen/carmen_export.h"
#include "carmen/artifact.h"
#include "carmen/villain.h"
#include "carmen/game_world.h"

typedef enum {
    CARMEN_DIFFICULTY_EASY,
    CARMEN_DIFFICULTY_MEDIUM,
    CARMEN_DIFFICULTY_HARD
} CarmenDifficulty;

typedef struct {
    CarmenDifficulty difficulty;
    int  max_trail_hops;  /* 0 = use difficulty default (EASY=1, MEDIUM=2, HARD=3) */
} CarmenCaseSettings;

#define CARMEN_MAX_TRAIL 8

typedef struct {
    const FitnaVillain *villain;
    CarmenArtifact      artifact;
    char   trail[CARMEN_MAX_TRAIL][CARMEN_MAX_NAME_LEN];
    int    trail_len;
    char   origin_id[CARMEN_MAX_NAME_LEN];
    char   hideout_id[CARMEN_MAX_NAME_LEN];
    int    time_budget_hrs;
    int    max_trail_hops;
    CarmenDifficulty difficulty;
} CarmenCase;

/*
 * Generate a new case scenario.
 *
 * Picks a random villain and artifact, builds a trail of cities whose
 * length is determined by difficulty (EASY=3, MEDIUM=5, HARD=7), and
 * sets a time budget based on difficulty and total trail travel time.
 *
 * The trail builder searches for next-stop candidates within
 * max_trail_hops of the current city (default per difficulty if 0),
 * preferring cities that the current city's sites have positive clues
 * pointing toward.
 *
 * Returns 1 on success, 0 if trail generation failed after retries.
 */
CARMEN_API int carmen_case_generate(CarmenCase *c, CarmenWorld *w,
                                    const CarmenCaseSettings *settings);

#endif
