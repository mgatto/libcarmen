#include "unity.h"
#include "carmen/game_world.h"
#include <stdlib.h>
#include <string.h>

static CarmenWorld world;

void setUp(void) { carmen_world_init(&world); }
void tearDown(void) { carmen_world_destroy(&world); }

/* ------------------------------------------------------------ lifecycle */

static void test_world_init_zeroes_state(void)
{
    TEST_ASSERT_EQUAL_INT(0, world.city_count);
    TEST_ASSERT_EQUAL_INT(0, world.continent_count);
    TEST_ASSERT_EQUAL_INT(0, world.country_count);
    TEST_ASSERT_NULL(world.city_map);
}

/* ------------------------------------------------------------- add_city */

static void test_add_city_returns_pointer(void)
{
    CarmenCity *c = carmen_world_add_city(&world, "paris", "Paris", NULL,
                                          "France", "Europe", 48.86, 2.35);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_STRING("paris", c->id);
    TEST_ASSERT_EQUAL_INT(1, world.city_count);
}

static void test_add_city_updates_continent_index(void)
{
    carmen_world_add_city(&world, "paris", "Paris", NULL,
                          "France", "Europe", 48.86, 2.35);
    carmen_world_add_city(&world, "london", "London", NULL,
                          "England", "Europe", 51.51, -0.13);
    carmen_world_add_city(&world, "cairo", "Cairo", NULL,
                          "Egypt", "Africa", 30.04, 31.24);

    TEST_ASSERT_EQUAL_INT(2, world.continent_count);
}

static void test_add_city_returns_null_at_capacity(void)
{
    for (int i = 0; i < CARMEN_MAX_CITIES; i++) {
        char id[16];
        snprintf(id, sizeof(id), "city_%d", i);
        TEST_ASSERT_NOT_NULL(
            carmen_world_add_city(&world, id, id, NULL, "C", "X", 0, 0));
    }
    TEST_ASSERT_NULL(
        carmen_world_add_city(&world, "overflow", "Overflow", NULL, "C", "X", 0, 0));
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_CITIES, world.city_count);
}

/* --------------------------------------------------------------- find */

static void test_find_existing_city(void)
{
    carmen_world_add_city(&world, "tokyo", "Tokyo", NULL,
                          "Japan", "Asia", 35.68, 139.69);
    CarmenCity *found = carmen_world_find(&world, "tokyo");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Tokyo", found->name);
}

static void test_find_nonexistent_city(void)
{
    carmen_world_add_city(&world, "tokyo", "Tokyo", NULL,
                          "Japan", "Asia", 35.68, 139.69);
    TEST_ASSERT_NULL(carmen_world_find(&world, "atlantis"));
}

static void test_find_among_many(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "C", "X", 0, 0);
    CarmenCity *b = carmen_world_find(&world, "b");
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_EQUAL_STRING("B", b->name);
}

/* ------------------------------------------------ cities_in_continent */

