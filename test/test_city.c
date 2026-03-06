#include "unity.h"
#include "carmen/city.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ------------------------------------------------------ helper: make a city */

static CarmenCity make_test_city(void)
{
    CarmenCity c;
    carmen_city_init(&c, "paris", "Paris", NULL, "France", "Europe", 48.86, 2.35);
    return c;
}

/* ------------------------------------------------------------- carmen_city_init */

static void test_city_init_sets_all_fields(void)
{
    CarmenCity c;
    carmen_city_init(&c, "cairo", "Cairo", NULL, "Egypt", "Africa", 30.04, 31.24);
    TEST_ASSERT_EQUAL_STRING("cairo", c.id);
    TEST_ASSERT_EQUAL_STRING("Cairo", c.name);
    TEST_ASSERT_EQUAL_STRING("Egypt", c.country);
    TEST_ASSERT_EQUAL_STRING("Africa", c.continent);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 30.04, c.latitude);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 31.24, c.longitude);
    TEST_ASSERT_EQUAL_INT(0, c.site_count);
    TEST_ASSERT_EQUAL_INT(0, c.connection_count);
}

static void test_city_init_zeroes_arrays(void)
{
    CarmenCity c;
    memset(&c, 0xFF, sizeof(c));
    carmen_city_init(&c, "test", "Test", NULL, "Country", "Continent", 0, 0);
    TEST_ASSERT_EQUAL_INT(0, c.site_count);
    TEST_ASSERT_EQUAL_INT(0, c.connection_count);
}

/* --------------------------------------------------------- carmen_city_add_site */

static void test_city_add_site_increments_count(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s;
    carmen_site_init(&s, "Louvre", "museum");
    carmen_city_add_site(&c, &s);
    TEST_ASSERT_EQUAL_INT(1, c.site_count);
    TEST_ASSERT_EQUAL_STRING("Louvre", c.sites[0].name);
}

static void test_city_add_site_copies_data(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s;
    carmen_site_init(&s, "Tower", "landmark");
    carmen_site_add_clue(&s, "A test clue", "london", CARMEN_CLUE_POSITIVE);
    carmen_city_add_site(&c, &s);

    TEST_ASSERT_EQUAL_STRING("Tower", c.sites[0].name);
    TEST_ASSERT_EQUAL_INT(1, c.sites[0].clue_count);
    TEST_ASSERT_EQUAL_STRING("A test clue", c.sites[0].clues[0].text);
    TEST_ASSERT_EQUAL_STRING("london", c.sites[0].clues[0].target_city_id);
}

static void test_city_add_site_respects_max(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s;
    carmen_site_init(&s, "S", "landmark");
    for (int i = 0; i < CARMEN_MAX_SITES + 3; i++)
        carmen_city_add_site(&c, &s);
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_SITES, c.site_count);
}

/* --------------------------------------------------- carmen_city_add_connection */

static void test_city_add_connection_increments_count(void)
{
    CarmenCity c = make_test_city();
    CarmenConnection conn;
    carmen_connection_init(&conn, "london", 340, "flight");
    carmen_city_add_connection(&c, &conn);
    TEST_ASSERT_EQUAL_INT(1, c.connection_count);
    TEST_ASSERT_EQUAL_STRING("london", c.connections[0].destination_id);
}

static void test_city_add_connection_respects_max(void)
{
    CarmenCity c = make_test_city();
    CarmenConnection conn;
    carmen_connection_init(&conn, "x", 100, "flight");
    for (int i = 0; i < CARMEN_MAX_CONNECTIONS + 3; i++)
        carmen_city_add_connection(&c, &conn);
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_CONNECTIONS, c.connection_count);
}

/* ------------------------------------------------ carmen_city_has_connection_to */

static void test_city_has_connection_to_found(void)
{
    CarmenCity c = make_test_city();
    CarmenConnection conn;
    carmen_connection_init(&conn, "rome", 1100, "flight");
    carmen_city_add_connection(&c, &conn);
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(&c, "rome"));
}

static void test_city_has_connection_to_not_found(void)
{
    CarmenCity c = make_test_city();
    TEST_ASSERT_EQUAL_INT(0, carmen_city_has_connection_to(&c, "tokyo"));
}

static void test_city_has_connection_to_among_multiple(void)
{
    CarmenCity c = make_test_city();
    const char *dests[] = {"london", "rome", "oslo"};
    for (int i = 0; i < 3; i++) {
        CarmenConnection conn;
        carmen_connection_init(&conn, dests[i], 1000 + i * 100, "flight");
        carmen_city_add_connection(&c, &conn);
    }
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(&c, "oslo"));
    TEST_ASSERT_EQUAL_INT(0, carmen_city_has_connection_to(&c, "cairo"));
}

/* --------------------------------------------------- carmen_city_sites_of_type */

