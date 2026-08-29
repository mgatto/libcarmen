#include "unity.h"
#include "carmen/utf8.h"
#include <string.h>

void setUp(void)
{}
void tearDown(void)
{}

/* --- carmen_utf8_copy --- */

static void test_utf8_copy_basic(void)
{
    char   buf[32];
    size_t n = carmen_utf8_copy(buf, sizeof buf, "hello");
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    TEST_ASSERT_EQUAL_INT(5, (int)n);
}

static void test_utf8_copy_null_src(void)
{
    char buf[8] = "x";
    TEST_ASSERT_EQUAL_INT(0, (int)carmen_utf8_copy(buf, sizeof buf, NULL));
    TEST_ASSERT_EQUAL_STRING("", buf);
}

/* --- carmen_utf8_bidi_visual --- */

static void test_bidi_ascii_passthrough(void)
{
    char out[64];
    TEST_ASSERT_EQUAL_INT(8, (int)carmen_utf8_bidi_visual("Istanbul", out, sizeof out));
    TEST_ASSERT_EQUAL_STRING("Istanbul", out);
}

static void test_bidi_null_src(void)
{
    char out[8] = "x";
    TEST_ASSERT_EQUAL_INT(0, (int)carmen_utf8_bidi_visual(NULL, out, sizeof out));
    TEST_ASSERT_EQUAL_STRING("", out);
}

static void test_bidi_null_dst(void)
{
    TEST_ASSERT_EQUAL_INT(0, (int)carmen_utf8_bidi_visual("test", NULL, 64));
}

static void test_bidi_empty_src(void)
{
    char out[8] = "x";
    TEST_ASSERT_EQUAL_INT(0, (int)carmen_utf8_bidi_visual("", out, sizeof out));
    TEST_ASSERT_EQUAL_STRING("", out);
}

static void test_bidi_mixed_city_name_changes(void)
{
    const char *in = "Casablanca (\xD8\xA7\xD9\x84\xD8\xAF\xD8\xA7\xD8\xB1 "
                     "\xD8\xA7\xD9\x84\xD8\xA8\xD9\x8A\xD8\xB6\xD8\xA7\xD8\xA1)";
    char        out[128];
    size_t      n = carmen_utf8_bidi_visual(in, out, sizeof out);
    TEST_ASSERT_GREATER_THAN(0, (int)n);
    TEST_ASSERT_NOT_EQUAL(0, strcmp(in, out));
}

static void test_bidi_connection_line(void)
{
    const char *in = "    - Casablanca (\xD8\xA7\xD9\x84\xD8\xAF\xD8\xA7\xD8\xB1 "
                     "\xD8\xA7\xD9\x84\xD8\xA8\xD9\x8A\xD8\xB6\xD8\xA7\xD8\xA1) (flight, 3350 km)";
    char        out[256];
    size_t      n = carmen_utf8_bidi_visual(in, out, sizeof out);
    TEST_ASSERT_GREATER_THAN(0, (int)n);
    TEST_ASSERT_NOT_EQUAL(0, strcmp(in, out));
    TEST_ASSERT_NOT_NULL(strstr(out, "Casablanca"));
    TEST_ASSERT_NOT_NULL(strstr(out, "flight"));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_utf8_copy_basic);
    RUN_TEST(test_utf8_copy_null_src);
    RUN_TEST(test_bidi_ascii_passthrough);
    RUN_TEST(test_bidi_null_src);
    RUN_TEST(test_bidi_null_dst);
    RUN_TEST(test_bidi_empty_src);
    RUN_TEST(test_bidi_mixed_city_name_changes);
    RUN_TEST(test_bidi_connection_line);
    return UNITY_END();
}
