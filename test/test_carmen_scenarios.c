#include "unity.h"
#include "carmen/carmen.h"
#include <string.h>
#include <stdlib.h>

static CarmenWorld world;

void setUp(void)
{
    srand(42);
    carmen_seed_build_world(&world);
}

void tearDown(void)
{
    carmen_world_destroy(&world);
}

/* ------------------------------------------------------ world integrity */

static void test_world_has_expected_city_count(void)
{
    TEST_ASSERT_EQUAL_INT(22, world.city_count);
}

static void test_world_has_six_continents(void)
{
    TEST_ASSERT_EQUAL_INT(6, carmen_world_continent_count(&world));
}

/* ------------------------------------ O(1) lookup (mirrors carmen.c) */

static void test_find_cairo(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    TEST_ASSERT_EQUAL_STRING("Cairo", cairo->name);
    TEST_ASSERT_EQUAL_STRING("Egypt", cairo->country);
    TEST_ASSERT_EQUAL_STRING("Africa", cairo->continent);
}

static void test_cairo_has_sites(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    TEST_ASSERT_TRUE(cairo->site_count > 0);
}

static void test_cairo_has_connections(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    TEST_ASSERT_TRUE(cairo->connection_count > 0);
}

/* ------------------------- continent query */

static void test_europe_contains_paris_london_rome(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_continent(&world, "Europe",
                                             out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n >= 3);

    int found_paris = 0, found_london = 0, found_rome = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(out[i]->id, "paris")  == 0) found_paris  = 1;
        if (strcmp(out[i]->id, "london") == 0) found_london = 1;
        if (strcmp(out[i]->id, "rome")   == 0) found_rome   = 1;
    }
    TEST_ASSERT_TRUE(found_paris);
    TEST_ASSERT_TRUE(found_london);
    TEST_ASSERT_TRUE(found_rome);
}

static void test_all_six_continents_have_cities(void)
{
    const char *continents[] = {
        "Africa", "Asia", "Europe", "North America",
        "Oceania", "South America"
    };
    CarmenCity *out[CARMEN_MAX_CITIES];
    for (int i = 0; i < 6; i++) {
        int n = carmen_world_cities_in_continent(&world, continents[i],
                                                  out, CARMEN_MAX_CITIES);
        TEST_ASSERT_TRUE_MESSAGE(n > 0, continents[i]);
    }
}

/* ----------------------- museums query */

static void test_cairo_has_museum(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    const CarmenSite *museums[CARMEN_MAX_SITES];
    int n = carmen_city_sites_of_type(cairo, "museum", museums,
                                      CARMEN_MAX_SITES);
    TEST_ASSERT_TRUE(n >= 1);
    TEST_ASSERT_EQUAL_STRING("Egyptian Museum", museums[0]->name);
}

/* -------------------- destinations from */

static void test_tokyo_has_direct_flights(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_destinations_from(&world, "tokyo",
                                           out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n >= 1);

    int found_bangkok = 0;
    for (int i = 0; i < n; i++)
        if (strcmp(out[i]->id, "bangkok") == 0) found_bangkok = 1;
    TEST_ASSERT_TRUE(found_bangkok);
}

/* -------------------- reachable within */

static void test_paris_reachable_within_2_hops(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "paris", 2,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n >= 2);
}

/* ----- shortest path Buenos Aires -> Beijing */

static void test_shortest_path_buenos_aires_to_beijing(void)
{
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "buenos_aires", "beijing",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(hops > 0);
    TEST_ASSERT_EQUAL_STRING("buenos_aires", path[0]);
    TEST_ASSERT_EQUAL_STRING("beijing", path[hops]);
}

/* -------- random clues */

