#include "unity.h"
#include "carmen/carmen.h"
#include <stdlib.h>
#include <string.h>

static CarmenWorld *world;

void setUp(void)
{
    world = carmen_world_create();
    TEST_ASSERT_NOT_NULL(world);
    carmen_seed_build_islamic_world(world);
}

void tearDown(void)
{
    carmen_world_free(world);
    world = NULL;
}

/* Build valid default settings with a chosen difficulty. */
static CarmenCaseSettings mk(CarmenDifficulty d)
{
    CarmenCaseSettings s = carmen_case_settings_default();
    s.difficulty = d;
    return s;
}

/* -------------------------------------------------- generation basics */

static void test_generate_easy_succeeds(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

static void test_generate_medium_succeeds(void)
{
    srand(99);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_MEDIUM);
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

static void test_generate_hard_succeeds(void)
{
    srand(17);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_HARD);
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

/* -------------------------------------------------- trail properties */

static void test_easy_trail_length_is_3(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(3, c.trail_len);
}

static void test_medium_trail_length_is_5(void)
{
    srand(99);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_MEDIUM);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(5, c.trail_len);
}

static void test_hard_trail_length_is_7(void)
{
    srand(17);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_HARD);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(7, c.trail_len);
}

static void test_trail_cities_exist_in_world(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_MEDIUM);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);

    for (int i = 0; i < c.trail_len; i++) {
        CarmenCity *city = carmen_world_find(world, c.trail[i]);
        TEST_ASSERT_NOT_NULL_MESSAGE(city, c.trail[i]);
    }
}

static void test_trail_has_no_duplicate_cities(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_HARD);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);

    for (int i = 0; i < c.trail_len; i++)
        for (int j = i + 1; j < c.trail_len; j++)
            TEST_ASSERT_NOT_EQUAL_MESSAGE(
                0, strcmp(c.trail[i], c.trail[j]),
                "duplicate city in trail");
}

static void test_origin_is_first_trail_city(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_STRING(c.trail[0], c.origin_id);
}

static void test_hideout_is_last_trail_city(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_STRING(c.trail[c.trail_len - 1], c.hideout_id);
}

/* -------------------------------------------------- villain & artifact */

static void test_villain_is_set(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_NOT_NULL(c.villain);
    TEST_ASSERT_NOT_NULL(c.villain->id);
}

static void test_artifact_is_set(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_NOT_NULL(c.artifact.id);
    TEST_ASSERT_NOT_NULL(c.artifact.name);
}

/* The player must start at the city the artifact was stolen from. */
static void test_artifact_origin_matches_trail_origin(void)
{
    CarmenDifficulty diffs[] = {
        CARMEN_DIFFICULTY_EASY, CARMEN_DIFFICULTY_MEDIUM, CARMEN_DIFFICULTY_HARD
    };
    unsigned seeds[] = {42, 99, 17};
    for (int d = 0; d < 3; d++) {
        srand(seeds[d]);
        CarmenCaseSettings s = mk(diffs[d]);
        CarmenCase c;
        int ok = carmen_case_generate(&c, world, &s);
        TEST_ASSERT_EQUAL_INT(1, ok);
        TEST_ASSERT_EQUAL_STRING(c.artifact.origin_city_id, c.origin_id);
        TEST_ASSERT_EQUAL_STRING(c.artifact.origin_city_id, c.trail[0]);
    }
}

/* -------------------------------------------------- time budget */

static void test_time_budget_is_positive(void)
{
    srand(42);
    CarmenDifficulty diffs[] = {
        CARMEN_DIFFICULTY_EASY, CARMEN_DIFFICULTY_MEDIUM, CARMEN_DIFFICULTY_HARD
    };
    for (int d = 0; d < 3; d++) {
        CarmenCaseSettings s = mk(diffs[d]);
        CarmenCase c;
        carmen_case_generate(&c, world, &s);
        TEST_ASSERT_GREATER_THAN(0, c.time_budget_hrs);
    }
}

/* ---------------------------------------- trail stop clue assignment */

static void test_trail_stops_have_sites(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    if (!ok) { TEST_IGNORE_MESSAGE("generation failed"); return; }
    for (int i = 0; i < c.trail_len; i++)
        TEST_ASSERT_GREATER_THAN(0, c.stops[i].site_count);
}

static void test_non_hideout_stops_have_two_positives(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    if (!ok) { TEST_IGNORE_MESSAGE("generation failed"); return; }
    for (int i = 0; i < c.trail_len - 1; i++) {
        int pos = 0;
        for (int j = 0; j < c.stops[i].site_count && j < 2; j++) {
            if (c.stops[i].sites[j].clue.type == CARMEN_CLUE_POSITIVE &&
                strcmp(c.stops[i].sites[j].clue.target_city_id,
                       c.trail[i + 1]) == 0)
                pos++;
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(2, pos,
            "first 2 sites should have positive clues to next trail city");
    }
}

/* -------------------------------------------------- difficulty stored */

static void test_difficulty_stored_in_case(void)
{
    srand(42);
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_HARD);
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_HARD, c.difficulty);
}

/* -------------------------------------------------- null/edge cases */

static void test_generate_null_case_returns_0(void)
{
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    TEST_ASSERT_EQUAL_INT(0, carmen_case_generate(NULL, world, &s));
}

static void test_generate_null_world_returns_0(void)
{
    CarmenCaseSettings s = mk(CARMEN_DIFFICULTY_EASY);
    CarmenCase c;
    TEST_ASSERT_EQUAL_INT(0, carmen_case_generate(&c, NULL, &s));
}

static void test_generate_null_settings_returns_0(void)
{
    CarmenCase c;
    TEST_ASSERT_EQUAL_INT(0, carmen_case_generate(&c, world, NULL));
}

/* ----------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_generate_easy_succeeds);
    RUN_TEST(test_generate_medium_succeeds);
    RUN_TEST(test_generate_hard_succeeds);
    RUN_TEST(test_easy_trail_length_is_3);
    RUN_TEST(test_medium_trail_length_is_5);
    RUN_TEST(test_hard_trail_length_is_7);
    RUN_TEST(test_trail_cities_exist_in_world);
    RUN_TEST(test_trail_has_no_duplicate_cities);
    RUN_TEST(test_origin_is_first_trail_city);
    RUN_TEST(test_hideout_is_last_trail_city);
    RUN_TEST(test_villain_is_set);
    RUN_TEST(test_artifact_is_set);
    RUN_TEST(test_artifact_origin_matches_trail_origin);
    RUN_TEST(test_time_budget_is_positive);
    RUN_TEST(test_trail_stops_have_sites);
    RUN_TEST(test_non_hideout_stops_have_two_positives);
    RUN_TEST(test_difficulty_stored_in_case);
    RUN_TEST(test_generate_null_case_returns_0);
    RUN_TEST(test_generate_null_world_returns_0);
    RUN_TEST(test_generate_null_settings_returns_0);
    return UNITY_END();
}
