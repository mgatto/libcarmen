#include "unity.h"
#include "carmen/carmen.h"
#include <stdlib.h>
#include <string.h>

static CarmenWorld *world;

void setUp(void)
{
    srand(42);
    world = carmen_world_create();
    TEST_ASSERT_NOT_NULL(world);
    carmen_world_build_islamic(world);
}

void tearDown(void)
{
    carmen_world_free(world);
    world = NULL;
}

static int graph_is_connected(CarmenWorld *w)
{
    if (w->city_count <= 1) return 1;

    int visited[CARMEN_MAX_CITIES];
    int queue[CARMEN_MAX_CITIES];
    int i;
    for (i = 0; i < w->city_count; i++)
        visited[i] = 0;

    int qh = 0, qt = 0, seen = 0;
    visited[0] = 1;
    queue[qt++] = 0;
    seen = 1;

    while (qh < qt) {
        int slot = queue[qh++];
        CarmenCity *c = &w->storage[slot];
        for (i = 0; i < c->connection_count; i++) {
            CarmenCity *nb = carmen_world_find(w, c->connections[i].destination_id);
            TEST_ASSERT_NOT_NULL(nb);
            int ns = (int)(nb - w->storage);
            if (!visited[ns]) {
                visited[ns] = 1;
                queue[qt++] = ns;
                seen++;
            }
        }
    }
    return seen == w->city_count;
}

static void snapshot_neighbors(const CarmenWorld *w,
                               char out[][CARMEN_MAX_CONNECTIONS][CARMEN_MAX_NAME_LEN])
{
    for (int i = 0; i < w->city_count; i++) {
        const CarmenCity *c = &w->storage[i];
        for (int k = 0; k < CARMEN_MAX_CONNECTIONS; k++)
            out[i][k][0] = '\0';
        for (int k = 0; k < c->connection_count; k++)
            carmen_utf8_copy(out[i][k], CARMEN_MAX_NAME_LEN,
                             c->connections[k].destination_id);
    }
}

/* --- carmen_world_generate_connections --- */

static void test_generate_null_is_safe(void)
{
    carmen_world_generate_connections(NULL); /* must not crash */
}

static void test_generate_every_city_degree_is_3(void)
{
    carmen_world_generate_connections(world);
    TEST_ASSERT_EQUAL_INT(22, world->city_count);
    for (int i = 0; i < world->city_count; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(3, world->storage[i].connection_count,
                                      world->storage[i].id);
    }
}

static void test_generate_undirected_no_loops_no_dups(void)
{
    carmen_world_generate_connections(world);
    int undirected = 0;
    for (int i = 0; i < world->city_count; i++) {
        CarmenCity *a = &world->storage[i];
        for (int k = 0; k < a->connection_count; k++) {
            const char *dest = a->connections[k].destination_id;
            TEST_ASSERT_NOT_EQUAL_MESSAGE(0, strcmp(a->id, dest),
                                          "self-loop");
            CarmenCity *b = carmen_world_find(world, dest);
            TEST_ASSERT_NOT_NULL(b);
            TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(b, a->id));
            if (strcmp(a->id, dest) < 0)
                undirected++;
            for (int m = k + 1; m < a->connection_count; m++)
                TEST_ASSERT_NOT_EQUAL(0, strcmp(dest,
                    a->connections[m].destination_id));
        }
    }
    TEST_ASSERT_EQUAL_INT(33, undirected);
}

static void test_generate_is_connected(void)
{
    carmen_world_generate_connections(world);
    TEST_ASSERT_EQUAL_INT(1, graph_is_connected(world));
}

static void test_generate_every_edge_is_flight_haversine(void)
{
    carmen_world_generate_connections(world);
    for (int i = 0; i < world->city_count; i++) {
        CarmenCity *a = &world->storage[i];
        for (int k = 0; k < a->connection_count; k++) {
            const CarmenConnection *conn = &a->connections[k];
            TEST_ASSERT_EQUAL_STRING("flight", conn->transport_mode);
            CarmenCity *b = carmen_world_find(world, conn->destination_id);
            TEST_ASSERT_NOT_NULL(b);
            int km = carmen_geo_distance_km(a->latitude, a->longitude,
                                            b->latitude, b->longitude);
            TEST_ASSERT_EQUAL_INT(km, conn->distance_km);
        }
    }
}

static void test_two_successive_generations_differ(void)
{
    srand(7);
    carmen_world_generate_connections(world);
    char first[CARMEN_MAX_CITIES][CARMEN_MAX_CONNECTIONS][CARMEN_MAX_NAME_LEN];
    snapshot_neighbors(world, first);

    carmen_world_generate_connections(world);
    int differ = 0;
    for (int i = 0; i < world->city_count && !differ; i++) {
        for (int k = 0; k < 3; k++) {
            if (strcmp(first[i][k], world->storage[i].connections[k].destination_id)
                != 0) {
                differ = 1;
                break;
            }
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(differ, "two generations produced the same graph");
}

static void test_clear_connections_empties_every_city(void)
{
    carmen_world_generate_connections(world);
    carmen_world_clear_connections(world);
    for (int i = 0; i < world->city_count; i++)
        TEST_ASSERT_EQUAL_INT(0, world->storage[i].connection_count);
}

static void test_clear_connections_null_is_safe(void)
{
    carmen_world_clear_connections(NULL); /* must not crash */
}

static void test_generate_noop_on_odd_city_count(void)
{
    CarmenWorld *tiny = carmen_world_create();
    TEST_ASSERT_NOT_NULL(tiny);
    carmen_world_add_city(tiny, "a", "A", NULL, "X", "Y", 0, 0);
    carmen_world_add_city(tiny, "b", "B", NULL, "X", "Y", 1, 1);
    carmen_world_add_city(tiny, "c", "C", NULL, "X", "Y", 2, 2);
    CarmenCity *a = carmen_world_find(tiny, "a");
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 100, "train");
    carmen_city_add_connection(a, &conn);

    carmen_world_generate_connections(tiny);
    TEST_ASSERT_EQUAL_INT(1, a->connection_count);
    TEST_ASSERT_EQUAL_STRING("b", a->connections[0].destination_id);
    TEST_ASSERT_EQUAL_STRING("train", a->connections[0].transport_mode);
    carmen_world_free(tiny);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_generate_null_is_safe);
    RUN_TEST(test_generate_every_city_degree_is_3);
    RUN_TEST(test_generate_undirected_no_loops_no_dups);
    RUN_TEST(test_generate_is_connected);
    RUN_TEST(test_generate_every_edge_is_flight_haversine);
    RUN_TEST(test_two_successive_generations_differ);
    RUN_TEST(test_clear_connections_empties_every_city);
    RUN_TEST(test_clear_connections_null_is_safe);
    RUN_TEST(test_generate_noop_on_odd_city_count);
    return UNITY_END();
}
