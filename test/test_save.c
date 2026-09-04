#include "unity.h"
#include "carmen/carmen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Save/load tests reuse the same small deterministic world as the session
 * tests (3 cities A -> B -> C with a fixed RNG), so a generated case is fully
 * reproducible and a round-trip can be asserted field-by-field.
 */

static CarmenWorld *world;

static int det_seq[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int det_idx;

static int det_rand(void *ctx)
{
    (void)ctx;
    int val = det_seq[det_idx % (int)(sizeof(det_seq) / sizeof(det_seq[0]))];
    det_idx++;
    return val;
}

static void build_test_world(void)
{
    world = carmen_world_create();

    carmen_world_add_city(world, "a", "CityA", NULL, "CountryA", "X", 0, 0);
    carmen_world_add_city(world, "b", "CityB", NULL, "CountryA", "X", 1, 1);
    carmen_world_add_city(world, "c", "CityC", NULL, "CountryB", "X", 2, 2);

    CarmenCity *a = carmen_world_find(world, "a");
    CarmenCity *b = carmen_world_find(world, "b");
    CarmenCity *c = carmen_world_find(world, "c");

    CarmenConnection conn;
    carmen_connection_init(&conn, "b", 400, "flight");
    carmen_city_add_connection(a, &conn);
    carmen_connection_init(&conn, "a", 400, "flight");
    carmen_city_add_connection(b, &conn);
    carmen_connection_init(&conn, "c", 200, "train");
    carmen_city_add_connection(b, &conn);
    carmen_connection_init(&conn, "b", 600, "boat");
    carmen_city_add_connection(c, &conn);

    CarmenSite site;
    carmen_site_init(&site, "museum_a", "Museum A", "museum");
    carmen_city_add_site(a, &site);
    carmen_site_init(&site, "market_a", "Market A", "market");
    carmen_city_add_site(a, &site);
    carmen_site_init(&site, "mosque_a", "Mosque A", "mosque");
    carmen_city_add_site(a, &site);

    carmen_site_init(&site, "museum_b", "Museum B", "museum");
    carmen_city_add_site(b, &site);
    carmen_site_init(&site, "market_b", "Market B", "market");
    carmen_city_add_site(b, &site);
    carmen_site_init(&site, "mosque_b", "Mosque B", "mosque");
    carmen_city_add_site(b, &site);

    carmen_site_init(&site, "museum_c", "Museum C", "museum");
    carmen_city_add_site(c, &site);
    carmen_site_init(&site, "market_c", "Market C", "market");
    carmen_city_add_site(c, &site);
    carmen_site_init(&site, "mosque_c", "Mosque C", "mosque");
    carmen_city_add_site(c, &site);

    carmen_city_add_inbound_clue(a, "clue.a.inbound.0");
    carmen_city_add_inbound_clue(a, "clue.a.inbound.1");
    carmen_city_add_inbound_clue(b, "clue.b.inbound.0");
    carmen_city_add_inbound_clue(b, "clue.b.inbound.1");
    carmen_city_add_inbound_clue(c, "clue.c.inbound.0");
    carmen_city_add_inbound_clue(c, "clue.c.inbound.1");
}

void setUp(void)
{
    det_idx = 0;
    carmen_set_rand(det_rand, NULL);
    build_test_world();
}

void tearDown(void)
{
    carmen_set_rand(NULL, NULL);
    carmen_world_free(world);
    world = NULL;
}

static int start_easy(CarmenSession *s)
{
    CarmenCaseSettings settings = carmen_case_settings_default();
    settings.difficulty         = CARMEN_DIFFICULTY_EASY;
    return carmen_session_start(s, world, &settings);
}

static int villain_index(const CarmenSession *s)
{
    for (int i = 0; i < carmen_villain_count(); i++)
        if (carmen_villain_at(i) == s->active_case.villain) return i;
    return -1;
}

/* Play through the whole trail to the hideout and win, so a save captures a
   fully-populated, WON session. */
static void play_to_win(CarmenSession *s)
{
    for (int i = 1; i < s->active_case.trail_len; i++)
        carmen_session_travel(s, s->active_case.trail[i]);
    s->evidence_count = FITNA_MAX_ID_CLUES;
    carmen_session_issue_warrant(s, villain_index(s));
    carmen_session_arrest(s);
}

static void assert_clue_equal(const CarmenClue *a, const CarmenClue *b)
{
    TEST_ASSERT_EQUAL_STRING(a->text, b->text);
    TEST_ASSERT_EQUAL_STRING(a->target_city_id, b->target_city_id);
    TEST_ASSERT_EQUAL_INT(a->type, b->type);
}

static void assert_sessions_equal(const CarmenSession *a, const CarmenSession *b)
{
    TEST_ASSERT_EQUAL_PTR(a->world, b->world);
    TEST_ASSERT_EQUAL_INT(a->status, b->status);
    TEST_ASSERT_EQUAL_STRING(a->current_city_id, b->current_city_id);
    TEST_ASSERT_EQUAL_INT(a->time_remaining_hrs, b->time_remaining_hrs);
    TEST_ASSERT_EQUAL_INT(a->moves, b->moves);
    TEST_ASSERT_EQUAL_INT(a->warrant_villain_idx, b->warrant_villain_idx);
    TEST_ASSERT_EQUAL_UINT(a->hideout_investigated_sites, b->hideout_investigated_sites);

    TEST_ASSERT_EQUAL_INT(a->visited_count, b->visited_count);
    for (int i = 0; i < a->visited_count; i++)
        TEST_ASSERT_EQUAL_STRING(a->visited[i], b->visited[i]);

    TEST_ASSERT_EQUAL_INT(a->notebook_count, b->notebook_count);
    for (int i = 0; i < a->notebook_count; i++)
        assert_clue_equal(&a->notebook[i], &b->notebook[i]);

    TEST_ASSERT_EQUAL_INT(a->evidence_count, b->evidence_count);
    for (int i = 0; i < a->evidence_count; i++)
        TEST_ASSERT_EQUAL_STRING(a->evidence[i], b->evidence[i]);

    TEST_ASSERT_EQUAL_INT(a->settings.difficulty, b->settings.difficulty);
    TEST_ASSERT_EQUAL_INT(a->settings.trail_length, b->settings.trail_length);
    TEST_ASSERT_EQUAL_INT(a->settings.time_budget_hrs, b->settings.time_budget_hrs);
    TEST_ASSERT_EQUAL_INT(a->settings.active_sites_per_city, b->settings.active_sites_per_city);
    TEST_ASSERT_EQUAL_INT(a->settings.positive_clues_per_stop, b->settings.positive_clues_per_stop);
    TEST_ASSERT_EQUAL_INT(a->settings.move_limit, b->settings.move_limit);
    TEST_ASSERT_EQUAL_INT(a->settings.visited_history_size, b->settings.visited_history_size);

    TEST_ASSERT_EQUAL_PTR(a->active_case.villain, b->active_case.villain);
    TEST_ASSERT_EQUAL_STRING(a->active_case.artifact.id, b->active_case.artifact.id);
    TEST_ASSERT_EQUAL_STRING(a->active_case.artifact.origin_city_id,
                             b->active_case.artifact.origin_city_id);
    TEST_ASSERT_EQUAL_STRING(a->active_case.origin_id, b->active_case.origin_id);
    TEST_ASSERT_EQUAL_STRING(a->active_case.hideout_id, b->active_case.hideout_id);
    TEST_ASSERT_EQUAL_INT(a->active_case.time_budget_hrs, b->active_case.time_budget_hrs);
    TEST_ASSERT_EQUAL_INT(a->active_case.difficulty, b->active_case.difficulty);

    TEST_ASSERT_EQUAL_INT(a->active_case.trail_len, b->active_case.trail_len);
    for (int i = 0; i < a->active_case.trail_len; i++)
        TEST_ASSERT_EQUAL_STRING(a->active_case.trail[i], b->active_case.trail[i]);
    for (int i = 0; i < a->active_case.trail_len; i++) {
        TEST_ASSERT_EQUAL_INT(a->active_case.stops[i].site_count,
                              b->active_case.stops[i].site_count);
        for (int j = 0; j < a->active_case.stops[i].site_count; j++) {
            TEST_ASSERT_EQUAL_INT(a->active_case.stops[i].sites[j].site_idx,
                                  b->active_case.stops[i].sites[j].site_idx);
            assert_clue_equal(&a->active_case.stops[i].sites[j].clue,
                              &b->active_case.stops[i].sites[j].clue);
        }
    }
}

/* A canonical, valid save document built from the real catalog ids, used as
   the baseline for the load error-path tests. Individual fields are varied per
   test to exercise a single failure mode at a time. */
static void build_json(char *buf, size_t cap, int schema, int status, const char *current,
                       const char *villain, const char *artifact, const char *origin,
                       const char *hideout, int difficulty, const char *trail0, const char *trail1,
                       int site_idx, int clue_type)
{
    snprintf(buf, cap,
             "{"
             "\"schema_version\":%d,"
             "\"status\":%d,"
             "\"current_city_id\":\"%s\","
             "\"time_remaining_hrs\":40,"
             "\"moves\":2,"
             "\"warrant_villain_idx\":-1,"
             "\"hideout_investigated_sites\":0,"
             "\"visited\":[\"a\"],"
             "\"notebook\":[],"
             "\"evidence\":[],"
             "\"settings\":{\"difficulty\":0,\"trail_length\":0,"
             "\"time_budget_hrs\":0,\"active_sites_per_city\":0,"
             "\"positive_clues_per_stop\":0,\"move_limit\":0,"
             "\"visited_history_size\":0},"
             "\"edges\":[{\"from\":\"a\",\"to\":\"b\",\"km\":400,\"mode\":\"flight\"},"
             "{\"from\":\"b\",\"to\":\"c\",\"km\":200,\"mode\":\"train\"}],"
             "\"case\":{"
             "\"villain_id\":\"%s\",\"artifact_id\":\"%s\","
             "\"origin_id\":\"%s\",\"hideout_id\":\"%s\","
             "\"time_budget_hrs\":48,\"difficulty\":%d,"
             "\"trail\":[\"%s\",\"%s\"],"
             "\"stops\":["
             "{\"sites\":[{\"site_idx\":%d,\"clue\":{\"text\":\"\","
             "\"target_city_id\":\"b\",\"type\":%d}}]},"
             "{\"sites\":[{\"site_idx\":0,\"clue\":{\"text\":\"\","
             "\"target_city_id\":\"\",\"type\":1}}]}"
             "]}"
             "}",
             schema, status, current, villain, artifact, origin, hideout, difficulty, trail0,
             trail1, site_idx, clue_type);
}

/* Build a document with caller-supplied trail/stops JSON fragments so tests can
   exercise trail-length and stops-count mismatches. */
static void build_json_case(char *buf, size_t cap, const char *villain, const char *artifact,
                            const char *trail_json, const char *stops_json)
{
    snprintf(buf, cap,
             "{\"schema_version\":2,\"status\":0,\"current_city_id\":\"a\","
             "\"time_remaining_hrs\":40,\"moves\":0,\"warrant_villain_idx\":-1,"
             "\"hideout_investigated_sites\":0,\"visited\":[\"a\"],"
             "\"notebook\":[],\"evidence\":[],"
             "\"settings\":{\"difficulty\":0,\"trail_length\":0,"
             "\"time_budget_hrs\":0,\"active_sites_per_city\":0,"
             "\"positive_clues_per_stop\":0,\"move_limit\":0,"
             "\"visited_history_size\":0},"
             "\"edges\":[{\"from\":\"a\",\"to\":\"b\",\"km\":400,\"mode\":\"flight\"},"
             "{\"from\":\"b\",\"to\":\"c\",\"km\":200,\"mode\":\"train\"}],"
             "\"case\":{\"villain_id\":\"%s\",\"artifact_id\":\"%s\","
             "\"origin_id\":\"a\",\"hideout_id\":\"a\","
             "\"time_budget_hrs\":48,\"difficulty\":0,"
             "\"trail\":%s,\"stops\":%s}}",
             villain, artifact, trail_json, stops_json);
}

static const char *cat_villain_id(void)
{
    return FITNA_VILLAINS[0].id;
}
static const char *cat_artifact_id(void)
{
    return CARMEN_ARTIFACTS[0].id;
}

/* --- carmen_session_save --- */

static void test_save_null_session_returns_zero(void)
{
    char buf[64] = "sentinel";
    TEST_ASSERT_EQUAL_INT(0, carmen_session_save(NULL, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("", buf);
}

static void test_save_returns_length_and_nul_terminates(void)
{
    CarmenSession s;
    start_easy(&s);

    char buf[16384];
    int  len = carmen_session_save(&s, buf, sizeof(buf));
    TEST_ASSERT_GREATER_THAN(0, len);
    TEST_ASSERT_EQUAL_INT(len, (int)strlen(buf));
}

static void test_save_length_query_with_null_buffer(void)
{
    CarmenSession s;
    start_easy(&s);

    int needed = carmen_session_save(&s, NULL, 0);
    TEST_ASSERT_GREATER_THAN(0, needed);

    char buf[16384];
    int  len = carmen_session_save(&s, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(needed, len);
}

static void test_save_truncates_and_nul_terminates_small_buffer(void)
{
    CarmenSession s;
    start_easy(&s);

    char buf[10];
    memset(buf, 'x', sizeof(buf));
    int len = carmen_session_save(&s, buf, sizeof(buf));

    TEST_ASSERT_GREATER_THAN((int)sizeof(buf), len); /* full length reported */
    TEST_ASSERT_EQUAL_INT(9, (int)strlen(buf));      /* truncated to fit */
    TEST_ASSERT_EQUAL_CHAR('\0', buf[9]);
}

/* --- carmen_session_load: round trips --- */

static void test_round_trip_fresh_session(void)
{
    CarmenSession s;
    start_easy(&s);

    char buf[16384];
    carmen_session_save(&s, buf, sizeof(buf));

    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&loaded, world, buf, strlen(buf)));
    assert_sessions_equal(&s, &loaded);
}

static void test_round_trip_after_play(void)
{
    CarmenSession s;
    start_easy(&s);

    int sites[CARMEN_TRAIL_SITES];
    int n = carmen_session_active_sites(&s, sites, CARMEN_TRAIL_SITES);
    if (n > 0) carmen_session_investigate(&s, sites[0]);
    carmen_session_travel(&s, s.active_case.trail[1]);
    n = carmen_session_active_sites(&s, sites, CARMEN_TRAIL_SITES);
    if (n > 0) carmen_session_investigate(&s, sites[0]);

    TEST_ASSERT_GREATER_THAN(0, s.notebook_count);
    TEST_ASSERT_GREATER_THAN(0, s.moves);

    char buf[16384];
    carmen_session_save(&s, buf, sizeof(buf));

    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&loaded, world, buf, strlen(buf)));
    assert_sessions_equal(&s, &loaded);
}

