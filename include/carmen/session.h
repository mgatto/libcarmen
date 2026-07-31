#ifndef CARMEN_SESSION_H
#define CARMEN_SESSION_H

#include "carmen_export.h"
#include "case.h"

#define CARMEN_MAX_NOTEBOOK 64

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
  CarmenWorld *world;
  CarmenCase active_case;
  CarmenCaseSettings settings;
  CarmenSessionStatus status;
  char current_city_id[CARMEN_MAX_NAME_LEN];
  char visited[CARMEN_MAX_VISITED][CARMEN_MAX_NAME_LEN];
  int visited_count;
  int time_remaining_hrs;
  int moves;
  CarmenClue notebook[CARMEN_MAX_NOTEBOOK];
  int notebook_count;
  char evidence[FITNA_MAX_ID_CLUES][CARMEN_MAX_CLUE_LEN];
  int evidence_count;
  int warrant_villain_idx;                 /* -1 = no warrant issued */
  unsigned int hideout_investigated_sites; /* bitmask: bit N = site N already
                                              gave evidence */
} CarmenSession;

/* Lifecycle */
CARMEN_API int carmen_session_start(CarmenSession *s, CarmenWorld *w,
                                    const CarmenCaseSettings *settings);
CARMEN_API void carmen_session_reset(CarmenSession *s);

/* Queries (read-only, for UI) */
CARMEN_API CarmenSessionStatus carmen_session_status(const CarmenSession *s);
CARMEN_API const CarmenCity *
carmen_session_current_city(const CarmenSession *s);
CARMEN_API const CarmenCase *carmen_session_case(const CarmenSession *s);
CARMEN_API int carmen_session_time_remaining(const CarmenSession *s);
CARMEN_API int carmen_session_moves(const CarmenSession *s);

/*
 * Cities-visited history (chronological, includes the origin and any
 * revisits). Capped at the session's visited_history_size setting.
 * carmen_session_visited_at returns NULL for an out-of-range index.
 */
CARMEN_API int         carmen_session_visited_count(const CarmenSession *s);
CARMEN_API const char *carmen_session_visited_at(const CarmenSession *s,
                                                 int index);

/*
 * Clue notebook (clues dispensed by carmen_session_investigate, oldest
 * first). carmen_session_notebook_at returns NULL for an out-of-range
 * index.
 */
CARMEN_API int               carmen_session_notebook_count(
                                 const CarmenSession *s);
CARMEN_API const CarmenClue *carmen_session_notebook_at(
                                 const CarmenSession *s, int index);

/*
 * Villain identity evidence collected at the hideout. Each entry is an
 * i18n key for one id clue. carmen_session_evidence_at returns NULL for
 * an out-of-range index.
 */
CARMEN_API int         carmen_session_evidence_count(const CarmenSession *s);
CARMEN_API const char *carmen_session_evidence_at(const CarmenSession *s,
                                                  int index);

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
 * generation time (2 of 3 active sites give a positive clue pointing
 * to the next trail city, 1 gives a herring or negative).
 * Off-trail cities always return a negative clue.
 * At the villain's hideout, also collects villain identity evidence.
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
 * been collected first (evidence_count == FITNA_MAX_ID_CLUES), matching the
 * classic collect-clues-then-warrant flow. Evidence is gathered only by
 * investigating sites at the villain's hideout.
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
