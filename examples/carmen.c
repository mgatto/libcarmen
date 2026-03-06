#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "carmen/carmen.h"

#define SEP "------------------------------------------------------------"

static void heading(const char *text)
{
    printf("%s\n  %s\n%s\n", SEP, text, SEP);
}

int main(void)
{
    srand((unsigned)time(NULL));

    CarmenWorld *world = carmen_world_create();
    if (!world) {
        fprintf(stderr, "Failed to allocate CarmenWorld\n");
        return 1;
    }
    carmen_seed_build_world(world);
    printf("World loaded: %d cities across %d continents.\n\n",
           world->city_count, carmen_world_continent_count(world));

    /* ------------------------------------------------ O(1) city lookup */
    heading("O(1) City Lookup");
    CarmenCity *cairo = carmen_world_find(world, "cairo");
    if (cairo) {
        carmen_city_print_detail(cairo);
        printf("\n  Sites:\n");
        for (int i = 0; i < cairo->site_count; i++) {
            printf("    - ");
            carmen_site_print(&cairo->sites[i]);
            printf("\n");
        }
        printf("  Connections:\n");
        for (int i = 0; i < cairo->connection_count; i++) {
            printf("    ");
            carmen_connection_print(&cairo->connections[i]);
            printf("\n");
        }
    }
    printf("\n");

    /* ---------------------------------------- secondary index: by continent */
    heading("Cities by Continent");
    const char *continents[] = {
        "Africa", "Asia", "Europe", "North America", "Oceania", "South America"
    };
    CarmenCity *buf[CARMEN_MAX_CITIES];
    for (int ci = 0; ci < 6; ci++) {
        int n = carmen_world_cities_in_continent(world, continents[ci],
                                                 buf, CARMEN_MAX_CITIES);
        printf("  %-18s", continents[ci]);
        for (int j = 0; j < n; j++) {
            if (j > 0) printf(", ");
            printf("%s", buf[j]->name);
        }
        printf("\n");
    }
    printf("\n");

    /* ------------------------------------------- all museums in the world */
    heading("All Museums in the World");
    for (int i = 0; i < world->city_count; i++) {
        CarmenCity *c = &world->storage[i];
        const CarmenSite *museums[CARMEN_MAX_SITES];
        int n = carmen_city_sites_of_type(c, "museum", museums,
                                          CARMEN_MAX_SITES);
        for (int j = 0; j < n; j++)
            printf("  %-24s%s\n", c->name, museums[j]->name);
    }
    printf("\n");

    /* ----------------------------------------- destinations from a city */
    heading("Direct Flights from Tokyo");
    int nd = carmen_world_destinations_from(world, "tokyo",
                                            buf, CARMEN_MAX_CITIES);
    for (int i = 0; i < nd; i++) {
        printf("  -> ");
        carmen_city_print(buf[i]);
        printf("\n");
    }
    printf("\n");

    /* -------------------------------- BFS: reachable within N hops */
    heading("Cities Reachable from Paris within 2 Hops");
    int nr = carmen_world_reachable_within(world, "paris", 2,
                                           buf, CARMEN_MAX_CITIES);
    for (int i = 0; i < nr; i++) {
        printf("  ");
        carmen_city_print(buf[i]);
        printf("\n");
    }
    printf("\n");

    /* ----------------------------- shortest path between two cities */
    heading("Shortest Path: Buenos Aires -> Beijing");
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(world, "buenos_aires", "beijing",
                                          path, CARMEN_MAX_CITIES);
    if (hops >= 0) {
        printf("  ");
        for (int i = 0; i <= hops; i++) {
            CarmenCity *pc = carmen_world_find(world, path[i]);
            if (i > 0) printf(" -> ");
            printf("%s", pc ? pc->name : path[i]);
        }
        printf("\n  (%d hops)\n", hops);
    } else {
        printf("  No path found!\n");
    }
    printf("\n");

    /* ---------------------------------------- random clues from Paris */
    heading("Investigating Paris...");
    CarmenCity *paris = carmen_world_find(world, "paris");
    if (paris) {
        for (int i = 0; i < paris->site_count; i++) {
            const CarmenClue *clue = carmen_site_random_clue(
                &paris->sites[i]);
            if (clue) {
                printf("  [%s]\n", paris->sites[i].name);
                printf("    \"%s\" -> %s\n",
                       clue->text, clue->target_city_id);
            }
        }
    }
    printf("\n");

    carmen_world_free(world);
    return 0;
}
