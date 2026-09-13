/*
 * save_load.c -- serialize a session to a JSON string and restore it into a
 * fresh CarmenSession against the same world, exercising the save/load API
 * (schema CARMEN_SAVE_SCHEMA_VERSION). The world pointer is re-attached on
 * load; the generated connection graph is restored from the saved "edges".
 */
#include "carmen/carmen.h"
#include <stdio.h>
#include <string.h>

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

    /* Query the required size, then serialize into a stack buffer. */
    char json[8192];
    int  needed = carmen_session_save(&s, NULL, 0);
    if (needed <= 0 || needed > (int)sizeof(json)) {
        fprintf(stderr, "save size check failed\n");
        carmen_world_free(w);
        return 1;
    }
    if (carmen_session_save(&s, json, (int)sizeof(json)) <= 0) {
        fprintf(stderr, "carmen_session_save() failed\n");
        carmen_world_free(w);
        return 1;
    }

    CarmenSession restored;
    if (carmen_session_load(&restored, w, json, strlen(json)) != 1) {
        fprintf(stderr, "carmen_session_load() failed\n");
        carmen_world_free(w);
        return 1;
    }

    printf("round-trip ok: restored at %s\n",
           carmen_session_current_city(&restored)->id);

    carmen_world_free(w);
    return 0;
}