static void test_round_trip_won_session_preserves_score(void)
{
    CarmenSession s;
    start_easy(&s);
    play_to_win(&s);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_WON, carmen_session_status(&s));
    int score = carmen_session_score(&s);
    TEST_ASSERT_GREATER_THAN(0, score);

    char buf[16384];
    carmen_session_save(&s, buf, sizeof(buf));

    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&loaded, world, buf, strlen(buf)));
    assert_sessions_equal(&s, &loaded);
    TEST_ASSERT_EQUAL_INT(score, carmen_session_score(&loaded));
}

static void test_round_trip_restores_world_graph(void)
{
    CarmenSession s;
    start_easy(&s);

    CarmenCity *a = carmen_world_find(world, "a");
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(a, "b"));

    char buf[16384];
    carmen_session_save(&s, buf, sizeof(buf));

    carmen_world_clear_connections(world);
    TEST_ASSERT_EQUAL_INT(0, a->connection_count);

    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&loaded, world, buf, strlen(buf)));
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(a, "b"));
    TEST_ASSERT_EQUAL_INT(0, carmen_session_travel(&loaded, "b"));
}

static void test_islamic_save_load_graph_and_travel(void)
{
    carmen_set_rand(NULL, NULL);
    srand(42);
    CarmenWorld *w = carmen_world_create();
    TEST_ASSERT_NOT_NULL(w);
    carmen_world_build_islamic(w);

    CarmenCaseSettings settings = carmen_case_settings_default();
    settings.difficulty = CARMEN_DIFFICULTY_EASY;
    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_start(&s, w, &settings));

    char next[CARMEN_MAX_NAME_LEN];
    carmen_utf8_copy(next, CARMEN_MAX_NAME_LEN, s.active_case.trail[1]);

    char buf[16384];
    TEST_ASSERT_GREATER_THAN(0, carmen_session_save(&s, buf, sizeof(buf)));

    carmen_world_clear_connections(w);
    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&loaded, w, buf, strlen(buf)));

    const CarmenCity *origin = carmen_world_find(w, loaded.current_city_id);
    TEST_ASSERT_NOT_NULL(origin);
    TEST_ASSERT_EQUAL_INT(3, origin->connection_count);
    TEST_ASSERT_EQUAL_INT(1, carmen_city_has_connection_to(origin, next));
    TEST_ASSERT_EQUAL_INT(0, carmen_session_travel(&loaded, next));
    carmen_world_free(w);
}

