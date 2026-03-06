#include "unity.h"
#include "carmen/site.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ---------------------------------------------------------------- carmen_site_init */

static void test_site_init_sets_fields(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Louvre Museum", "museum");
    TEST_ASSERT_EQUAL_STRING("Louvre Museum", s.name);
    TEST_ASSERT_EQUAL_STRING("museum", s.site_type);
    TEST_ASSERT_EQUAL_INT(0, s.clue_count);
}

static void test_site_init_zeroes_clue_array(void)
{
    CarmenSite s;
    memset(&s, 0xFF, sizeof(s));
    carmen_site_init(&s, "Eiffel Tower", "landmark");
    TEST_ASSERT_EQUAL_INT(0, s.clue_count);
    TEST_ASSERT_EQUAL_UINT8(0, s.clues[0].text[0]);
}

static void test_site_init_truncates_long_name(void)
{
    char long_name[128];
    memset(long_name, 'A', sizeof(long_name) - 1);
    long_name[sizeof(long_name) - 1] = '\0';

    CarmenSite s;
    carmen_site_init(&s, long_name, "landmark");
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_NAME_LEN - 1, (int)strlen(s.name));
}

/* ------------------------------------------------------------- carmen_site_add_clue */

static void test_site_add_clue_increments_count(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Test", "landmark");
    carmen_site_add_clue(&s, "Clue one", "cairo", CARMEN_CLUE_POSITIVE);
    TEST_ASSERT_EQUAL_INT(1, s.clue_count);
    TEST_ASSERT_EQUAL_STRING("Clue one", s.clues[0].text);
    TEST_ASSERT_EQUAL_STRING("cairo", s.clues[0].target_city_id);
}

static void test_site_add_clue_null_target(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Test", "landmark");
    carmen_site_add_clue(&s, "Red herring clue", NULL, CARMEN_CLUE_POSITIVE);
    TEST_ASSERT_EQUAL_INT(1, s.clue_count);
    TEST_ASSERT_EQUAL_STRING("Red herring clue", s.clues[0].text);
    TEST_ASSERT_EQUAL_STRING("", s.clues[0].target_city_id);
}

static void test_site_add_multiple_clues(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Test", "museum");
    for (int i = 0; i < 4; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Clue %d", i);
        carmen_site_add_clue(&s, buf, "target", CARMEN_CLUE_POSITIVE);
    }
    TEST_ASSERT_EQUAL_INT(4, s.clue_count);
    TEST_ASSERT_EQUAL_STRING("Clue 0", s.clues[0].text);
    TEST_ASSERT_EQUAL_STRING("Clue 3", s.clues[3].text);
}

static void test_site_add_clue_respects_max(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Test", "landmark");
    for (int i = 0; i < CARMEN_MAX_CLUES + 5; i++)
        carmen_site_add_clue(&s, "overflow", "x", CARMEN_CLUE_POSITIVE);
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_CLUES, s.clue_count);
}

static void test_site_add_clue_truncates_long_clue(void)
{
    char long_clue[512];
    memset(long_clue, 'B', sizeof(long_clue) - 1);
    long_clue[sizeof(long_clue) - 1] = '\0';

    CarmenSite s;
    carmen_site_init(&s, "Test", "landmark");
    carmen_site_add_clue(&s, long_clue, "target", CARMEN_CLUE_POSITIVE);
    TEST_ASSERT_EQUAL_INT(1, s.clue_count);
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_CLUE_LEN - 1,
                          (int)strlen(s.clues[0].text));
}

/* --------------------------------------------------------- carmen_site_random_clue */

static void test_site_random_clue_returns_null_when_empty(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Empty", "landmark");
    TEST_ASSERT_NULL(carmen_site_random_clue(&s));
}

static void test_site_random_clue_returns_valid_clue(void)
{
    CarmenSite s;
    carmen_site_init(&s, "Test", "landmark");
    carmen_site_add_clue(&s, "Only clue", "paris", CARMEN_CLUE_POSITIVE);
    const CarmenClue *clue = carmen_site_random_clue(&s);
    TEST_ASSERT_NOT_NULL(clue);
    TEST_ASSERT_EQUAL_STRING("Only clue", clue->text);
    TEST_ASSERT_EQUAL_STRING("paris", clue->target_city_id);
}

static void test_site_random_clue_is_among_added_clues(void)
{
    srand(42);
    CarmenSite s;
    carmen_site_init(&s, "Test", "museum");
    carmen_site_add_clue(&s, "Alpha", "a", CARMEN_CLUE_POSITIVE);
    carmen_site_add_clue(&s, "Beta", "b", CARMEN_CLUE_POSITIVE);
    carmen_site_add_clue(&s, "Gamma", "c", CARMEN_CLUE_POSITIVE);

    for (int trial = 0; trial < 20; trial++) {
        const CarmenClue *clue = carmen_site_random_clue(&s);
        TEST_ASSERT_NOT_NULL(clue);
        int found = strcmp(clue->text, "Alpha") == 0 ||
                    strcmp(clue->text, "Beta")  == 0 ||
                    strcmp(clue->text, "Gamma") == 0;
        TEST_ASSERT_TRUE_MESSAGE(found, "Clue not among added clues");
    }
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_site_init_sets_fields);
    RUN_TEST(test_site_init_zeroes_clue_array);
    RUN_TEST(test_site_init_truncates_long_name);
    RUN_TEST(test_site_add_clue_increments_count);
    RUN_TEST(test_site_add_clue_null_target);
    RUN_TEST(test_site_add_multiple_clues);
    RUN_TEST(test_site_add_clue_respects_max);
    RUN_TEST(test_site_add_clue_truncates_long_clue);
    RUN_TEST(test_site_random_clue_returns_null_when_empty);
    RUN_TEST(test_site_random_clue_returns_valid_clue);
    RUN_TEST(test_site_random_clue_is_among_added_clues);
    return UNITY_END();
}
