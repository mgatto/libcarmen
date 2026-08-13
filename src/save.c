#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carmen/save.h"
#include "carmen/city.h"
#include "cJSON.h"

/* --------------------------------------------------------------------------
 * Small typed field readers. Each returns 1 on success (field present and of
 * the expected type/range) and 0 otherwise, leaving *out untouched on failure.
 * ------------------------------------------------------------------------ */

static int read_str(const cJSON *obj, const char *key, char *out, size_t cap)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsString(it) || !it->valuestring) return 0;
    if (strlen(it->valuestring) >= cap) return 0;
    memcpy(out, it->valuestring, strlen(it->valuestring) + 1);
    return 1;
}

static int read_int(const cJSON *obj, const char *key, int *out)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsNumber(it)) return 0;
    *out = it->valueint;
    return 1;
}

static int read_uint(const cJSON *obj, const char *key, unsigned int *out)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsNumber(it) || it->valuedouble < 0) return 0;
    *out = (unsigned int)it->valuedouble;
    return 1;
}

/* --------------------------------------------------------------------------
 * Catalog lookups: villain and artifact are stored by id and re-resolved from
 * the built-in catalogs on load, since a CarmenCase holds a pointer into
 * FITNA_VILLAINS[] and a copy of a CARMEN_ARTIFACTS[] entry (whose fields are
 * pointers into static storage).
 * ------------------------------------------------------------------------ */

static const FitnaVillain *find_villain(const char *id)
{
    if (!id || !*id) return NULL;
    for (int i = 0; i < carmen_villain_count(); i++) {
        const FitnaVillain *v = carmen_villain_at(i);
        if (v && v->id && strcmp(v->id, id) == 0) return v;
    }
    return NULL;
}

static const CarmenArtifact *find_artifact(const char *id)
{
    if (!id || !*id) return NULL;
    for (int i = 0; i < carmen_artifact_count(); i++) {
        const CarmenArtifact *a = carmen_artifact_at(i);
        if (a && a->id && strcmp(a->id, id) == 0) return a;
    }
    return NULL;
}

/* --------------------------------------------------------------------------
 * Serialization (build cJSON).
 * ------------------------------------------------------------------------ */

static cJSON *clue_to_json(const CarmenClue *c)
{
    cJSON *o = cJSON_CreateObject();
    if (!o) return NULL;
    cJSON_AddStringToObject(o, "text", c->text);
    cJSON_AddStringToObject(o, "target_city_id", c->target_city_id);
    cJSON_AddNumberToObject(o, "type", c->type);
    return o;
}

static cJSON *settings_to_json(const CarmenCaseSettings *st)
{
    cJSON *o = cJSON_CreateObject();
    if (!o) return NULL;
    cJSON_AddNumberToObject(o, "difficulty", st->difficulty);
    cJSON_AddNumberToObject(o, "trail_length", st->trail_length);
    cJSON_AddNumberToObject(o, "time_budget_hrs", st->time_budget_hrs);
    cJSON_AddNumberToObject(o, "active_sites_per_city", st->active_sites_per_city);
    cJSON_AddNumberToObject(o, "positive_clues_per_stop", st->positive_clues_per_stop);
    cJSON_AddNumberToObject(o, "move_limit", st->move_limit);
    cJSON_AddNumberToObject(o, "visited_history_size", st->visited_history_size);
    return o;
}

static cJSON *case_to_json(const CarmenCase *c)
{
    cJSON *o = cJSON_CreateObject();
    if (!o) return NULL;

    cJSON_AddStringToObject(o, "villain_id", c->villain && c->villain->id ? c->villain->id : "");
    cJSON_AddStringToObject(o, "artifact_id", c->artifact.id ? c->artifact.id : "");
    cJSON_AddStringToObject(o, "origin_id", c->origin_id);
    cJSON_AddStringToObject(o, "hideout_id", c->hideout_id);
    cJSON_AddNumberToObject(o, "time_budget_hrs", c->time_budget_hrs);
    cJSON_AddNumberToObject(o, "difficulty", c->difficulty);

    cJSON *trail = cJSON_AddArrayToObject(o, "trail");
    if (!trail) {
        cJSON_Delete(o);
        return NULL;
    }
    for (int i = 0; i < c->trail_len; i++)
        cJSON_AddItemToArray(trail, cJSON_CreateString(c->trail[i]));

    cJSON *stops = cJSON_AddArrayToObject(o, "stops");
    if (!stops) {
        cJSON_Delete(o);
        return NULL;
    }
    for (int i = 0; i < c->trail_len; i++) {
        cJSON *stop = cJSON_CreateObject();
        if (!stop) {
            cJSON_Delete(o);
            return NULL;
        }
        cJSON_AddItemToArray(stops, stop);

        cJSON *sites = cJSON_AddArrayToObject(stop, "sites");
        if (!sites) {
            cJSON_Delete(o);
            return NULL;
        }
        for (int j = 0; j < c->stops[i].site_count; j++) {
            cJSON *site = cJSON_CreateObject();
            if (!site) {
                cJSON_Delete(o);
                return NULL;
            }
            cJSON_AddItemToArray(sites, site);
            cJSON_AddNumberToObject(site, "site_idx", c->stops[i].sites[j].site_idx);
            cJSON *clue = clue_to_json(&c->stops[i].sites[j].clue);
            if (!clue) {
                cJSON_Delete(o);
                return NULL;
            }
            cJSON_AddItemToObject(site, "clue", clue);
        }
    }

    return o;
}