static void test_load_resolves_villain_to_catalog_pointer(void)
{
    CarmenSession s;
    start_easy(&s);

    char buf[16384];
    carmen_session_save(&s, buf, sizeof(buf));

    CarmenSession loaded;
    carmen_session_load(&loaded, world, buf, strlen(buf));

    /* The restored villain must be the same static catalog entry, not a copy. */
    int found = 0;
    for (int i = 0; i < carmen_villain_count(); i++)
        if (carmen_villain_at(i) == loaded.active_case.villain) found = 1;
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_PTR(s.active_case.villain, loaded.active_case.villain);
}

/* --- carmen_session_load: argument guards --- */

static void test_load_null_session_returns_neg1(void)
{
    char json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_load(NULL, world, json, strlen(json)));
}

static void test_load_null_world_returns_neg1(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_load(&s, NULL, json, strlen(json)));
}

static void test_load_null_json_returns_neg1(void)
{
    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_load(&s, world, NULL, 0));
}

/* --- carmen_session_load: parse / schema errors --- */

static void test_load_invalid_json_returns_neg2(void)
{
    CarmenSession s;
    const char   *bad = "{ this is not json";
    TEST_ASSERT_EQUAL_INT(-2, carmen_session_load(&s, world, bad, strlen(bad)));
}

