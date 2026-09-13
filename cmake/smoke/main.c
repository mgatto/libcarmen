/*
 * main.c -- minimal find_package(carmen) smoke consumer. Build the built-in
 * world, start a session, and read back the current city and status; proves a
 * downstream CMake project links and runs against an extracted SDK archive.
 */
#include <carmen/carmen.h>
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

    const CarmenCity *current = carmen_session_current_city(&s);
    printf("smoke: started in %s (status %d)\n", current->id, carmen_session_status(&s));

    carmen_world_free(w);
    return 0;
}
