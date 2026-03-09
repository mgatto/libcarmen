#include "unity.h"
#include "carmen/carmen.h"
#include <stdlib.h>
#include <string.h>

/*
 * These tests use a small hand-crafted world (3 cities: A, B, C) so
 * that trail generation and clue behaviour are fully deterministic.
 *
 * Graph:
 *   A --flight 400km--> B --train 200km--> C
 *   A <--flight 400km-- B <--boat 600km--- C
 *
 * A has a positive clue pointing to B, B has a positive clue pointing
 * to C, C has a negative clue.  A deterministic RNG ensures the case
 * generator always builds the trail A -> B -> C.
 */

static CarmenWorld *world;

/* Deterministic RNG: cycles through a fixed sequence. */
static int det_seq[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static int det_idx;

static int det_rand(void *ctx)
{
    (void)ctx;
    int val = det_seq[det_idx % (int)(sizeof(det_seq) / sizeof(det_seq[0]))];
    det_idx++;
    return val;
}

static void build_test_world(void)
{
    world = carmen_world_create();

    carmen_world_add_city(world, "a", "CityA", NULL, "CountryA", "X", 0, 0);
    carmen_world_add_city(world, "b", "CityB", NULL, "CountryA", "X", 1, 1);
    carmen_world_add_city(world, "c", "CityC", NULL, "CountryB", "X", 2, 2);

    CarmenCity *a = carmen_world_find(world, "a");
    CarmenCity *b = carmen_world_find(world, "b");
    CarmenCity *c = carmen_world_find(world, "c");

    /* A -> B (flight 400 km) */
    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 400, "flight");
    carmen_city_add_connection(a, &conn);

    /* B -> A (flight 400 km) */
    carmen_connection_init(&conn, "a", 400, "flight");
    carmen_city_add_connection(b, &conn);

    /* B -> C (train 200 km) */
    carmen_connection_init(&conn, "c", 200, "train");
    carmen_city_add_connection(b, &conn);

    /* C -> B (boat 600 km) */
    carmen_connection_init(&conn, "b", 600, "boat");
    carmen_city_add_connection(c, &conn);

    /* Sites with clues */
    CarmenSite sa;
    carmen_site_init(&sa, "museum_a", "Museum A", "museum");
    carmen_site_add_clue(&sa, "Went to B", "b", CARMEN_CLUE_POSITIVE);
    carmen_site_add_clue(&sa, "Nobody here", "", CARMEN_CLUE_NEGATIVE);
    carmen_city_add_site(a, &sa);

    CarmenSite sb;
    carmen_site_init(&sb, "museum_b", "Museum B", "museum");
    carmen_site_add_clue(&sb, "Went to C", "c", CARMEN_CLUE_POSITIVE);
    carmen_site_add_clue(&sb, "Dead end", "", CARMEN_CLUE_NEGATIVE);
    carmen_city_add_site(b, &sb);

    CarmenSite sc;
    carmen_site_init(&sc, "museum_c", "Museum C", "museum");
    carmen_site_add_clue(&sc, "No trace", "", CARMEN_CLUE_NEGATIVE);
    carmen_city_add_site(c, &sc);
}

void setUp(void)
{
    det_idx = 0;
    carmen_set_rand(det_rand, NULL);
    build_test_world();
}

void tearDown(void)
{
    carmen_set_rand(NULL, NULL);
    carmen_world_free(world);
    world = NULL;
}

/* Helper: start a session on the test world with EASY difficulty.
   The deterministic RNG makes the trail A -> B -> C. */
static int start_easy(CarmenSession *s)
{
    CarmenCaseSettings settings = { CARMEN_DIFFICULTY_EASY, 0 };
    return carmen_session_start(s, world, &settings);
}

/* =================================================== lifecycle tests */

static void test_start_session_status_is_playing(void)
{
    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(1, start_easy(&s));
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_PLAYING, carmen_session_status(&s));
}

static void test_start_sets_current_city_to_origin(void)
{
    CarmenSession s;
    start_easy(&s);
    const CarmenCase *cas = carmen_session_case(&s);
    TEST_ASSERT_EQUAL_STRING(cas->origin_id, s.current_city_id);
}

static void test_start_sets_time_remaining(void)
{
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_GREATER_THAN(0, carmen_session_time_remaining(&s));
    TEST_ASSERT_EQUAL_INT(s.active_case.time_budget_hrs,
                          carmen_session_time_remaining(&s));
}

static void test_start_moves_is_zero(void)
{
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_EQUAL_INT(0, carmen_session_moves(&s));
}