static cJSON *session_to_json(const CarmenSession *s)
{
    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    cJSON_AddNumberToObject(root, "schema_version", CARMEN_SAVE_SCHEMA_VERSION);
    cJSON_AddNumberToObject(root, "status", s->status);
    cJSON_AddStringToObject(root, "current_city_id", s->current_city_id);
    cJSON_AddNumberToObject(root, "time_remaining_hrs", s->time_remaining_hrs);
    cJSON_AddNumberToObject(root, "moves", s->moves);
    cJSON_AddNumberToObject(root, "warrant_villain_idx", s->warrant_villain_idx);
    cJSON_AddNumberToObject(root, "hideout_investigated_sites",
                            (double)s->hideout_investigated_sites);

    cJSON *visited = cJSON_AddArrayToObject(root, "visited");
    if (!visited) {
        cJSON_Delete(root);
        return NULL;
    }
    for (int i = 0; i < s->visited_count; i++)
        cJSON_AddItemToArray(visited, cJSON_CreateString(s->visited[i]));

    cJSON *notebook = cJSON_AddArrayToObject(root, "notebook");
    if (!notebook) {
        cJSON_Delete(root);
        return NULL;
    }
    for (int i = 0; i < s->notebook_count; i++) {
        cJSON *clue = clue_to_json(&s->notebook[i]);
        if (!clue) {
            cJSON_Delete(root);
            return NULL;
        }
        cJSON_AddItemToArray(notebook, clue);
    }

    cJSON *evidence = cJSON_AddArrayToObject(root, "evidence");
    if (!evidence) {
        cJSON_Delete(root);
        return NULL;
    }
    for (int i = 0; i < s->evidence_count; i++)
        cJSON_AddItemToArray(evidence, cJSON_CreateString(s->evidence[i]));

    cJSON *settings = settings_to_json(&s->settings);
    if (!settings) {
        cJSON_Delete(root);
        return NULL;
    }
    cJSON_AddItemToObject(root, "settings", settings);

    cJSON *active = case_to_json(&s->active_case);
    if (!active) {
        cJSON_Delete(root);
        return NULL;
    }
    cJSON_AddItemToObject(root, "case", active);

    return root;
}

int carmen_session_save(const CarmenSession *s, char *buf, int buf_size)
{
    if (buf && buf_size > 0) buf[0] = '\0';
    if (!s) return 0;

    cJSON *root = session_to_json(s);
    if (!root) return 0;

    char *out = cJSON_Print(root);
    cJSON_Delete(root);
    if (!out) return 0;

    int len = (int)strlen(out);
    if (buf && buf_size > 0) {
        int n = len < buf_size - 1 ? len : buf_size - 1;
        memcpy(buf, out, (size_t)n);
        buf[n] = '\0';
    }
    cJSON_free(out);
    return len;
}

int carmen_session_save_file(const CarmenSession *s, const char *path)
{
    if (!s || !path) return -1;

    cJSON *root = session_to_json(s);
    if (!root) return -2;
    char *out = cJSON_Print(root);
    cJSON_Delete(root);
    if (!out) return -2;

    FILE *f = fopen(path, "wb");
    if (!f) {
        cJSON_free(out);
        return -3;
    }

    size_t len = strlen(out);
    size_t nw  = fwrite(out, 1, len, f);
    cJSON_free(out);
    if (fclose(f) != 0 || nw != len) return -4;
    return 1;
}

/* --------------------------------------------------------------------------
 * Deserialization (parse cJSON into a scratch session, then let the caller
 * copy it into place only on full success).
 * ------------------------------------------------------------------------ */

static int clue_from_json(const cJSON *o, CarmenClue *c)
{
    if (!cJSON_IsObject(o)) return 0;
    memset(c, 0, sizeof(*c));
    if (!read_str(o, "text", c->text, sizeof(c->text))) return 0;
    if (!read_str(o, "target_city_id", c->target_city_id, sizeof(c->target_city_id))) return 0;
    int t;
    if (!read_int(o, "type", &t)) return 0;
    if (t != CARMEN_CLUE_POSITIVE && t != CARMEN_CLUE_NEGATIVE) return 0;
    c->type = (CarmenClueType)t;
    return 1;
}

