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

/* -------------------------------------------------- generation basics */

static void test_generate_easy_succeeds(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

static void test_generate_medium_succeeds(void)
{
    srand(99);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_MEDIUM, 0 };
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

static void test_generate_hard_succeeds(void)
{
    srand(17);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_HARD, 0 };
    CarmenCase c;
    int ok = carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, ok);
}

/* -------------------------------------------------- trail properties */

static void test_easy_trail_length_is_3(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(3, c.trail_len);
}

static void test_medium_trail_length_is_5(void)
{
    srand(99);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_MEDIUM, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(5, c.trail_len);
}

static void test_hard_trail_length_is_7(void)
{
    srand(17);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_HARD, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(7, c.trail_len);
}

static void test_trail_cities_exist_in_world(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_MEDIUM, 0 };
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
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_HARD, 0 };
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
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_STRING(c.trail[0], c.origin_id);
}

static void test_hideout_is_last_trail_city(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_STRING(c.trail[c.trail_len - 1], c.hideout_id);
}

/* -------------------------------------------------- villain & artifact */

static void test_villain_is_set(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_NOT_NULL(c.villain);
    TEST_ASSERT_NOT_NULL(c.villain->id);
}

static void test_artifact_is_set(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_NOT_NULL(c.artifact.id);
    TEST_ASSERT_NOT_NULL(c.artifact.name);
}

/* -------------------------------------------------- time budget */

static void test_time_budget_is_positive(void)
{
    srand(42);
    CarmenDifficulty diffs[] = {
        CARMEN_DIFFICULTY_EASY, CARMEN_DIFFICULTY_MEDIUM, CARMEN_DIFFICULTY_HARD
    };
    for (int d = 0; d < 3; d++) {
        CarmenCaseSettings s = { diffs[d], 0 };
        CarmenCase c;
        carmen_case_generate(&c, world, &s);
        TEST_ASSERT_GREATER_THAN(0, c.time_budget_hrs);
    }
}

/* -------------------------------------------------- max_trail_hops */

static void test_default_hops_easy_is_1(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(1, c.max_trail_hops);
}

static void test_default_hops_medium_is_2(void)
{
    srand(99);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_MEDIUM, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(2, c.max_trail_hops);
}

static void test_default_hops_hard_is_3(void)
{
    srand(17);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_HARD, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(3, c.max_trail_hops);
}

static void test_explicit_hops_override(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 3 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(3, c.max_trail_hops);
}

/* -------------------------------------------------- difficulty stored */

static void test_difficulty_stored_in_case(void)
{
    srand(42);
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_HARD, 0 };
    CarmenCase c;
    carmen_case_generate(&c, world, &s);
    TEST_ASSERT_EQUAL_INT(CARMEN_DIFFICULTY_HARD, c.difficulty);
}

/* -------------------------------------------------- null/edge cases */

static void test_generate_null_case_returns_0(void)
{
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
    TEST_ASSERT_EQUAL_INT(0, carmen_case_generate(NULL, world, &s));
}

static void test_generate_null_world_returns_0(void)
{
    CarmenCaseSettings s = { CARMEN_DIFFICULTY_EASY, 0 };
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
    RUN_TEST(test_time_budget_is_positive);
    RUN_TEST(test_default_hops_easy_is_1);
    RUN_TEST(test_default_hops_medium_is_2);
    RUN_TEST(test_default_hops_hard_is_3);
    RUN_TEST(test_explicit_hops_override);
    RUN_TEST(test_difficulty_stored_in_case);
    RUN_TEST(test_generate_null_case_returns_0);
    RUN_TEST(test_generate_null_world_returns_0);
    RUN_TEST(test_generate_null_settings_returns_0);
    return UNITY_END();
}