static void test_load_non_object_root_returns_neg2(void)
{
    CarmenSession s;
    const char   *arr = "[1,2,3]";
    TEST_ASSERT_EQUAL_INT(-2, carmen_session_load(&s, world, arr, strlen(arr)));
}

static void test_load_missing_schema_returns_neg3(void)
{
    CarmenSession s;
    const char   *doc = "{\"status\":0}";
    TEST_ASSERT_EQUAL_INT(-3, carmen_session_load(&s, world, doc, strlen(doc)));
}

static void test_load_wrong_schema_returns_neg3(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 999, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-3, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_v1_schema_returns_neg3(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 1, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-3, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_missing_edges_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    snprintf(json, sizeof(json),
             "{\"schema_version\":2,\"status\":0,\"current_city_id\":\"a\","
             "\"time_remaining_hrs\":40,\"moves\":0,\"warrant_villain_idx\":-1,"
             "\"hideout_investigated_sites\":0,\"visited\":[\"a\"],"
             "\"notebook\":[],\"evidence\":[],"
             "\"settings\":{\"difficulty\":0,\"trail_length\":0,"
             "\"time_budget_hrs\":0,\"active_sites_per_city\":0,"
             "\"positive_clues_per_stop\":0,\"move_limit\":0,"
             "\"visited_history_size\":0},"
             "\"case\":{\"villain_id\":\"%s\",\"artifact_id\":\"%s\","
             "\"origin_id\":\"a\",\"hideout_id\":\"b\","
             "\"time_budget_hrs\":48,\"difficulty\":0,"
             "\"trail\":[\"a\",\"b\"],"
             "\"stops\":[{\"sites\":[{\"site_idx\":0,\"clue\":{\"text\":\"\","
             "\"target_city_id\":\"b\",\"type\":1}}]},"
             "{\"sites\":[{\"site_idx\":0,\"clue\":{\"text\":\"\","
             "\"target_city_id\":\"\",\"type\":1}}]}]}}",
             cat_villain_id(), cat_artifact_id());
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

/* --- carmen_session_load: malformed field (-4) --- */

static void test_load_bad_status_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 99, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_bad_site_idx_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "b", 99, 1);
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_bad_clue_type_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "b", 0, 5);
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_short_trail_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    build_json_case(json, sizeof(json), cat_villain_id(), cat_artifact_id(), "[\"a\"]",
                    "[{\"sites\":[]}]");
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_stops_mismatch_returns_neg4(void)
{
    CarmenSession s;
    char          json[2048];
    build_json_case(json, sizeof(json), cat_villain_id(), cat_artifact_id(), "[\"a\",\"b\"]",
                    "[{\"sites\":[]}]");
    TEST_ASSERT_EQUAL_INT(-4, carmen_session_load(&s, world, json, strlen(json)));
}