static int settings_from_json(const cJSON *o, CarmenCaseSettings *out)
{
    if (!cJSON_IsObject(o)) return 0;
    int d;
    if (!read_int(o, "difficulty", &d)) return 0;
    if (d < CARMEN_DIFFICULTY_EASY || d > CARMEN_DIFFICULTY_HARD) return 0;
    out->difficulty = (CarmenDifficulty)d;
    return read_int(o, "trail_length", &out->trail_length) &&
           read_int(o, "time_budget_hrs", &out->time_budget_hrs) &&
           read_int(o, "active_sites_per_city", &out->active_sites_per_city) &&
           read_int(o, "positive_clues_per_stop", &out->positive_clues_per_stop) &&
           read_int(o, "move_limit", &out->move_limit) &&
           read_int(o, "visited_history_size", &out->visited_history_size);
}

/* Fill c from JSON. Returns 1 on success, -4 for a malformed/out-of-range
   field, -5 for an unknown villain id, -6 for an unknown artifact id. */
static int case_from_json(const cJSON *o, CarmenCase *c)
{
    if (!cJSON_IsObject(o)) return -4;
    memset(c, 0, sizeof(*c));

    char villain_id[CARMEN_MAX_NAME_LEN];
    char artifact_id[CARMEN_MAX_NAME_LEN];
    if (!read_str(o, "villain_id", villain_id, sizeof(villain_id))) return -4;
    if (!read_str(o, "artifact_id", artifact_id, sizeof(artifact_id))) return -4;
    if (!read_str(o, "origin_id", c->origin_id, sizeof(c->origin_id))) return -4;
    if (!read_str(o, "hideout_id", c->hideout_id, sizeof(c->hideout_id))) return -4;
    if (!read_int(o, "time_budget_hrs", &c->time_budget_hrs)) return -4;

    int d;
    if (!read_int(o, "difficulty", &d)) return -4;
    if (d < CARMEN_DIFFICULTY_EASY || d > CARMEN_DIFFICULTY_HARD) return -4;
    c->difficulty = (CarmenDifficulty)d;

    const FitnaVillain *v = find_villain(villain_id);
    if (!v) return -5;
    c->villain = v;

    const CarmenArtifact *a = find_artifact(artifact_id);
    if (!a) return -6;
    c->artifact = *a;

    const cJSON *trail = cJSON_GetObjectItemCaseSensitive(o, "trail");
    if (!cJSON_IsArray(trail)) return -4;
    int trail_len = cJSON_GetArraySize(trail);
    if (trail_len < 2 || trail_len > CARMEN_MAX_TRAIL) return -4;
    c->trail_len = trail_len;
    for (int i = 0; i < trail_len; i++) {
        const cJSON *city = cJSON_GetArrayItem(trail, i);
        if (!cJSON_IsString(city) || !city->valuestring) return -4;
        if (strlen(city->valuestring) >= CARMEN_MAX_NAME_LEN) return -4;
        memcpy(c->trail[i], city->valuestring, strlen(city->valuestring) + 1);
    }

    const cJSON *stops = cJSON_GetObjectItemCaseSensitive(o, "stops");
    if (!cJSON_IsArray(stops)) return -4;
    if (cJSON_GetArraySize(stops) != trail_len) return -4;
    for (int i = 0; i < trail_len; i++) {
        const cJSON *stop = cJSON_GetArrayItem(stops, i);
        if (!cJSON_IsObject(stop)) return -4;
        const cJSON *sites = cJSON_GetObjectItemCaseSensitive(stop, "sites");
        if (!cJSON_IsArray(sites)) return -4;
        int site_count = cJSON_GetArraySize(sites);
        if (site_count < 0 || site_count > CARMEN_TRAIL_SITES) return -4;
        c->stops[i].site_count = site_count;
        for (int j = 0; j < site_count; j++) {
            const cJSON *site = cJSON_GetArrayItem(sites, j);
            if (!cJSON_IsObject(site)) return -4;
            int idx;
            if (!read_int(site, "site_idx", &idx)) return -4;
            if (idx < 0 || idx >= CARMEN_MAX_SITES) return -4;
            c->stops[i].sites[j].site_idx = idx;
            const cJSON *clue             = cJSON_GetObjectItemCaseSensitive(site, "clue");
            if (!clue_from_json(clue, &c->stops[i].sites[j].clue)) return -4;
        }
    }

    return 1;
}

/* Fill *t (already zeroed) from the parsed root. Returns 1 or a negative
   code (-3..-7) as documented on carmen_session_load. */
