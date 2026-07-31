#include "unity.h"
#include "carmen/villain.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ------------------------------------------------ catalog accessors */

static void test_count_matches_constant(void)
{
    TEST_ASSERT_EQUAL_INT(FITNA_VILLAIN_COUNT, carmen_villain_count());
}

static void test_at_returns_matching_entry(void)
{
    for (int i = 0; i < carmen_villain_count(); i++) {
        const FitnaVillain *v = carmen_villain_at(i);
        TEST_ASSERT_NOT_NULL(v);
        TEST_ASSERT_EQUAL_PTR(&FITNA_VILLAINS[i], v);
    }
}

static void test_at_out_of_range_returns_null(void)
{
    TEST_ASSERT_NULL(carmen_villain_at(-1));
    TEST_ASSERT_NULL(carmen_villain_at(carmen_villain_count()));
    TEST_ASSERT_NULL(carmen_villain_at(FITNA_VILLAIN_COUNT + 100));
}

static void test_all_fields_populated(void)
{
    for (int i = 0; i < carmen_villain_count(); i++) {
        const FitnaVillain *v = carmen_villain_at(i);
        TEST_ASSERT_NOT_NULL(v->id);
        TEST_ASSERT_NOT_NULL(v->name);
        TEST_ASSERT_NOT_NULL(v->alias);
        TEST_ASSERT_GREATER_THAN(0, (int)strlen(v->id));
        TEST_ASSERT_TRUE(v->gender == 'M' || v->gender == 'F');
        for (int j = 0; j < FITNA_MAX_ID_CLUES; j++)
            TEST_ASSERT_NOT_NULL(v->id_clues[j]);
    }
}

static void test_no_duplicate_ids(void)
{
    int n = carmen_villain_count();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            TEST_ASSERT_NOT_EQUAL_MESSAGE(
                0,
                strcmp(carmen_villain_at(i)->id, carmen_villain_at(j)->id),
                "duplicate villain id found");
}

/* --------------------------------------------- pronoun expansion */

static void test_expand_clue_male(void)
{
    char out[128];
    carmen_villain_expand_clue("{They} left {their} bag with {them}.", 'M',
                               out, sizeof out);
    TEST_ASSERT_EQUAL_STRING("He left his bag with him.", out);
}

static void test_expand_clue_female(void)
{
    char out[128];
    carmen_villain_expand_clue("{They} left {their} bag with {them}.", 'F',
                               out, sizeof out);
    TEST_ASSERT_EQUAL_STRING("She left her bag with her.", out);
}

static void test_expand_clue_contractions(void)
{
    char out[128];
    carmen_villain_expand_clue("{they}'s gone; {they}'d fled.", 'M',
                               out, sizeof out);
    TEST_ASSERT_EQUAL_STRING("he's gone; he'd fled.", out);
}

static void test_expand_clue_no_tokens_is_copy(void)
{
    char out[64];
    carmen_villain_expand_clue("plain text", 'F', out, sizeof out);
    TEST_ASSERT_EQUAL_STRING("plain text", out);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_count_matches_constant);
    RUN_TEST(test_at_returns_matching_entry);
    RUN_TEST(test_at_out_of_range_returns_null);
    RUN_TEST(test_all_fields_populated);
    RUN_TEST(test_no_duplicate_ids);
    RUN_TEST(test_expand_clue_male);
    RUN_TEST(test_expand_clue_female);
    RUN_TEST(test_expand_clue_contractions);
    RUN_TEST(test_expand_clue_no_tokens_is_copy);
    return UNITY_END();
}
