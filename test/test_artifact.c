#include "unity.h"
#include "carmen/artifact.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_catalog_not_empty(void)
{
    TEST_ASSERT_GREATER_THAN(0, CARMEN_ARTIFACT_COUNT);
}

static void test_all_fields_populated(void)
{
    for (int i = 0; i < CARMEN_ARTIFACT_COUNT; i++) {
        const CarmenArtifact *a = &CARMEN_ARTIFACTS[i];
        TEST_ASSERT_NOT_NULL_MESSAGE(a->id, "id is NULL");
        TEST_ASSERT_NOT_NULL_MESSAGE(a->name, "name is NULL");
        TEST_ASSERT_NOT_NULL_MESSAGE(a->origin_city_id, "origin_city_id is NULL");
        TEST_ASSERT_NOT_NULL_MESSAGE(a->description, "description is NULL");

        TEST_ASSERT_GREATER_THAN_MESSAGE(0, (int)strlen(a->id),
                                         "id is empty");
        TEST_ASSERT_GREATER_THAN_MESSAGE(0, (int)strlen(a->name),
                                         "name is empty");
        TEST_ASSERT_GREATER_THAN_MESSAGE(0, (int)strlen(a->origin_city_id),
                                         "origin_city_id is empty");
        TEST_ASSERT_GREATER_THAN_MESSAGE(0, (int)strlen(a->description),
                                         "description is empty");
    }
}

static void test_no_duplicate_ids(void)
{
    for (int i = 0; i < CARMEN_ARTIFACT_COUNT; i++)
        for (int j = i + 1; j < CARMEN_ARTIFACT_COUNT; j++)
            TEST_ASSERT_NOT_EQUAL_MESSAGE(
                0, strcmp(CARMEN_ARTIFACTS[i].id, CARMEN_ARTIFACTS[j].id),
                "duplicate artifact id found");
}

static void test_count_matches_array(void)
{
    int count = 0;
    while (count < CARMEN_ARTIFACT_COUNT &&
           CARMEN_ARTIFACTS[count].id != NULL)
        count++;
    TEST_ASSERT_EQUAL_INT(CARMEN_ARTIFACT_COUNT, count);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_catalog_not_empty);
    RUN_TEST(test_all_fields_populated);
    RUN_TEST(test_no_duplicate_ids);
    RUN_TEST(test_count_matches_array);
    return UNITY_END();
}