static void test_city_sites_of_type_filters_correctly(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s1, s2, s3;
    carmen_site_init(&s1, "Louvre", "museum");
    carmen_site_init(&s2, "Eiffel Tower", "landmark");
    carmen_site_init(&s3, "Orsay", "museum");
    carmen_city_add_site(&c, &s1);
    carmen_city_add_site(&c, &s2);
    carmen_city_add_site(&c, &s3);

    const CarmenSite *museums[CARMEN_MAX_SITES];
    int n = carmen_city_sites_of_type(&c, "museum", museums, CARMEN_MAX_SITES);
    TEST_ASSERT_EQUAL_INT(2, n);
    TEST_ASSERT_EQUAL_STRING("Louvre", museums[0]->name);
    TEST_ASSERT_EQUAL_STRING("Orsay", museums[1]->name);
}

static void test_city_sites_of_type_returns_zero_when_none(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s;
    carmen_site_init(&s, "Eiffel Tower", "landmark");
    carmen_city_add_site(&c, &s);

    const CarmenSite *museums[CARMEN_MAX_SITES];
    int n = carmen_city_sites_of_type(&c, "museum", museums, CARMEN_MAX_SITES);
    TEST_ASSERT_EQUAL_INT(0, n);
}

static void test_city_sites_of_type_respects_max_out(void)
{
    CarmenCity c = make_test_city();
    for (int i = 0; i < 5; i++) {
        CarmenSite s;
        carmen_site_init(&s, "Museum", "museum");
        carmen_city_add_site(&c, &s);
    }

    const CarmenSite *museums[2];
    int n = carmen_city_sites_of_type(&c, "museum", museums, 2);
    TEST_ASSERT_EQUAL_INT(2, n);
}

/* --------------------------------------------------- carmen_city_random_clue */

static void test_city_random_clue_returns_null_when_no_sites(void)
{
    CarmenCity c = make_test_city();
    TEST_ASSERT_NULL(carmen_city_random_clue(&c));
}

static void test_city_random_clue_returns_null_when_sites_have_no_clues(void)
{
    CarmenCity c = make_test_city();
    CarmenSite s;
    carmen_site_init(&s, "Empty Site", "landmark");
    carmen_city_add_site(&c, &s);
    TEST_ASSERT_NULL(carmen_city_random_clue(&c));
}

static void test_city_random_clue_returns_valid_clue_with_target(void)
{
    srand(42);
    CarmenCity c = make_test_city();
    CarmenSite s1, s2;
    carmen_site_init(&s1, "Museum", "museum");
    carmen_site_add_clue(&s1, "Clue A", "cairo", CARMEN_CLUE_POSITIVE);
    carmen_site_add_clue(&s1, "Clue B", "london", CARMEN_CLUE_POSITIVE);
    carmen_site_init(&s2, "Airport", "airport");
    carmen_site_add_clue(&s2, "Clue C", "rome", CARMEN_CLUE_POSITIVE);
    carmen_city_add_site(&c, &s1);
    carmen_city_add_site(&c, &s2);

    for (int trial = 0; trial < 20; trial++) {
        const CarmenClue *clue = carmen_city_random_clue(&c);
        TEST_ASSERT_NOT_NULL(clue);
        int found = (strcmp(clue->text, "Clue A") == 0 && strcmp(clue->target_city_id, "cairo")  == 0) ||
                    (strcmp(clue->text, "Clue B") == 0 && strcmp(clue->target_city_id, "london") == 0) ||
                    (strcmp(clue->text, "Clue C") == 0 && strcmp(clue->target_city_id, "rome")   == 0);
        TEST_ASSERT_TRUE_MESSAGE(found, "Clue text/target mismatch");
    }
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_city_init_sets_all_fields);
    RUN_TEST(test_city_init_zeroes_arrays);
    RUN_TEST(test_city_add_site_increments_count);
    RUN_TEST(test_city_add_site_copies_data);
    RUN_TEST(test_city_add_site_respects_max);
    RUN_TEST(test_city_add_connection_increments_count);
    RUN_TEST(test_city_add_connection_respects_max);
    RUN_TEST(test_city_has_connection_to_found);
    RUN_TEST(test_city_has_connection_to_not_found);
    RUN_TEST(test_city_has_connection_to_among_multiple);
    RUN_TEST(test_city_sites_of_type_filters_correctly);
    RUN_TEST(test_city_sites_of_type_returns_zero_when_none);
    RUN_TEST(test_city_sites_of_type_respects_max_out);
    RUN_TEST(test_city_random_clue_returns_null_when_no_sites);
    RUN_TEST(test_city_random_clue_returns_null_when_sites_have_no_clues);
    RUN_TEST(test_city_random_clue_returns_valid_clue_with_target);
    return UNITY_END();
}
