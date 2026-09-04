#ifndef CARMEN_SESSION_H
#define CARMEN_SESSION_H

#include <stdbool.h>
#include <stdint.h>
#include "carmen_export.h"
#include "case.h"

#define CARMEN_MAX_NOTEBOOK 64

/*
 * Scoring constants for carmen_session_score() (additive model).
 *
 * base(difficulty) is the per-difficulty floor a win is always worth;
 * harder cases score higher for the same play. Leftover time is rewarded
 * per hour and each move costs a small penalty. See carmen_session_score()
 * for the exact formula.
 */
#define CARMEN_SCORE_BASE_EASY    1000
#define CARMEN_SCORE_BASE_MEDIUM  2000
#define CARMEN_SCORE_BASE_HARD    3000
#define CARMEN_SCORE_TIME_WEIGHT    10
#define CARMEN_SCORE_MOVE_PENALTY   50

/* ABI note: CarmenSessionStatus enum values fit in int32 and bindings may
   read a CarmenSessionStatus struct field as a 32-bit signed integer.  The
   enumerators are guaranteed to remain: PLAYING=0, WON=1, LOST_TIME=2,
   LOST_MOVES=3, LOST_WRONG_ARREST=4, LOST_NO_WARRANT=5,
   NOT_AT_HIDEOUT=6. */
typedef enum {
  CARMEN_STATUS_PLAYING,
  CARMEN_STATUS_WON,
  CARMEN_STATUS_LOST_TIME,
  CARMEN_STATUS_LOST_MOVES,
  CARMEN_STATUS_LOST_WRONG_ARREST,
  CARMEN_STATUS_LOST_NO_WARRANT,
  CARMEN_STATUS_NOT_AT_HIDEOUT
} CarmenSessionStatus;

typedef struct {
  /* Caller-supplied pointer; borrowed, not owned by the session.  Bindings
     should not read this field by offset (pointer width is platform-dependent);
     re-attach via carmen_session_start() / carmen_session_load(). */
  CarmenWorld *world;
  CarmenCase   active_case;
  CarmenCaseSettings settings;
  CarmenSessionStatus status;
  char    current_city_id[CARMEN_MAX_NAME_LEN];
  char    visited[CARMEN_MAX_VISITED][CARMEN_MAX_NAME_LEN];
  int32_t visited_count;         /* fixed-width for cross-platform ABI */
  int32_t time_remaining_hrs;    /* fixed-width for cross-platform ABI */
  int32_t moves;                 /* fixed-width for cross-platform ABI */
  CarmenClue notebook[CARMEN_MAX_NOTEBOOK];
  int32_t    notebook_count;     /* fixed-width for cross-platform ABI */
  char    evidence[FITNA_MAX_ID_CLUES][CARMEN_MAX_CLUE_LEN];
  int32_t evidence_count;        /* fixed-width for cross-platform ABI */
  int32_t warrant_villain_idx;   /* -1 = no warrant issued; fixed-width for ABI */
} CarmenSession;

/* Lifecycle */
CARMEN_API int carmen_session_start(CarmenSession *s, CarmenWorld *w,
                                    const CarmenCaseSettings *settings);

/*
 * Reset the session and regenerate a fresh case in place, reusing the world
 * and settings the session was last started with. Equivalent to calling
 * carmen_session_start() again with the same world/settings: it clears all
 * play state (notebook, evidence, visited history, warrant, moves, status)
 * and leaves the session immediately PLAYING at the new case's origin city.
 *
 * Returns 1 on success, 0 on failure (s is NULL, no world is attached, or
 * case generation failed). On failure the session is left non-playable, so
 * callers should treat a 0 the same way they treat a failed
 * carmen_session_start().
 */
CARMEN_API int carmen_session_reset(CarmenSession *s);

/* Queries (read-only, for UI) */
CARMEN_API CarmenSessionStatus carmen_session_status(const CarmenSession *s);

