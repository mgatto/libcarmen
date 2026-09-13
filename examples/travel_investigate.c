/*
 * travel_investigate.c -- the core play loop in miniature: list the current
 * city's connections and active sites, investigate one, travel along one, and
 * report the evidence gathered. Demonstrates the query/action split of the
 * session API (read-only accessors vs. carmen_session_travel/investigate).
 */
#include "carmen/carmen.h"
#include <stdio.h>

int main(void)
{
    CarmenWorld *w = carmen_world_create();
    if (!w) {
        fprintf(stderr, "carmen_world_create() failed\n");
        return 1;
    }

    carmen_world_build_islamic(w);

    CarmenSession      s;
    CarmenCaseSettings settings = carmen_case_settings_default();
    if (!carmen_session_start(&s, w, &settings)) {
        fprintf(stderr, "carmen_session_start() failed\n");
        carmen_world_free(w);
        return 1;
    }

    const CarmenConnection *conns[CARMEN_MAX_CONNECTIONS];
    int nconn = carmen_session_connections(&s, conns, CARMEN_MAX_CONNECTIONS);
    printf("%s has %d connection(s)\n",
           carmen_session_current_city(&s)->id, nconn);

    int sites[CARMEN_TRAIL_SITES];
    int nsites = carmen_session_active_sites(&s, sites, CARMEN_TRAIL_SITES);
    if (nsites > 0) {
        const CarmenClue *clue = carmen_session_investigate(&s, sites[0]);
        if (clue)
            printf("investigated site -> clue key: %s\n", clue->text);
    }

    if (nconn > 0) {
        if (carmen_session_travel(&s, conns[0]->destination_id) == 0)
            printf("travelled to %s\n", carmen_session_current_city(&s)->id);
    }

    printf("evidence collected: %d (of %d required for a warrant)\n",
           carmen_session_evidence_count(&s),
           carmen_session_evidence_required(&s));

    carmen_world_free(w);
    return 0;
}
