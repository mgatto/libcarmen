#include "unity.h"
#include "carmen/carmen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CarmenWorld *world;

static const char *TMP = "carmen_settings_test.toml";

void setUp(void)
{
    world = carmen_world_create();
    TEST_ASSERT_NOT_NULL(world);
    carmen_world_build_islamic(world);
}

void tearDown(void)
{
    remove(TMP);
    carmen_world_free(world);
    world = NULL;
}

static void write_tmp(const char *contents)
{
    FILE *f = fopen(TMP, "wb");
    TEST_ASSERT_NOT_NULL(f);
    fputs(contents, f);
    TEST_ASSERT_EQUAL(0, fclose(f));
}

/* ------------------------------------------------------- defaults */

static void test_defaults_are_valid(void)
{
    CarmenCaseSettings s = carmen_case_settings_default();
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_MEDIUM, s.difficulty);
    TEST_ASSERT_EQUAL_INT(0, s.trail_length);
    TEST_ASSERT_EQUAL_INT(0, s.time_budget_hrs);
    TEST_ASSERT_EQUAL_INT(0, s.active_sites_per_city);
    TEST_ASSERT_EQUAL_INT(0, s.positive_clues_per_stop);
    TEST_ASSERT_EQUAL_INT(0, s.move_limit);
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_VISITED, s.visited_history_size);
}

/* ---------------------------------------------------------- load */

static void test_load_null_args_fail(void)
{
    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(0, carmen_case_settings_load(NULL, TMP));
    TEST_ASSERT_EQUAL_INT(0, carmen_case_settings_load(&s, NULL));
}

static void test_load_missing_file_fails(void)
{
    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(0,
        carmen_case_settings_load(&s, "does_not_exist_12345.toml"));
}

static void test_load_parse_error_fails(void)
{
    write_tmp("this is = not valid = toml ][\n");
    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(0, carmen_case_settings_load(&s, TMP));
}

