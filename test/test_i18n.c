#include "unity.h"
#include "carmen/carmen.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* --- carmen_i18n_load_json --- */

static void test_load_json_reads_keys(void)
{
    const char *json = "{ \"ui.title\": \"Hello\", \"ui.sub\": \"World\" }";
    CarmenI18n *ctx = carmen_i18n_load_json(json, strlen(json));
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Hello", carmen_i18n_get(ctx, "ui.title"));
    TEST_ASSERT_EQUAL_STRING("World", carmen_i18n_get(ctx, "ui.sub"));
    carmen_i18n_free(ctx);
}

static void test_load_json_respects_length(void)
{
    /* Extra trailing bytes past len must be ignored (buffer need not be
       NUL-terminated at the logical end). */
    const char *json = "{ \"a\": \"b\" }GARBAGE";
    size_t len = strlen("{ \"a\": \"b\" }");
    CarmenI18n *ctx = carmen_i18n_load_json(json, len);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("b", carmen_i18n_get(ctx, "a"));
    carmen_i18n_free(ctx);
}

static void test_load_json_missing_key_returns_key(void)
{
    const char *json = "{ \"a\": \"b\" }";
    CarmenI18n *ctx = carmen_i18n_load_json(json, strlen(json));
    TEST_ASSERT_NOT_NULL(ctx);
    /* Unknown keys fall back to the key itself. */
    TEST_ASSERT_EQUAL_STRING("missing.key", carmen_i18n_get(ctx, "missing.key"));
    carmen_i18n_free(ctx);
}

static void test_load_json_ignores_underscore_keys(void)
{
    const char *json = "{ \"_comment\": \"ignore me\", \"real\": \"v\" }";
    CarmenI18n *ctx = carmen_i18n_load_json(json, strlen(json));
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("v", carmen_i18n_get(ctx, "real"));
    /* The underscore key is not stored, so it resolves to itself. */
    TEST_ASSERT_EQUAL_STRING("_comment", carmen_i18n_get(ctx, "_comment"));
    carmen_i18n_free(ctx);
}

static void test_load_json_null_returns_null(void)
{
    TEST_ASSERT_NULL(carmen_i18n_load_json(NULL, 0));
}

static void test_load_json_malformed_returns_null(void)
{
    const char *json = "{ not valid json ";
    TEST_ASSERT_NULL(carmen_i18n_load_json(json, strlen(json)));
}

static void test_load_json_non_object_returns_null(void)
{
    const char *json = "[1, 2, 3]";
    TEST_ASSERT_NULL(carmen_i18n_load_json(json, strlen(json)));
}

/* --- carmen_i18n_get guards --- */

static void test_get_null_ctx_returns_key(void)
{
    TEST_ASSERT_EQUAL_STRING("some.key", carmen_i18n_get(NULL, "some.key"));
}

static void test_get_null_key_returns_empty(void)
{
    const char *json = "{ \"a\": \"b\" }";
    CarmenI18n *ctx = carmen_i18n_load_json(json, strlen(json));
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("", carmen_i18n_get(ctx, NULL));
    carmen_i18n_free(ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_load_json_reads_keys);
    RUN_TEST(test_load_json_respects_length);
    RUN_TEST(test_load_json_missing_key_returns_key);
    RUN_TEST(test_load_json_ignores_underscore_keys);
    RUN_TEST(test_load_json_null_returns_null);
    RUN_TEST(test_load_json_malformed_returns_null);
    RUN_TEST(test_load_json_non_object_returns_null);
    RUN_TEST(test_get_null_ctx_returns_key);
    RUN_TEST(test_get_null_key_returns_empty);
    return UNITY_END();
}