static int session_from_json(const cJSON *root, CarmenSession *t, CarmenWorld *w)
{
    int ver;
    if (!read_int(root, "schema_version", &ver) || ver != CARMEN_SAVE_SCHEMA_VERSION) return -3;

    int status;
    if (!read_int(root, "status", &status)) return -4;
    if (status < CARMEN_STATUS_PLAYING || status > CARMEN_STATUS_NOT_AT_HIDEOUT) return -4;
    t->status = (CarmenSessionStatus)status;

    if (!read_str(root, "current_city_id", t->current_city_id, sizeof(t->current_city_id)))
        return -4;
    if (!read_int(root, "time_remaining_hrs", &t->time_remaining_hrs)) return -4;
    if (!read_int(root, "moves", &t->moves)) return -4;
    if (!read_int(root, "warrant_villain_idx", &t->warrant_villain_idx)) return -4;
    if (t->warrant_villain_idx < -1 || t->warrant_villain_idx >= FITNA_VILLAIN_COUNT) return -4;
    if (!read_uint(root, "hideout_investigated_sites", &t->hideout_investigated_sites)) return -4;

    const cJSON *visited = cJSON_GetObjectItemCaseSensitive(root, "visited");
    if (!cJSON_IsArray(visited)) return -4;
    int vc = cJSON_GetArraySize(visited);
    if (vc < 0 || vc > CARMEN_MAX_VISITED) return -4;
    t->visited_count = vc;
    for (int i = 0; i < vc; i++) {
        const cJSON *city = cJSON_GetArrayItem(visited, i);
        if (!cJSON_IsString(city) || !city->valuestring) return -4;
        if (strlen(city->valuestring) >= CARMEN_MAX_NAME_LEN) return -4;
        memcpy(t->visited[i], city->valuestring, strlen(city->valuestring) + 1);
    }

    const cJSON *notebook = cJSON_GetObjectItemCaseSensitive(root, "notebook");
    if (!cJSON_IsArray(notebook)) return -4;
    int nc = cJSON_GetArraySize(notebook);
    if (nc < 0 || nc > CARMEN_MAX_NOTEBOOK) return -4;
    t->notebook_count = nc;
    for (int i = 0; i < nc; i++) {
        const cJSON *clue = cJSON_GetArrayItem(notebook, i);
        if (!clue_from_json(clue, &t->notebook[i])) return -4;
    }

    const cJSON *evidence = cJSON_GetObjectItemCaseSensitive(root, "evidence");
    if (!cJSON_IsArray(evidence)) return -4;
    int ec = cJSON_GetArraySize(evidence);
    if (ec < 0 || ec > FITNA_MAX_ID_CLUES) return -4;
    t->evidence_count = ec;
    for (int i = 0; i < ec; i++) {
        const cJSON *key = cJSON_GetArrayItem(evidence, i);
        if (!cJSON_IsString(key) || !key->valuestring) return -4;
        if (strlen(key->valuestring) >= CARMEN_MAX_CLUE_LEN) return -4;
        memcpy(t->evidence[i], key->valuestring, strlen(key->valuestring) + 1);
    }

    const cJSON *settings = cJSON_GetObjectItemCaseSensitive(root, "settings");
    if (!settings_from_json(settings, &t->settings)) return -4;

    const cJSON *active = cJSON_GetObjectItemCaseSensitive(root, "case");
    int          rc     = case_from_json(active, &t->active_case);
    if (rc != 1) return rc;

    /* City-existence check against the caller's world: the current city and
       every trail city must resolve, otherwise this save does not belong to
       this world and play would immediately break. */
    if (!carmen_world_find(w, t->current_city_id)) return -7;
    for (int i = 0; i < t->active_case.trail_len; i++)
        if (!carmen_world_find(w, t->active_case.trail[i])) return -7;

    t->world = w;
    return 1;
}

int carmen_session_load(CarmenSession *s, CarmenWorld *w, const char *json, size_t len)
{
    if (!s || !w || !json) return -1;

    cJSON *root = cJSON_ParseWithLength(json, len);
    if (!root) return -2;
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        return -2;
    }

    CarmenSession tmp;
    memset(&tmp, 0, sizeof(tmp));
    int rc = session_from_json(root, &tmp, w);
    cJSON_Delete(root);

    if (rc == 1) *s = tmp;
    return rc;
}

int carmen_session_load_file(CarmenSession *s, CarmenWorld *w, const char *path)
{
    if (!s || !w || !path) return -8;

    FILE *f = fopen(path, "rb");
    if (!f) return -8;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0 || (size_t)len > CARMEN_SAVE_MAX_FILE_SIZE) {
        fclose(f);
        return -9;
    }

    char *buf = malloc((size_t)len + 1);
    if (!buf) {
        fclose(f);
        return -8;
    }

    size_t nread = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[nread] = '\0';

    int rc = carmen_session_load(s, w, buf, nread);
    free(buf);
    return rc;
}
