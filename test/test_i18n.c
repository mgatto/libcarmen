#include "unity.h"
#include "carmen/carmen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* Temp file used by the carmen_i18n_load tests. */
static const char *I18N_TMP = "carmen_i18n_test.tmp.json";

/**
 * Writes the specified bytes to the temporary test file.
 * @param data Data to write.
 * @param len Number of bytes to write.
 */
static void write_tmp(const void *data, size_t len)
{
    FILE *f = fopen(I18N_TMP, "wb");
    TEST_ASSERT_NOT_NULL(f);
    if (len > 0)
        TEST_ASSERT_EQUAL(len, fwrite(data, 1, len, f));
    TEST_ASSERT_EQUAL(0, fclose(f));
}

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

/* --- carmen_i18n_load --- */

static void test_load_reads_from_file(void)
{
    const char *json = "{ \"ui.title\": \"Hi\", \"ui.sub\": \"There\" }";
    write_tmp(json, strlen(json));

    CarmenI18n *ctx = carmen_i18n_load(I18N_TMP);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Hi",    carmen_i18n_get(ctx, "ui.title"));
    TEST_ASSERT_EQUAL_STRING("There", carmen_i18n_get(ctx, "ui.sub"));
    carmen_i18n_free(ctx);
    remove(I18N_TMP);
}

/**
 * Verifies that loading translations with a null path returns NULL.
 */
static void test_load_null_path_returns_null(void)
{
    TEST_ASSERT_NULL(carmen_i18n_load(NULL));
}

/**
 * Verifies that loading a nonexistent file returns NULL.
 */
static void test_load_missing_file_returns_null(void)
{
    /* Guarantee the file does not exist. */
    remove(I18N_TMP);
    TEST_ASSERT_NULL(carmen_i18n_load(I18N_TMP));
}

static void test_load_oversized_file_returns_null(void)
{
    /* Write a file one byte beyond the cap.  Content doesn't matter because
       the size check fires before any parsing attempt. */
    const size_t oversized = CARMEN_I18N_MAX_FILE_SIZE + 1;
    char        *buf       = malloc(oversized);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, ' ', oversized);
    write_tmp(buf, oversized);
    free(buf);

    TEST_ASSERT_NULL(carmen_i18n_load(I18N_TMP));
    remove(I18N_TMP);
}

static void test_load_at_cap_succeeds(void)
{
    /* A file of exactly CARMEN_I18N_MAX_FILE_SIZE bytes must be accepted
       (the guard is >, not >=).  Build a valid JSON object padded with
       trailing spaces to reach exactly the cap. */
    const char  *prefix    = "{ \"ui.title\": \"Cap\" }";
    const size_t plen      = strlen(prefix);
    const size_t total     = CARMEN_I18N_MAX_FILE_SIZE;
    TEST_ASSERT_TRUE(plen < total); /* sanity: padding fits */

    char *buf = malloc(total);
    TEST_ASSERT_NOT_NULL(buf);
    memcpy(buf, prefix, plen);
    memset(buf + plen, ' ', total - plen); /* pad with spaces (valid JSON) */
    write_tmp(buf, total);
    free(buf);

    CarmenI18n *ctx = carmen_i18n_load(I18N_TMP);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL_STRING("Cap", carmen_i18n_get(ctx, "ui.title"));
    carmen_i18n_free(ctx);
    remove(I18N_TMP);
}

/**
 * Verifies that loading an empty file returns NULL.
 */
static void test_load_empty_file_returns_null(void)
{
    /* len <= 0 branch: fseek/ftell gives 0 bytes, must return NULL. */
    write_tmp("", 0);
    TEST_ASSERT_NULL(carmen_i18n_load(I18N_TMP));
    remove(I18N_TMP);
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
    RUN_TEST(test_load_reads_from_file);
    RUN_TEST(test_load_null_path_returns_null);
    RUN_TEST(test_load_missing_file_returns_null);
    RUN_TEST(test_load_oversized_file_returns_null);
    RUN_TEST(test_load_at_cap_succeeds);
    RUN_TEST(test_load_empty_file_returns_null);
    return UNITY_END();
}
