#include "unity.h"
#include "carmen/connection.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* --------------------------------------------------------- carmen_connection_init */

static void test_connection_init_sets_fields(void)
{
    CarmenConnection c;
    carmen_connection_init(&c, "paris", 1100, "flight");
    TEST_ASSERT_EQUAL_STRING("paris", c.destination_id);
    TEST_ASSERT_EQUAL_INT(1100, c.distance_km);
    TEST_ASSERT_EQUAL_STRING("flight", c.transport_mode);
}

static void test_connection_init_null_mode_defaults_to_flight(void)
{
    CarmenConnection c;
    carmen_connection_init(&c, "tokyo", 9000, NULL);
    TEST_ASSERT_EQUAL_STRING("flight", c.transport_mode);
}

static void test_connection_init_custom_transport(void)
{
    CarmenConnection c;
    carmen_connection_init(&c, "london", 340, "train");
    TEST_ASSERT_EQUAL_STRING("train", c.transport_mode);
}

static void test_connection_init_zero_distance(void)
{
    CarmenConnection c;
    carmen_connection_init(&c, "same_city", 0, "walk");
    TEST_ASSERT_EQUAL_INT(0, c.distance_km);
    TEST_ASSERT_EQUAL_STRING("same_city", c.destination_id);
}

static void test_connection_init_truncates_long_destination(void)
{
    char long_id[128];
    memset(long_id, 'X', sizeof(long_id) - 1);
    long_id[sizeof(long_id) - 1] = '\0';

    CarmenConnection c;
    carmen_connection_init(&c, long_id, 500, "flight");
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_NAME_LEN - 1,
                          (int)strlen(c.destination_id));
}

/* --------------------------------------------------------- carmen_geo_distance_km */

static void test_geo_distance_same_point_is_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, carmen_geo_distance_km(41.01, 28.98, 41.01, 28.98));
}

static void test_geo_distance_paris_london(void)
{
    /* Great-circle Paris (48.8566, 2.3522) ↔ London (51.5074, -0.1278) is
       about 344 km; ±25 km covers rounding of the 6371 km radius model. */
    int km = carmen_geo_distance_km(48.8566, 2.3522, 51.5074, -0.1278);
    TEST_ASSERT_INT_WITHIN(25, 344, km);
}

static void test_geo_distance_new_york_london(void)
{
    int km = carmen_geo_distance_km(40.7128, -74.0060, 51.5074, -0.1278);
    TEST_ASSERT_INT_WITHIN(50, 5570, km);
}

static void test_geo_distance_is_symmetric(void)
{
    int ab = carmen_geo_distance_km(41.01, 28.98, 30.04, 31.24);
    int ba = carmen_geo_distance_km(30.04, 31.24, 41.01, 28.98);
    TEST_ASSERT_EQUAL_INT(ab, ba);
    TEST_ASSERT_GREATER_THAN(1000, ab);
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_connection_init_sets_fields);
    RUN_TEST(test_connection_init_null_mode_defaults_to_flight);
    RUN_TEST(test_connection_init_custom_transport);
    RUN_TEST(test_connection_init_zero_distance);
    RUN_TEST(test_connection_init_truncates_long_destination);
    RUN_TEST(test_geo_distance_same_point_is_zero);
    RUN_TEST(test_geo_distance_paris_london);
    RUN_TEST(test_geo_distance_new_york_london);
    RUN_TEST(test_geo_distance_is_symmetric);
    return UNITY_END();
}