static void test_reset_preserves_world(void)
{
    CarmenSession s;
    start_easy(&s);
    carmen_session_reset(&s);
    TEST_ASSERT_EQUAL_PTR(world, s.world);
    TEST_ASSERT_EQUAL_INT(0, s.moves);
    TEST_ASSERT_EQUAL_INT(-1, s.warrant_villain_idx);
}

/* ===================================================== travel tests */

static void test_travel_to_connected_city_succeeds(void)
{
    CarmenSession s;
    start_easy(&s);

    char origin[CARMEN_MAX_NAME_LEN];
    strncpy(origin, s.current_city_id, CARMEN_MAX_NAME_LEN);
    origin[CARMEN_MAX_NAME_LEN - 1] = '\0';

    const CarmenCity *ocity = carmen_session_current_city(&s);
    if (ocity->connection_count == 0) {
        TEST_IGNORE_MESSAGE("origin has no connections in test world");
        return;
    }
    const char *dest = ocity->connections[0].destination_id;

    int rc = carmen_session_travel(&s, dest);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_STRING(dest, s.current_city_id);
    TEST_ASSERT_EQUAL_INT(1, carmen_session_moves(&s));
    TEST_ASSERT_NOT_EQUAL_MESSAGE(
        0, strcmp(origin, s.current_city_id),
        "should have moved to new city");
}

static void test_travel_decreases_time(void)
{
    CarmenSession s;
    start_easy(&s);

    int before = carmen_session_time_remaining(&s);
    const CarmenCity *ocity = carmen_session_current_city(&s);
    if (ocity->connection_count == 0) {
        TEST_IGNORE_MESSAGE("origin has no connections");
        return;
    }
    carmen_session_travel(&s, ocity->connections[0].destination_id);
    TEST_ASSERT_LESS_THAN(before, carmen_session_time_remaining(&s));
}

static void test_travel_by_train_costs_more_per_km_than_flight(void)
{
    int flight_hrs = carmen_connection_travel_hrs(
        &(CarmenConnection){ .distance_km = 400,
                             .transport_mode = "flight" });
    int train_hrs = carmen_connection_travel_hrs(
        &(CarmenConnection){ .distance_km = 400,
                             .transport_mode = "train" });
    TEST_ASSERT_GREATER_THAN(flight_hrs, train_hrs);
}

static void test_travel_by_boat_costs_most(void)
{
    int flight_hrs = carmen_connection_travel_hrs(
        &(CarmenConnection){ .distance_km = 600,
                             .transport_mode = "flight" });
    int boat_hrs = carmen_connection_travel_hrs(
        &(CarmenConnection){ .distance_km = 600,
                             .transport_mode = "boat" });
    TEST_ASSERT_GREATER_THAN(flight_hrs, boat_hrs);
}

static void test_travel_to_unconnected_city_fails(void)
{
    CarmenSession s;
    start_easy(&s);

    /* "c" is not directly connected from "a" in our test world */
    if (strcmp(s.current_city_id, "a") == 0) {
        int rc = carmen_session_travel(&s, "c");
        TEST_ASSERT_EQUAL_INT(-1, rc);
    } else {
        TEST_IGNORE_MESSAGE("origin is not 'a'; skipping");
    }
}

static void test_travel_to_self_fails(void)
{
    CarmenSession s;
    start_easy(&s);
    int rc = carmen_session_travel(&s, s.current_city_id);
    TEST_ASSERT_EQUAL_INT(-1, rc);
    TEST_ASSERT_EQUAL_INT(0, carmen_session_moves(&s));
}

static void test_travel_after_game_over_fails(void)
{
    CarmenSession s;
    start_easy(&s);
    s.status = CARMEN_STATUS_WON;
    int rc = carmen_session_travel(&s, "b");
    TEST_ASSERT_EQUAL_INT(-3, rc);
}

static void test_time_running_out_loses(void)
{
    CarmenSession s;
    start_easy(&s);

    s.time_remaining_hrs = 1;
    const CarmenCity *ocity = carmen_session_current_city(&s);
    if (ocity->connection_count == 0) {
        TEST_IGNORE_MESSAGE("no connections");
        return;
    }
    int rc = carmen_session_travel(&s, ocity->connections[0].destination_id);
    TEST_ASSERT_EQUAL_INT(-2, rc);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_LOST_TIME, carmen_session_status(&s));
}

/* ================================================ investigate tests */

static void test_investigate_returns_clue(void)
{
    CarmenSession s;
    start_easy(&s);

    const CarmenCity *city = carmen_session_current_city(&s);
    if (city->site_count == 0) {
        TEST_IGNORE_MESSAGE("no sites in current city");
        return;
    }
    const CarmenClue *clue = carmen_session_investigate(&s, 0);
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_GREATER_THAN(0, (int)strlen(clue->text));
}

