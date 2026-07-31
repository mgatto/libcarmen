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
    carmen_site_init(&s, "louvre", "Louvre Museum", "museum");
    TEST_ASSERT_EQUAL_STRING("louvre", s.id);
    TEST_ASSERT_EQUAL_STRING("Louvre Museum", s.name);
    TEST_ASSERT_EQUAL_STRING("museum", s.site_type);
}

static void test_site_init_zeroes_struct(void)
{
    CarmenSite s;
    memset(&s, 0xFF, sizeof(s));
    carmen_site_init(&s, "eiffel", "Eiffel Tower", "landmark");
    TEST_ASSERT_EQUAL_STRING("eiffel", s.id);
    TEST_ASSERT_EQUAL_STRING("Eiffel Tower", s.name);
    TEST_ASSERT_EQUAL_STRING("landmark", s.site_type);
}

static void test_site_init_truncates_long_name(void)
{
    char long_name[128];
    memset(long_name, 'A', sizeof(long_name) - 1);
    long_name[sizeof(long_name) - 1] = '\0';

    CarmenSite s;
    carmen_site_init(&s, "long_name", long_name, "landmark");
    TEST_ASSERT_EQUAL_INT(CARMEN_MAX_NAME_LEN - 1, (int)strlen(s.name));
}

/* --------------------------------------------------------- carmen_site_to_string */

static void test_site_to_string_formats_name_and_type(void)
{
    CarmenSite s;
    carmen_site_init(&s, "louvre", "Louvre", "museum");
    char buf[64];
    int n = carmen_site_to_string(&s, buf, (int)sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("Louvre (museum)", buf);
    TEST_ASSERT_EQUAL_INT((int)strlen(buf), n);
}

/* ------------------------------------------------------------------- runner */

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_site_init_sets_fields);
    RUN_TEST(test_site_init_zeroes_struct);
    RUN_TEST(test_site_init_truncates_long_name);
    RUN_TEST(test_site_to_string_formats_name_and_type);
    return UNITY_END();
}