/* Returns the current city, borrowed from the session's world.  The pointer
   is valid for the lifetime of the world; it does not change ownership when
   the player travels.  Returns NULL if s is NULL or the world is unset. */
CARMEN_API const CarmenCity *
carmen_session_current_city(const CarmenSession *s);

/* Returns the active case embedded in the session.  The pointer is borrowed:
   it points into s and remains valid for the lifetime of the session.
   Returns NULL if s is NULL. */
CARMEN_API const CarmenCase *carmen_session_case(const CarmenSession *s);

CARMEN_API int carmen_session_time_remaining(const CarmenSession *s);
CARMEN_API int carmen_session_moves(const CarmenSession *s);

/*
 * Final score for a won session (a simple additive model). Returns 0 for
 * any non-WON status (including a NULL session), so a front-end can call
 * it unconditionally and only surface a score once
 * carmen_session_status() == CARMEN_STATUS_WON.
 *
 * On a win the score is:
 *
 *     base(difficulty)
 *       + time_remaining_hrs * CARMEN_SCORE_TIME_WEIGHT
 *       - moves              * CARMEN_SCORE_MOVE_PENALTY
 *
 * clamped so it never falls below base(difficulty), where base is
 * CARMEN_SCORE_BASE_EASY / _MEDIUM / _HARD. Leftover time rewards speed
 * and extra moves cost a small penalty; the higher base for harder cases
 * means a hard win outscores an easy one played equally well. The result
 * is deterministic given the frozen end-state (no game actions remain
 * once the session is WON).
 */
CARMEN_API int carmen_session_score(const CarmenSession *s);

/*
 * The villain behind the active case.  Returns a borrowed pointer into the
 * static FITNA_VILLAINS[] catalog; valid for the program lifetime and never
 * needs to be freed.  Returns NULL if s is NULL.  Convenience wrapper so
 * clients don't dereference the embedded case.
 */
CARMEN_API const FitnaVillain *carmen_session_villain(const CarmenSession *s);

/*
 * Cities-visited history (chronological, includes the origin and any
 * revisits). Capped at the session's visited_history_size setting.
 * carmen_session_visited_at returns NULL for an out-of-range index or NULL s.
 * The returned pointer is borrowed: it points into s->visited[] and remains
 * valid for the lifetime of the session.  Callers must not free the pointer.
 */
CARMEN_API int         carmen_session_visited_count(const CarmenSession *s);
CARMEN_API const char *carmen_session_visited_at(const CarmenSession *s,
                                                 int index);

/*
 * Clue notebook (clues dispensed by carmen_session_investigate, oldest
 * first).  carmen_session_notebook_at returns NULL for an out-of-range
 * index or NULL s.  The returned pointer is borrowed: it points into
 * s->notebook[] and remains valid for the lifetime of the session.
 * Callers must not free the pointer.
 */
CARMEN_API int               carmen_session_notebook_count(
                                 const CarmenSession *s);
CARMEN_API const CarmenClue *carmen_session_notebook_at(
                                 const CarmenSession *s, int index);

/*
 * Villain identity evidence collected by investigating identity-clue sites
 * along the suspect's trail.  Each entry is an i18n key for one id clue.
 * carmen_session_evidence_at returns NULL for an out-of-range index or NULL
 * s.  The returned pointer is borrowed: it points into s->evidence[] and
 * remains valid for the lifetime of the session.  Callers must not free the
 * pointer.
 */
CARMEN_API int         carmen_session_evidence_count(const CarmenSession *s);
CARMEN_API const char *carmen_session_evidence_at(const CarmenSession *s,
                                                  int index);

/*
 * How many identity clues a warrant requires for the active case: the number
 * of identity clues seeded into the trail (CarmenCase.identity_clue_count,
 * normally CARMEN_IDENTITY_CLUES). Front-ends can render progress as
 * carmen_session_evidence_count() / carmen_session_evidence_required().
 * Returns 0 if s is NULL.
 */