/* --- carmen_session_load: catalog / world resolution (-5, -6, -7) --- */

static void test_load_unknown_villain_returns_neg5(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", "no_such_villain", cat_artifact_id(), "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-5, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_unknown_artifact_returns_neg6(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), "no_such_artifact", "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-6, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_current_city_not_in_world_returns_neg7(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "zzz", cat_villain_id(), cat_artifact_id(), "a", "b", 0,
               "a", "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(-7, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_trail_city_not_in_world_returns_neg7(void)
{
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "zzz", 0, 1);
    TEST_ASSERT_EQUAL_INT(-7, carmen_session_load(&s, world, json, strlen(json)));
}

static void test_load_leaves_session_unchanged_on_failure(void)
{
    /* Load a valid session first, then a bad one into the same struct. */
    CarmenSession s;
    char          json[2048];
    build_json(json, sizeof(json), 2, 0, "a", cat_villain_id(), cat_artifact_id(), "a", "b", 0, "a",
               "b", 0, 1);
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load(&s, world, json, strlen(json)));

    const char *bad = "{ not json";
    TEST_ASSERT_EQUAL_INT(-2, carmen_session_load(&s, world, bad, strlen(bad)));

    /* Unchanged: still the good load. */
    TEST_ASSERT_EQUAL_STRING("a", s.current_city_id);
    TEST_ASSERT_EQUAL_INT(CARMEN_STATUS_PLAYING, s.status);
}

