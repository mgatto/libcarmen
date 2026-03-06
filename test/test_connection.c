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

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_connection_init_sets_fields);
    RUN_TEST(test_connection_init_null_mode_defaults_to_flight);
    RUN_TEST(test_connection_init_custom_transport);
    RUN_TEST(test_connection_init_zero_distance);
    RUN_TEST(test_connection_init_truncates_long_destination);
    return UNITY_END();
}