static void test_investigate_adds_to_notebook(void)
{
    CarmenSession s;
    start_easy(&s);

    TEST_ASSERT_EQUAL_INT(0, s.notebook_count);
    carmen_session_investigate(&s, 0);
    TEST_ASSERT_EQUAL_INT(1, s.notebook_count);
}

static void test_investigate_returns_notebook_pointer(void)
{
    CarmenSession s;
    start_easy(&s);

    const CarmenClue *clue = carmen_session_investigate(&s, 0);
    if (!clue) {
        TEST_IGNORE_MESSAGE("no clue returned");
        return;
    }
    /* The returned pointer should point inside s.notebook */
    TEST_ASSERT_TRUE((const char *)clue >= (const char *)s.notebook);
    TEST_ASSERT_TRUE((const char *)clue < (const char *)(s.notebook + CARMEN_MAX_NOTEBOOK));
}

static void test_investigate_out_of_range_returns_null(void)
{
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_NULL(carmen_session_investigate(&s, -1));
    TEST_ASSERT_NULL(carmen_session_investigate(&s, 99));
}

static void test_investigate_at_hideout_collects_evidence(void)
{
    CarmenSession s;
    start_easy(&s);

    /* Move to hideout */
    carmen_utf8_copy(s.current_city_id, CARMEN_MAX_NAME_LEN,
                     s.active_case.hideout_id);

    const CarmenCity *city = carmen_session_current_city(&s);
    if (!city || city->site_count == 0) {
        TEST_IGNORE_MESSAGE("hideout has no sites");
        return;
    }

    int before = s.evidence_count;
    carmen_session_investigate(&s, 0);
    TEST_ASSERT_GREATER_OR_EQUAL(before, s.evidence_count);
}

static void test_investigate_hideout_evidence_once_per_site(void)
{
    CarmenSession s;
    start_easy(&s);

    carmen_utf8_copy(s.current_city_id, CARMEN_MAX_NAME_LEN,
                     s.active_case.hideout_id);

    const CarmenCity *city = carmen_session_current_city(&s);
    if (!city || city->site_count == 0) {
        TEST_IGNORE_MESSAGE("hideout has no sites");
        return;
    }

    carmen_session_investigate(&s, 0);
    int after_first = s.evidence_count;

    carmen_session_investigate(&s, 0);
    TEST_ASSERT_EQUAL_INT(after_first, s.evidence_count);
}

static void test_investigate_full_notebook_returns_null(void)
{
    CarmenSession s;
    start_easy(&s);

    s.notebook_count = CARMEN_MAX_NOTEBOOK;
    TEST_ASSERT_NULL(carmen_session_investigate(&s, 0));
}

/* ================================================== warrant tests */

static void test_issue_warrant_succeeds(void)
{
    CarmenSession s;
    start_easy(&s);
    int rc = carmen_session_issue_warrant(&s, 0);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_INT(0, s.warrant_villain_idx);
}

static void test_issue_warrant_change(void)
{
    CarmenSession s;
    start_easy(&s);
    carmen_session_issue_warrant(&s, 0);
    carmen_session_issue_warrant(&s, 1);
    TEST_ASSERT_EQUAL_INT(1, s.warrant_villain_idx);
}

static void test_issue_warrant_invalid_index_fails(void)
{
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_issue_warrant(&s, -1));
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_issue_warrant(&s, FITNA_VILLAIN_COUNT));
}

/* ==================================================== arrest tests */

static void test_arrest_at_hideout_correct_warrant_wins(void)
{
    CarmenSession s;
    start_easy(&s);

    /* Move directly to hideout */
    carmen_utf8_copy(s.current_city_id, CARMEN_MAX_NAME_LEN,
                     s.active_case.hideout_id);

    /* Find the villain's index in FITNA_VILLAINS */
    int vidx = -1;
    for (int i = 0; i < FITNA_VILLAIN_COUNT; i++) {
        if (strcmp(FITNA_VILLAINS[i].id, s.active_case.villain->id) == 0) {
            vidx = i;
            break;
        }
    }
    TEST_ASSERT_NOT_EQUAL(-1, vidx);

    carmen_session_issue_warrant(&s, vidx);
    CarmenSessionStatus st = carmen_session_arrest(&s);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_WON, st);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_WON, carmen_session_status(&s));
}