/* --- carmen_session_save_file / load_file --- */

static void test_save_file_and_load_file_round_trip(void)
{
    CarmenSession s;
    start_easy(&s);

    const char *path = "test_save_roundtrip.tmp.json";
    TEST_ASSERT_EQUAL_INT(1, carmen_session_save_file(&s, path));

    CarmenSession loaded;
    TEST_ASSERT_EQUAL_INT(1, carmen_session_load_file(&loaded, world, path));
    assert_sessions_equal(&s, &loaded);

    remove(path);
}

static void test_save_file_null_args(void)
{
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_save_file(NULL, "x.json"));
    TEST_ASSERT_EQUAL_INT(-1, carmen_session_save_file(&s, NULL));
}

static void test_save_file_unwritable_path_returns_neg3(void)
{
    /* fopen("no_such_dir/x.json", "wb") fails because the directory does not
       exist, exercising the -3 branch in carmen_session_save_file. */
    CarmenSession s;
    start_easy(&s);
    TEST_ASSERT_EQUAL_INT(-3, carmen_session_save_file(&s, "carmen_no_such_dir/x.json"));
}

static void test_load_file_oversized_returns_neg9(void)
{
    /* Write a file one byte beyond CARMEN_SAVE_MAX_FILE_SIZE.  The size guard
       fires before any JSON parsing, so content doesn't matter. */
    const char *path     = "carmen_save_oversized.tmp.json";
    const size_t oversized = CARMEN_SAVE_MAX_FILE_SIZE + 1;
    char        *buf       = malloc(oversized);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, ' ', oversized);
    FILE *f = fopen(path, "wb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(oversized, fwrite(buf, 1, oversized, f));
    fclose(f);
    free(buf);

    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-9, carmen_session_load_file(&s, world, path));
    remove(path);
}