static void test_cities_in_continent_returns_correct_set(void)
{
    carmen_world_add_city(&world, "paris", "Paris", NULL,
                          "France", "Europe", 48.86, 2.35);
    carmen_world_add_city(&world, "rome", "Rome", NULL,
                          "Italy", "Europe", 41.90, 12.50);
    carmen_world_add_city(&world, "cairo", "Cairo", NULL,
                          "Egypt", "Africa", 30.04, 31.24);

    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_continent(&world, "Europe",
                                             out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(2, n);
}

static void test_cities_in_continent_unknown_returns_zero(void)
{
    carmen_world_add_city(&world, "paris", "Paris", NULL,
                          "France", "Europe", 48.86, 2.35);
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_continent(&world, "Antarctica",
                                             out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

/* ------------------------------------------------ destinations_from */

static void test_destinations_from_returns_connected_cities(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "C", "X", 0, 0);

    CarmenCity *a = carmen_world_find(&world, "a");
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 100, "flight");
    carmen_city_add_connection(a, &conn);
    carmen_connection_init(&conn, "c", 200, "flight");
    carmen_city_add_connection(a, &conn);

    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_destinations_from(&world, "a",
                                           out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(2, n);
}

static void test_destinations_from_unknown_city(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_destinations_from(&world, "ghost",
                                           out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

/* ----------------------------------------------- reachable_within */

static void test_reachable_within_zero_hops(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "a", 0,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

static void test_reachable_within_one_hop(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "C", "X", 0, 0);

    CarmenCity *a = carmen_world_find(&world, "a");
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 100, "flight");
    carmen_city_add_connection(a, &conn);

    CarmenCity *b = carmen_world_find(&world, "b");
    carmen_connection_init(&conn, "c", 200, "flight");
    carmen_city_add_connection(b, &conn);

    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "a", 1,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(1, n);
    TEST_ASSERT_EQUAL_STRING("B", out[0]->name);
}

static void test_reachable_within_two_hops(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "C", "X", 0, 0);

    CarmenCity *a = carmen_world_find(&world, "a");
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 100, "flight");
    carmen_city_add_connection(a, &conn);

    CarmenCity *b = carmen_world_find(&world, "b");
    carmen_connection_init(&conn, "c", 200, "flight");
    carmen_city_add_connection(b, &conn);

    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "a", 2,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(2, n);
}

static void test_reachable_within_unknown_city(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_reachable_within(&world, "ghost", 2,
                                          out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

/* --------------------------------------------------- shortest_path */

static void test_shortest_path_same_city(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "a", "a",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, hops);
    TEST_ASSERT_EQUAL_STRING("a", path[0]);
}

static void test_shortest_path_direct_connection(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);

    CarmenCity *a = carmen_world_find(&world, "a");
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 100, "flight");
    carmen_city_add_connection(a, &conn);

    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "a", "b",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(1, hops);
    TEST_ASSERT_EQUAL_STRING("a", path[0]);
    TEST_ASSERT_EQUAL_STRING("b", path[1]);
}

static void test_shortest_path_multi_hop(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "C", "X", 0, 0);

    CarmenConnection conn;
    CarmenCity *a = carmen_world_find(&world, "a");
    carmen_connection_init(&conn, "b", 100, "flight");
    carmen_city_add_connection(a, &conn);

    CarmenCity *b = carmen_world_find(&world, "b");
    carmen_connection_init(&conn, "c", 200, "flight");
    carmen_city_add_connection(b, &conn);

    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "a", "c",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(2, hops);
    TEST_ASSERT_EQUAL_STRING("a", path[0]);
    TEST_ASSERT_EQUAL_STRING("b", path[1]);
    TEST_ASSERT_EQUAL_STRING("c", path[2]);
}

static void test_shortest_path_no_path(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    carmen_world_add_city(&world, "b", "B", NULL, "C", "X", 0, 0);
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "a", "b",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(-1, hops);
}

static void test_shortest_path_nonexistent_origin(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "ghost", "a",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(-1, hops);
}

