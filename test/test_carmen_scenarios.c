#include "unity.h"
#include "carmen/carmen.h"
#include <string.h>
#include <stdlib.h>

static CarmenWorld world;

void setUp(void)
{
    srand(42);
    carmen_seed_build_islamic_world(&world);
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

static void test_world_has_continent_index(void)
{
    TEST_ASSERT_TRUE(carmen_world_continent_count(&world) > 0);
}

/* ------------------------------------ O(1) lookup */

static void test_find_cairo(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    TEST_ASSERT_EQUAL_STRING("city.cairo.name", cairo->name);
    TEST_ASSERT_EQUAL_STRING("city.cairo.country", cairo->country);
    TEST_ASSERT_EQUAL_STRING("city.cairo.continent", cairo->continent);
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

static void test_continent_query_returns_city(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_continent(&world,
                "city.istanbul.continent", out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(1, n);
    TEST_ASSERT_EQUAL_STRING("istanbul", out[0]->id);
}

static void test_both_continents_have_cities(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n_eu = carmen_world_cities_in_continent(&world,
                   "city.istanbul.continent", out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n_eu > 0);

    int n_af = carmen_world_cities_in_continent(&world,
                   "city.cairo.continent", out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n_af > 0);
}

/* ----------------------- site type query */

static void test_cairo_has_mosque(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    const CarmenSite *mosques[CARMEN_MAX_SITES];
    int n = carmen_city_sites_of_type(cairo, "type.mosque", mosques,
                                      CARMEN_MAX_SITES);
    TEST_ASSERT_TRUE(n >= 1);
}

/* -------------------- destinations from */

static void test_dubai_has_direct_flights(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_destinations_from(&world, "dubai",
                                           out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n >= 1);

    int found_muscat = 0;
    for (int i = 0; i < n; i++)
        if (strcmp(out[i]->id, "muscat") == 0) found_muscat = 1;
    TEST_ASSERT_TRUE(found_muscat);
}

/* -------------------- reachable within */

static void test_istanbul_reachable_within_2_hops(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "istanbul", 2,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(n >= 2);
}

/* ----- shortest path istanbul -> isfahan */

static void test_shortest_path_istanbul_to_isfahan(void)
{
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "istanbul", "isfahan",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_TRUE(hops > 0);
    TEST_ASSERT_EQUAL_STRING("istanbul", path[0]);
    TEST_ASSERT_EQUAL_STRING("isfahan", path[hops]);
}

/* -------- inbound clue pools */

static void test_istanbul_has_inbound_pool(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(ist);
    TEST_ASSERT_TRUE(ist->inbound_clue_count > 0);
}

static void test_istanbul_inbound_clues_have_keys(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(ist);
    for (int i = 0; i < ist->inbound_clue_count; i++)
        TEST_ASSERT_TRUE(strlen(ist->inbound_clues[i]) > 0);
}

static void test_istanbul_first_site_is_suleymaniye(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(ist);
    TEST_ASSERT_EQUAL_STRING("site.istanbul.suleymaniye", ist->sites[0].name);
}

/* ---------------------- bidirectional routes */

static void test_routes_are_bidirectional(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    CarmenCity *sar = carmen_world_find(&world, "sarajevo");
    TEST_ASSERT_NOT_NULL(ist);
    TEST_ASSERT_NOT_NULL(sar);
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(ist, "sarajevo"));
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(sar, "istanbul"));
}

/* ---------------------- convenience clue API */

static void test_city_random_inbound_clue_from_cairo(void)
{
    CarmenCity *cairo = carmen_world_find(&world, "cairo");
    TEST_ASSERT_NOT_NULL(cairo);
    const char *clue = carmen_city_random_inbound_clue(cairo);
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_TRUE(strlen(clue) > 0);
}

static void test_city_random_inbound_clue_from_istanbul(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(ist);
    const char *clue = carmen_city_random_inbound_clue(ist);
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_TRUE(strlen(clue) > 0);
}

/* ------------- each city has 3+ sites and a non-empty inbound pool */

static void test_every_city_has_sites_and_inbound_clues(void)
{
    for (int i = 0; i < world.city_count; i++) {
        CarmenCity *c = &world.storage[i];
        TEST_ASSERT_TRUE_MESSAGE(c->site_count >= 3, c->id);
        TEST_ASSERT_TRUE_MESSAGE(c->inbound_clue_count >= 2, c->id);
    }
}

/* ---------------------- destroy then reinit */

static void test_destroy_and_reinit(void)
{
    carmen_world_destroy(&world);
    carmen_world_init(&world);
    TEST_ASSERT_EQUAL_INT(0, world.city_count);
    TEST_ASSERT_NULL(world.city_map);
    carmen_seed_build_islamic_world(&world);
    TEST_ASSERT_TRUE(world.city_count > 0);
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_world_has_expected_city_count);
    RUN_TEST(test_world_has_continent_index);
    RUN_TEST(test_find_cairo);
    RUN_TEST(test_cairo_has_sites);
    RUN_TEST(test_cairo_has_connections);
    RUN_TEST(test_continent_query_returns_city);
    RUN_TEST(test_both_continents_have_cities);
    RUN_TEST(test_cairo_has_mosque);
    RUN_TEST(test_dubai_has_direct_flights);
    RUN_TEST(test_istanbul_reachable_within_2_hops);
    RUN_TEST(test_shortest_path_istanbul_to_isfahan);
    RUN_TEST(test_istanbul_has_inbound_pool);
    RUN_TEST(test_istanbul_inbound_clues_have_keys);
    RUN_TEST(test_istanbul_first_site_is_suleymaniye);
    RUN_TEST(test_routes_are_bidirectional);
    RUN_TEST(test_city_random_inbound_clue_from_cairo);
    RUN_TEST(test_city_random_inbound_clue_from_istanbul);
    RUN_TEST(test_every_city_has_sites_and_inbound_clues);
    RUN_TEST(test_destroy_and_reinit);
    return UNITY_END();
}