static void test_load_file_null_args_returns_neg8(void)
{
    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-8, carmen_session_load_file(NULL, world, "x.json"));
    TEST_ASSERT_EQUAL_INT(-8, carmen_session_load_file(&s, NULL, "x.json"));
    TEST_ASSERT_EQUAL_INT(-8, carmen_session_load_file(&s, world, NULL));
}

static void test_load_file_missing_file_returns_neg8(void)
{
    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-8, carmen_session_load_file(&s, world, "definitely_not_here.json"));
}

static void test_load_file_empty_file_returns_neg9(void)
{
    const char *path = "test_save_empty.tmp.json";
    FILE       *f    = fopen(path, "wb");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);

    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-9, carmen_session_load_file(&s, world, path));
    remove(path);
}

static void test_load_file_propagates_content_error(void)
{
    /* A well-formed file whose contents fail validation returns the content
       error code (-3 here), not a file-level code. */
    const char *path = "test_save_badcontent.tmp.json";
    FILE       *f    = fopen(path, "wb");
    TEST_ASSERT_NOT_NULL(f);
    const char *doc = "{\"status\":0}";
    fwrite(doc, 1, strlen(doc), f);
    fclose(f);

    CarmenSession s;
    TEST_ASSERT_EQUAL_INT(-3, carmen_session_load_file(&s, world, path));
    remove(path);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_save_null_session_returns_zero);
    RUN_TEST(test_save_returns_length_and_nul_terminates);
    RUN_TEST(test_save_length_query_with_null_buffer);
    RUN_TEST(test_save_truncates_and_nul_terminates_small_buffer);

    RUN_TEST(test_round_trip_fresh_session);
    RUN_TEST(test_round_trip_after_play);
    RUN_TEST(test_round_trip_won_session_preserves_score);
    RUN_TEST(test_round_trip_restores_world_graph);
    RUN_TEST(test_islamic_save_load_graph_and_travel);
    RUN_TEST(test_load_resolves_villain_to_catalog_pointer);

    RUN_TEST(test_load_null_session_returns_neg1);
    RUN_TEST(test_load_null_world_returns_neg1);
    RUN_TEST(test_load_null_json_returns_neg1);

    RUN_TEST(test_load_invalid_json_returns_neg2);
    RUN_TEST(test_load_non_object_root_returns_neg2);
    RUN_TEST(test_load_missing_schema_returns_neg3);
    RUN_TEST(test_load_wrong_schema_returns_neg3);
    RUN_TEST(test_load_v1_schema_returns_neg3);
    RUN_TEST(test_load_missing_edges_returns_neg4);

    RUN_TEST(test_load_bad_status_returns_neg4);
    RUN_TEST(test_load_bad_site_idx_returns_neg4);
    RUN_TEST(test_load_bad_clue_type_returns_neg4);
    RUN_TEST(test_load_short_trail_returns_neg4);
    RUN_TEST(test_load_stops_mismatch_returns_neg4);

    RUN_TEST(test_load_unknown_villain_returns_neg5);
    RUN_TEST(test_load_unknown_artifact_returns_neg6);
    RUN_TEST(test_load_current_city_not_in_world_returns_neg7);
    RUN_TEST(test_load_trail_city_not_in_world_returns_neg7);
    RUN_TEST(test_load_leaves_session_unchanged_on_failure);

    RUN_TEST(test_save_file_and_load_file_round_trip);
    RUN_TEST(test_save_file_null_args);
    RUN_TEST(test_save_file_unwritable_path_returns_neg3);
    RUN_TEST(test_load_file_null_args_returns_neg8);
    RUN_TEST(test_load_file_missing_file_returns_neg8);
    RUN_TEST(test_load_file_empty_file_returns_neg9);
    RUN_TEST(test_load_file_oversized_returns_neg9);
    RUN_TEST(test_load_file_propagates_content_error);

    return UNITY_END();
}