static void test_paris_sites_have_clues(void)
{
    CarmenCity *paris = carmen_world_find(&world, "paris");
    TEST_ASSERT_NOT_NULL(paris);
    TEST_ASSERT_TRUE(paris->site_count > 0);

    int total_clues = 0;
    for (int i = 0; i < paris->site_count; i++)
        total_clues += paris->sites[i].clue_count;
    TEST_ASSERT_TRUE(total_clues > 0);
}

static void test_paris_random_clues_have_targets(void)
{
    CarmenCity *paris = carmen_world_find(&world, "paris");
    TEST_ASSERT_NOT_NULL(paris);
    for (int i = 0; i < paris->site_count; i++) {
        const CarmenClue *clue = carmen_site_random_clue(&paris->sites[i]);
        TEST_ASSERT_NOT_NULL(clue);
        TEST_ASSERT_TRUE(strlen(clue->text) > 0);
        TEST_ASSERT_TRUE(strlen(clue->target_city_id) > 0);
    }
}

static void test_paris_louvre_clues_point_to_cairo(void)
{
    CarmenCity *paris = carmen_world_find(&world, "paris");
    TEST_ASSERT_NOT_NULL(paris);
    TEST_ASSERT_EQUAL_STRING("Louvre Museum", paris->sites[0].name);
    for (int i = 0; i < paris->sites[0].clue_count; i++)
        TEST_ASSERT_EQUAL_STRING("cairo",
                                 paris->sites[0].clues[i].target_city_id);
}

/* ---------------------- bidirectional routes */

static void test_routes_are_bidirectional(void)
{
    CarmenCity *paris  = carmen_world_find(&world, "paris");
    CarmenCity *london = carmen_world_find(&world, "london");
    TEST_ASSERT_NOT_NULL(paris);
    TEST_ASSERT_NOT_NULL(london);
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(paris,  "london"));
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(london, "paris"));
}

/* ---------------------- convenience clue API */

static void test_world_random_clue_from_cairo(void)
{
    const CarmenClue *clue = carmen_world_random_clue(&world, "cairo");
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_TRUE(strlen(clue->text) > 0);
    TEST_ASSERT_TRUE(strlen(clue->target_city_id) > 0);
}

static void test_city_random_clue_from_paris(void)
{
    CarmenCity *paris = carmen_world_find(&world, "paris");
    TEST_ASSERT_NOT_NULL(paris);
    const CarmenClue *clue = carmen_city_random_clue(paris);
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_TRUE(strlen(clue->text) > 0);
    TEST_ASSERT_TRUE(strlen(clue->target_city_id) > 0);
}

/* ---------------------- destroy then reinit */

static void test_destroy_and_reinit(void)
{
    carmen_world_destroy(&world);
    carmen_world_init(&world);
    TEST_ASSERT_EQUAL_INT(0, world.city_count);
    TEST_ASSERT_NULL(world.city_map);
    carmen_seed_build_world(&world);
    TEST_ASSERT_TRUE(world.city_count > 0);
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_world_has_expected_city_count);
    RUN_TEST(test_world_has_six_continents);
    RUN_TEST(test_find_cairo);
    RUN_TEST(test_cairo_has_sites);
    RUN_TEST(test_cairo_has_connections);
    RUN_TEST(test_europe_contains_paris_london_rome);
    RUN_TEST(test_all_six_continents_have_cities);
    RUN_TEST(test_cairo_has_museum);
    RUN_TEST(test_tokyo_has_direct_flights);
    RUN_TEST(test_paris_reachable_within_2_hops);
    RUN_TEST(test_shortest_path_buenos_aires_to_beijing);
    RUN_TEST(test_paris_sites_have_clues);
    RUN_TEST(test_paris_random_clues_have_targets);
    RUN_TEST(test_paris_louvre_clues_point_to_cairo);
    RUN_TEST(test_routes_are_bidirectional);
    RUN_TEST(test_world_random_clue_from_cairo);
    RUN_TEST(test_city_random_clue_from_paris);
    RUN_TEST(test_destroy_and_reinit);
    return UNITY_END();
}
