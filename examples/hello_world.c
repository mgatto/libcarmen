/*
 * hello_world.c -- minimal libcarmen program: build the built-in world, start
 * a session, and read back the current city and status. The shortest proof
 * that the library links and runs.
 *
 * Build (in-tree) with either:
 *   make                   # builds build/hello_world
 *   cmake -S . -B build && cmake --build build -j   # builds hello_world
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

    /* City name/country fields hold i18n keys; resolve them with
     * carmen_i18n_get() (see i18n.c). `id` is the raw, stable kind. */
    const CarmenCity *current = carmen_session_current_city(&s);
    printf("Started in %s (status %d)\n",
           current->id, carmen_session_status(&s));

    carmen_world_free(w);
    return 0;
}