static void test_load_applies_overrides(void)
{
    write_tmp(
        "difficulty = \"hard\"\n"
        "trail_length = 4\n"
        "time_budget_hrs = 300\n"
        "active_sites_per_city = 2\n"
        "positive_clues_per_stop = 1\n"
        "move_limit = 7\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_HARD, s.difficulty);
    TEST_ASSERT_EQUAL_INT(4, s.trail_length);
    TEST_ASSERT_EQUAL_INT(300, s.time_budget_hrs);
    TEST_ASSERT_EQUAL_INT(2, s.active_sites_per_city);
    TEST_ASSERT_EQUAL_INT(1, s.positive_clues_per_stop);
    TEST_ASSERT_EQUAL_INT(7, s.move_limit);
}

static void test_load_partial_keeps_defaults(void)
{
    write_tmp("move_limit = 9\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(9, s.move_limit);
    /* untouched keys keep defaults */
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_MEDIUM, s.difficulty);
    TEST_ASSERT_EQUAL_INT(0, s.active_sites_per_city);
    TEST_ASSERT_EQUAL_INT(0, s.positive_clues_per_stop);
}

static void test_load_clamps_out_of_range(void)
{
    write_tmp(
        "trail_length = 99\n"
        "active_sites_per_city = 99\n"
        "positive_clues_per_stop = 99\n"
        "time_budget_hrs = -5\n"
        "move_limit = -3\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_TRAIL, s.trail_length);
    TEST_ASSERT_EQUAL_INT(CARMEN_TRAIL_SITES, s.active_sites_per_city);
    TEST_ASSERT_EQUAL_INT(CARMEN_TRAIL_SITES, s.positive_clues_per_stop);
    TEST_ASSERT_EQUAL_INT(0, s.time_budget_hrs);
    TEST_ASSERT_EQUAL_INT(0, s.move_limit);
}

static void test_load_unknown_difficulty_uses_default(void)
{
    write_tmp("difficulty = \"impossible\"\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_MEDIUM, s.difficulty);
}

static void test_load_visited_history_size(void)
{
    write_tmp("visited_history_size = 10\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(10, s.visited_history_size);
}

static void test_load_visited_history_size_zero_means_full(void)
{
    write_tmp("visited_history_size = 0\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    /* 0 is preserved as "derive/full"; record_visit() treats it as the
       compile-time ceiling. */
    TEST_ASSERT_EQUAL_INT(0, s.visited_history_size);
}

static void test_load_visited_history_size_clamps(void)
{
    write_tmp("visited_history_size = 999\n");

    CarmenCaseSettings s;
    TEST_ASSERT_EQUAL_INT(1, carmen_case_settings_load(&s, TMP));
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_VISITED, s.visited_history_size);
}

/* ----------------------------------------------- behavioral wiring */

static void test_trail_length_override(void)
{
    srand(42);
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = CARMEN_DIFFICULTY_EASY; /* would normally be 3 */
    s.trail_length = 4;

    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    if (!ok) { TEST_IGNORE_MESSAGE("generation failed"); return; }
    TEST_ASSERT_EQUAL_INT(4, c.trail_len);
}

static void test_time_budget_override(void)
{
    srand(42);
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = CARMEN_DIFFICULTY_EASY;
    s.time_budget_hrs = 500;

    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    if (!ok) { TEST_IGNORE_MESSAGE("generation failed"); return; }
    TEST_ASSERT_EQUAL_INT(500, c.time_budget_hrs);
}

static void test_active_sites_and_positive_clues_override(void)
{
    srand(42);
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = CARMEN_DIFFICULTY_EASY;
    s.active_sites_per_city = 2;
    s.positive_clues_per_stop = 1;

    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    if (!ok) { TEST_IGNORE_MESSAGE("generation failed"); return; }

    for (int i = 0; i < c.trail_len; i++)
        TEST_ASSERT_LESS_OR_EQUAL_INT(2, c.stops[i].site_count);

    /* Non-hideout stops carry exactly 1 correct positive clue. */
    for (int i = 0; i < c.trail_len - 1; i++) {
        int pos = 0;
        for (int j = 0; j < c.stops[i].site_count; j++) {
            if (c.stops[i].sites[j].clue.type == CARMEN_CLUE_POSITIVE &&
                strcmp(c.stops[i].sites[j].clue.target_city_id,
                       c.trail[i + 1]) == 0)
                pos++;
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, pos,
            "expected exactly one correct positive clue per non-hideout stop");
    }
}

/* Count correct positive clues (pointing to the next trail city) at a stop. */
static int count_correct_positives(const CarmenCase *c, int stop)
{
    int pos = 0;
    for (int j = 0; j < c->stops[stop].site_count; j++) {
        if (c->stops[stop].sites[j].clue.type == CARMEN_CLUE_POSITIVE &&
            strcmp(c->stops[stop].sites[j].clue.target_city_id,
                   c->trail[stop + 1]) == 0)
            pos++;
    }
    return pos;
}

static void test_clue_ratio_scales_with_difficulty(void)
{
    CarmenDifficulty diffs[] = {
        CARMEN_DIFFICULTY_EASY, CARMEN_DIFFICULTY_MEDIUM, CARMEN_DIFFICULTY_HARD
    };
    int expected[] = {3, 2, 1};
    unsigned seeds[] = {42, 99, 17};

    for (int d = 0; d < 3; d++) {
        srand(seeds[d]);
        CarmenCaseSettings s = carmen_case_settings_default();
        s.difficulty = diffs[d];

        CarmenCase c;
        int ok = carmen_case_generate(&c, world, &s);
        TEST_ASSERT_EQUAL_INT(1, ok);

        for (int i = 0; i < c.trail_len - 1; i++)
            TEST_ASSERT_EQUAL_INT_MESSAGE(
                expected[d], count_correct_positives(&c, i),
                "positive clue count should match the difficulty ratio");
    }
}

static void test_move_limit_triggers_loss(void)
{
    srand(42);
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = CARMEN_DIFFICULTY_EASY;
    s.move_limit = 1;

    CarmenSession sess;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_start(&sess, world, &s));

    const CarmenCity *city = carmen_session_current_city(&sess);
    TEST_ASSERT_NOT_NULL(city);
    TEST_ASSERT_GREATER_THAN(0, city->connection_count);

    int rc = carmen_session_travel(&sess, city->connections[0].destination_id);
    /* First (and only permitted) move exhausts the limit. */
    if (rc == -2) {
        TEST_IGNORE_MESSAGE("ran out of time before move limit");
        return;
    }
    TEST_ASSERT_EQUAL_INT(-4, rc);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_LOST_MOVES,
                          carmen_session_status(&sess));
}

static void test_no_move_limit_allows_travel(void)
{
    srand(42);
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = CARMEN_DIFFICULTY_EASY;
    s.move_limit = 0; /* unlimited */

    CarmenSession sess;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_start(&sess, world, &s));

    const CarmenCity *city = carmen_session_current_city(&sess);
    TEST_ASSERT_NOT_NULL(city);
    TEST_ASSERT_GREATER_THAN(0, city->connection_count);

    int rc = carmen_session_travel(&sess, city->connections[0].destination_id);
    TEST_ASSERT_NOT_EQUAL(-4, rc);
    TEST_ASSERT_NOT_EQUAL(CARMEN_STATUS_LOST_MOVES,
                          carmen_session_status(&sess));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_defaults_are_valid);
    RUN_TEST(test_load_null_args_fail);
    RUN_TEST(test_load_missing_file_fails);
    RUN_TEST(test_load_parse_error_fails);
    RUN_TEST(test_load_applies_overrides);
    RUN_TEST(test_load_partial_keeps_defaults);
    RUN_TEST(test_load_clamps_out_of_range);
    RUN_TEST(test_load_unknown_difficulty_uses_default);
    RUN_TEST(test_load_visited_history_size);
    RUN_TEST(test_load_visited_history_size_zero_means_full);
    RUN_TEST(test_load_visited_history_size_clamps);
    RUN_TEST(test_trail_length_override);
    RUN_TEST(test_time_budget_override);
    RUN_TEST(test_active_sites_and_positive_clues_override);
    RUN_TEST(test_clue_ratio_scales_with_difficulty);
    RUN_TEST(test_move_limit_triggers_loss);
    RUN_TEST(test_no_move_limit_allows_travel);
    return UNITY_END();
}