CARMEN_API int carmen_session_evidence_required(const CarmenSession *s);

/*
 * True once enough evidence has been collected to issue a warrant and the
 * session is still PLAYING -- i.e. carmen_session_issue_warrant() would no
 * longer be refused with -2 for lack of evidence. Front-ends can watch this
 * flip false->true (evidence is only ever gained via
 * carmen_session_investigate on identity-clue sites) to notify the player
 * they can now make an ID. Returns false if s is NULL.
 */
CARMEN_API bool carmen_session_can_issue_warrant(const CarmenSession *s);

/*
 * Connections leaving the current city. Writes up to max_out connection
 * pointers into out and returns the count (0 if there is no current
 * city). The pointers reference the world's city data and remain valid
 * for the lifetime of the world.
 */
CARMEN_API int carmen_session_connections(const CarmenSession *s,
                                          const CarmenConnection **out,
                                          int max_out);

/*
 * Return the active site indices for the current city.
 *
 * On-trail cities have up to CARMEN_TRAIL_SITES (3) active sites chosen
 * at case-generation time.  Off-trail cities return 0.
 * Writes site indices into out_indices and returns the count.
 */
CARMEN_API int carmen_session_active_sites(const CarmenSession *s,
                                           int *out_indices, int max_out);

/*
 * Travel to a connected city.
 *
 * Deducts time based on the connection's transport mode and distance.
 * Returns 0 on success, -1 if no connection, -2 if out of time,
 * -3 if the session is not in PLAYING state, -4 if the move limit was
 * reached (settings.move_limit > 0 and moves exhausted).
 */
CARMEN_API int carmen_session_travel(CarmenSession *s, const char *dest_id);

/*
 * Investigate a site in the current city.
 *
 * On-trail cities return a deterministic clue assigned at case-
 * generation time: a positive clue pointing to the next trail city, a
 * herring, a negative, or a suspect-identity clue. Investigating an
 * identity-clue site also records that clue into the session's evidence
 * (deduplicated, so revisiting the same site does not double-count).
 * Off-trail cities always return a negative clue.
 *
 * site_idx must be one of the active site indices returned by
 * carmen_session_active_sites, or a valid site index if off-trail.
 *
 * The returned pointer points into the session's notebook and remains
 * valid for the lifetime of the session.
 *
 * Returns NULL if site_idx is not active, notebook is full, or
 * session is not PLAYING.
 */
CARMEN_API const CarmenClue *carmen_session_investigate(CarmenSession *s,
                                                        int site_idx);

/*
 * Issue (or change) an arrest warrant for a villain.
 * villain_idx is the index into FITNA_VILLAINS[].
 *
 * A warrant always requires a full set of villain identity clues to have
 * been collected first, matching the classic collect-clues-then-warrant
 * flow. Evidence is gathered by investigating the identity-clue sites
 * seeded across the trail cities, so "full set" means every identity clue
 * the case placed (CarmenCase.identity_clue_count, normally
 * CARMEN_IDENTITY_CLUES; see carmen_session_evidence_required()).
 *
 * Returns 0 on success, -1 on invalid index or not PLAYING, -2 if
 * insufficient evidence has been collected.
 */
CARMEN_API int carmen_session_issue_warrant(CarmenSession *s, int villain_idx);

/*
 * Attempt to arrest the villain at the current city.
 *
 * Succeeds only if the player is at the hideout city with a warrant
 * for the correct villain.
 *
 * Returns the resulting CarmenSessionStatus:
 *   CARMEN_STATUS_WON           -- correct arrest
 *   CARMEN_STATUS_LOST_WRONG_ARREST -- wrong villain
 *   CARMEN_STATUS_LOST_NO_WARRANT   -- no warrant issued
 *   CARMEN_STATUS_NOT_AT_HIDEOUT    -- not at hideout (no state change)
 */
CARMEN_API CarmenSessionStatus carmen_session_arrest(CarmenSession *s);

#endif
