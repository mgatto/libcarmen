#ifndef CARMEN_SESSION_H
#define CARMEN_SESSION_H

#include "carmen/carmen_export.h"
#include "carmen/case.h"

#define CARMEN_MAX_NOTEBOOK 64

typedef enum {
  CARMEN_STATUS_PLAYING,
  CARMEN_STATUS_WON,
  CARMEN_STATUS_LOST_TIME,
  CARMEN_STATUS_LOST_WRONG_ARREST,
  CARMEN_STATUS_LOST_NO_WARRANT,
  CARMEN_STATUS_NOT_AT_HIDEOUT
} CarmenSessionStatus;

typedef struct {
  CarmenWorld *world;
  CarmenCase active_case;
  CarmenSessionStatus status;
  char current_city_id[CARMEN_MAX_NAME_LEN];
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
 * Travel to a connected city.
 *
 * Deducts time based on the connection's transport mode and distance.
 * Returns 0 on success, -1 if no connection, -2 if out of time,
 * -3 if the session is not in PLAYING state.
 */
CARMEN_API int carmen_session_travel(CarmenSession *s, const char *dest_id);

/*
 * Investigate a site in the current city.
 *
 * Returns a clue from site at site_idx, applying difficulty-based clue
 * selection: on-trail cities yield positive clues pointing to the next
 * trail stop (probability scales with difficulty), otherwise a negative
 * or misleading clue is returned. The clue is appended to the notebook.
 *
 * At the villain's hideout, also collects villain identity evidence.
 *
 * The returned pointer points into the session's notebook and remains
 * valid for the lifetime of the session.
 *
 * Returns NULL if site_idx is out of range, notebook is full, or
 * session is not PLAYING.
 */
CARMEN_API const CarmenClue *carmen_session_investigate(CarmenSession *s,
                                                        int site_idx);

/*
 * Issue (or change) an arrest warrant for a villain.
 * villain_idx is the index into FITNA_VILLAINS[].
 * Returns 0 on success, -1 on invalid index or not PLAYING.
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