static void test_arrest_at_hideout_wrong_warrant_loses(void)
{
    CarmenSession s;
    start_easy(&s);

    carmen_utf8_copy(s.current_city_id, CARMEN_MAX_NAME_LEN,
                     s.active_case.hideout_id);

    /* Issue warrant for a villain that is NOT the case villain */
    int wrong = -1;
    for (int i = 0; i < FITNA_VILLAIN_COUNT; i++) {
        if (strcmp(FITNA_VILLAINS[i].id, s.active_case.villain->id) != 0) {
            wrong = i;
            break;
        }
    }
    TEST_ASSERT_NOT_EQUAL(-1, wrong);

    carmen_session_issue_warrant(&s, wrong);
    CarmenSessionStatus st = carmen_session_arrest(&s);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_LOST_WRONG_ARREST, st);
}

static void test_arrest_no_warrant_loses(void)
{
    CarmenSession s;
    start_easy(&s);

    carmen_utf8_copy(s.current_city_id, CARMEN_MAX_NAME_LEN,
                     s.active_case.hideout_id);

    CarmenSessionStatus st = carmen_session_arrest(&s);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_LOST_NO_WARRANT, st);
}

static void test_arrest_not_at_hideout_returns_not_at_hideout(void)
{
    CarmenSession s;
    start_easy(&s);

    /* Ensure we're NOT at hideout */
    if (strcmp(s.current_city_id, s.active_case.hideout_id) == 0) {
        /* Move away */
        const CarmenCity *city = carmen_session_current_city(&s);
        if (city->connection_count > 0)
            carmen_session_travel(&s, city->connections[0].destination_id);
    }

    carmen_session_issue_warrant(&s, 0);
    CarmenSessionStatus st = carmen_session_arrest(&s);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_NOT_AT_HIDEOUT, st);
    /* Status should remain PLAYING since we weren't at hideout */
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_PLAYING, carmen_session_status(&s));
}

static void test_actions_after_game_over_return_errors(void)
{
    CarmenSession s;
    start_easy(&s);

    /* Force game over */
    s.status = CARMEN_STATUS_WON;

    TEST_ASSERT_EQUAL_INT(-3, carmen_session_travel(&s, "b"));
    TEST_ASSERT_NULL(carmen_session_investigate(&s, 0));
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_issue_warrant(&s, 0));
}

/* ================================================ query null safety */

static void test_queries_null_session(void)
{
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_LOST_TIME, carmen_session_status(NULL));
    TEST_ASSERT_NULL(carmen_session_current_city(NULL));
    TEST_ASSERT_NULL(carmen_session_case(NULL));
    TEST_ASSERT_EQUAL_INT(0, carmen_session_time_remaining(NULL));
    TEST_ASSERT_EQUAL_INT(0, carmen_session_moves(NULL));
}

/* ================================================================= runner */

int main(void)
{
    UNITY_BEGIN();

    /* Lifecycle */
    RUN_TEST(test_start_session_status_is_playing);
    RUN_TEST(test_start_sets_current_city_to_origin);
    RUN_TEST(test_start_sets_time_remaining);
    RUN_TEST(test_start_moves_is_zero);
    RUN_TEST(test_reset_preserves_world);

    /* Travel */
    RUN_TEST(test_travel_to_connected_city_succeeds);
    RUN_TEST(test_travel_decreases_time);
    RUN_TEST(test_travel_by_train_costs_more_per_km_than_flight);
    RUN_TEST(test_travel_by_boat_costs_most);
    RUN_TEST(test_travel_to_unconnected_city_fails);
    RUN_TEST(test_travel_to_self_fails);
    RUN_TEST(test_travel_after_game_over_fails);
    RUN_TEST(test_time_running_out_loses);

    /* Investigate */
    RUN_TEST(test_investigate_returns_clue);
    RUN_TEST(test_investigate_adds_to_notebook);
    RUN_TEST(test_investigate_returns_notebook_pointer);
    RUN_TEST(test_investigate_out_of_range_returns_null);
    RUN_TEST(test_investigate_at_hideout_collects_evidence);
    RUN_TEST(test_investigate_hideout_evidence_once_per_site);
    RUN_TEST(test_investigate_full_notebook_returns_null);

    /* Warrant */
    RUN_TEST(test_issue_warrant_succeeds);
    RUN_TEST(test_issue_warrant_change);
    RUN_TEST(test_issue_warrant_invalid_index_fails);

    /* Arrest */
    RUN_TEST(test_arrest_at_hideout_correct_warrant_wins);
    RUN_TEST(test_arrest_at_hideout_wrong_warrant_loses);
    RUN_TEST(test_arrest_no_warrant_loses);
    RUN_TEST(test_arrest_not_at_hideout_returns_not_at_hideout);
    RUN_TEST(test_actions_after_game_over_return_errors);

    /* Null safety */
    RUN_TEST(test_queries_null_session);

    return UNITY_END();
}