static void test_shortest_path_nonexistent_destination(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    const char *path[CARMEN_MAX_CITIES];
    int hops = carmen_world_shortest_path(&world, "a", "ghost",
                                          path, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(-1, hops);
}

/* ------------------------------------------------ cities_in_country */

static void test_cities_in_country_returns_correct_set(void)
{
    carmen_world_add_city(&world, "paris", "Paris", NULL,
                          "France", "Europe", 48.86, 2.35);
    carmen_world_add_city(&world, "lyon", "Lyon", NULL,
                          "France", "Europe", 45.76, 4.84);
    carmen_world_add_city(&world, "cairo", "Cairo", NULL,
                          "Egypt", "Africa", 30.04, 31.24);

    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_country(&world, "France",
                                            out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(2, n);
}

static void test_cities_in_country_unknown_returns_zero(void)
{
    carmen_world_add_city(&world, "paris", "Paris", NULL,
                          "France", "Europe", 48.86, 2.35);
    CarmenCity *out[CARMEN_MAX_CITIES];
    int n = carmen_world_cities_in_country(&world, "Narnia",
                                            out, CARMEN_MAX_CITIES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

static void test_cities_in_country_null_args(void)
{
    CarmenCity *out[CARMEN_MAX_CITIES];
    TEST_ASSERT_EQUAL_INT(0,
        carmen_world_cities_in_country(NULL, "France", out, CARMEN_MAX_CITIES));
    TEST_ASSERT_EQUAL_INT(0,
        carmen_world_cities_in_country(&world, NULL, out, CARMEN_MAX_CITIES));
    TEST_ASSERT_EQUAL_INT(0,
        carmen_world_cities_in_country(&world, "France", NULL, CARMEN_MAX_CITIES));
    TEST_ASSERT_EQUAL_INT(0,
        carmen_world_cities_in_country(&world, "France", out, 0));
}

/* -------------------------------------------------- continent_count */

static void test_continent_count_empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, carmen_world_continent_count(&world));
}

static void test_continent_count_after_adds(void)
{
    carmen_world_add_city(&world, "p", "P", NULL, "France", "Europe", 0, 0);
    carmen_world_add_city(&world, "c", "C", NULL, "Egypt", "Africa", 0, 0);
    carmen_world_add_city(&world, "l", "L", NULL, "England", "Europe", 0, 0);
    TEST_ASSERT_EQUAL_INT(2, carmen_world_continent_count(&world));
}

/* ------------------------------------------- carmen_world_random_clue */

static void test_random_clue_from_valid_city(void)
{
    srand(42);
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    CarmenCity *a = carmen_world_find(&world, "a");
    CarmenSite s;
    carmen_site_init(&s, "museum", "Museum", "museum");
    carmen_site_add_clue(&s, "The suspect fled east", "tokyo", CARMEN_CLUE_POSITIVE);
    carmen_city_add_site(a, &s);

    const CarmenClue *clue = carmen_world_random_clue(&world, "a");
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_EQUAL_STRING("The suspect fled east", clue->text);
    TEST_ASSERT_EQUAL_STRING("tokyo", clue->target_city_id);
}

static void test_random_clue_from_nonexistent_city(void)
{
    TEST_ASSERT_NULL(carmen_world_random_clue(&world, "ghost"));
}

static void test_random_clue_from_city_with_no_clues(void)
{
    carmen_world_add_city(&world, "a", "A", NULL, "C", "X", 0, 0);
    TEST_ASSERT_NULL(carmen_world_random_clue(&world, "a"));
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_world_init_zeroes_state);
    RUN_TEST(test_add_city_returns_pointer);
    RUN_TEST(test_add_city_updates_continent_index);
    RUN_TEST(test_add_city_returns_null_at_capacity);
    RUN_TEST(test_find_existing_city);
    RUN_TEST(test_find_nonexistent_city);
    RUN_TEST(test_find_among_many);
    RUN_TEST(test_cities_in_continent_returns_correct_set);
    RUN_TEST(test_cities_in_continent_unknown_returns_zero);
    RUN_TEST(test_cities_in_country_returns_correct_set);
    RUN_TEST(test_cities_in_country_unknown_returns_zero);
    RUN_TEST(test_cities_in_country_null_args);
    RUN_TEST(test_destinations_from_returns_connected_cities);
    RUN_TEST(test_destinations_from_unknown_city);
    RUN_TEST(test_reachable_within_zero_hops);
    RUN_TEST(test_reachable_within_one_hop);
    RUN_TEST(test_reachable_within_two_hops);
    RUN_TEST(test_reachable_within_unknown_city);
    RUN_TEST(test_shortest_path_same_city);
    RUN_TEST(test_shortest_path_direct_connection);
    RUN_TEST(test_shortest_path_multi_hop);
    RUN_TEST(test_shortest_path_no_path);
    RUN_TEST(test_shortest_path_nonexistent_origin);
    RUN_TEST(test_shortest_path_nonexistent_destination);
    RUN_TEST(test_continent_count_empty);
    RUN_TEST(test_continent_count_after_adds);
    RUN_TEST(test_random_clue_from_valid_city);
    RUN_TEST(test_random_clue_from_nonexistent_city);
    RUN_TEST(test_random_clue_from_city_with_no_clues);
    return UNITY_END();
}
