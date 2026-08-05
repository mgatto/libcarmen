#include "unity.h"
#include "carmen/carmen.h"
#include <string.h>

/* Golden-value checks for the built-in world produced by the build-time
   generator (carmen_world_build_islamic, generated from presets/islamic.jsonc).
   Values are spot-checked against the preset so the two can't silently drift. */

static CarmenWorld world;

void setUp(void)
{
    carmen_world_build_islamic(&world);
}

void tearDown(void)
{
    carmen_world_destroy(&world);
}

static const CarmenConnection *conn_to(const CarmenCity *c, const char *dest)
{
    for (int i = 0; i < c->connection_count; i++)
        if (strcmp(c->connections[i].destination_id, dest) == 0)
            return &c->connections[i];
    return NULL;
}

/* --- carmen_world_build_islamic --- */

static void test_build_islamic_city_count(void)
{
    TEST_ASSERT_EQUAL_INT(22, world.city_count);
}

static void test_build_islamic_null_is_safe(void)
{
    carmen_world_build_islamic(NULL); /* must not crash */
}

static void test_build_islamic_city_fields(void)
{
    CarmenCity *c = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_DOUBLE(41.01, c->latitude);
    TEST_ASSERT_EQUAL_DOUBLE(28.98, c->longitude);
    TEST_ASSERT_EQUAL_STRING("city.istanbul.name", c->name);
    TEST_ASSERT_EQUAL_STRING("city.istanbul.local_name", c->local_name);
    TEST_ASSERT_EQUAL_STRING("turkey_balkans", c->sphere);
}

static void test_build_islamic_sites(void)
{
    CarmenCity *c = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(4, c->site_count);
    /* Order is preserved 1:1 from the preset. */
    TEST_ASSERT_EQUAL_STRING("suleymaniye", c->sites[0].id);
    TEST_ASSERT_EQUAL_STRING("site.istanbul.suleymaniye", c->sites[0].name);
    TEST_ASSERT_EQUAL_STRING("type.mosque", c->sites[0].site_type);
}

static void test_build_islamic_inbound_clues(void)
{
    CarmenCity *c = carmen_world_find(&world, "istanbul");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(4, c->inbound_clue_count);
    TEST_ASSERT_EQUAL_STRING("clue.istanbul.inbound.0", c->inbound_clues[0]);
    TEST_ASSERT_EQUAL_STRING("clue.istanbul.inbound.3", c->inbound_clues[3]);
}

static void test_build_islamic_route_and_mirror(void)
{
    CarmenCity *ist = carmen_world_find(&world, "istanbul");
    CarmenCity *sar = carmen_world_find(&world, "sarajevo");
    TEST_ASSERT_NOT_NULL(ist);
    TEST_ASSERT_NOT_NULL(sar);

    const CarmenConnection *fwd = conn_to(ist, "sarajevo");
    TEST_ASSERT_NOT_NULL(fwd);
    TEST_ASSERT_EQUAL_INT(950, fwd->distance_km);
    TEST_ASSERT_EQUAL_STRING("flight", fwd->transport_mode);

    /* add_route mirrors the reverse edge with the same distance/mode. */
    const CarmenConnection *rev = conn_to(sar, "istanbul");
    TEST_ASSERT_NOT_NULL(rev);
    TEST_ASSERT_EQUAL_INT(950, rev->distance_km);
    TEST_ASSERT_EQUAL_STRING("flight", rev->transport_mode);
}

static void test_build_islamic_non_flight_mode(void)
{
    /* Dubai <-> Muscat is a 450 km boat hop in the preset. */
    CarmenCity *dubai = carmen_world_find(&world, "dubai");
    TEST_ASSERT_NOT_NULL(dubai);
    const CarmenConnection *c = conn_to(dubai, "muscat");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(450, c->distance_km);
    TEST_ASSERT_EQUAL_STRING("boat", c->transport_mode);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_build_islamic_city_count);
    RUN_TEST(test_build_islamic_null_is_safe);
    RUN_TEST(test_build_islamic_city_fields);
    RUN_TEST(test_build_islamic_sites);
    RUN_TEST(test_build_islamic_inbound_clues);
    RUN_TEST(test_build_islamic_route_and_mirror);
    RUN_TEST(test_build_islamic_non_flight_mode);
    return UNITY_END();
}
