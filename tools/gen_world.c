/* gen_world -- build-time world preset compiler.
 *
 * Reads a JSONC world preset (presets/<name>.jsonc), validates it against the
 * library's CARMEN_MAX_* caps and reference integrity, and emits a C source
 * that defines carmen_world_build_islamic() by replaying the same
 * carmen_world_add_city()/set_sphere()/add_site()/add_inbound_clues()/
 * add_route() calls the hand-written seed used to make.
 *
 * This is a host build tool, not part of the shipped library, so (unlike the
 * library) it is allowed to print diagnostics and exit non-zero: any invalid
 * preset fails the build here rather than at runtime.
 *
 *   usage: gen_world <input.jsonc> <output.c>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

/* Caps + field sizes come straight from the public headers so validation can
 * never drift from what the runtime structs actually accept. */
#include "carmen/clue.h"       /* CARMEN_MAX_NAME_LEN, _CLUE_LEN, _INBOUND_CLUES */
#include "carmen/city.h"       /* CARMEN_MAX_SITES, CARMEN_MAX_CONNECTIONS */
#include "carmen/game_world.h" /* CARMEN_MAX_CITIES */

static const char *g_input = "?";

static void fail(const char *msg, const char *detail)
{
    if (detail && detail[0])
        fprintf(stderr, "gen_world: %s: '%s' (in %s)\n", msg, detail, g_input);
    else
        fprintf(stderr, "gen_world: %s (in %s)\n", msg, g_input);
    exit(EXIT_FAILURE);
}

static char *read_file(const char *path, size_t *out_len)
{
    FILE *f = fopen(path, "rb");
    if (!f) fail("cannot open preset", path);

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); fail("empty or unreadable preset", path); }

    char *buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); fail("out of memory reading preset", NULL); }

    size_t n = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[n] = '\0';
    if (out_len) *out_len = n;
    return buf;
}

/* Fetch a required string field, enforcing the destination buffer cap
 * (length must be < maxlen so it survives strncpy with a NUL terminator). */
static const char *req_str(const cJSON *obj, const char *key, size_t maxlen,
                           const char *ctx)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsString(it) || it->valuestring == NULL) {
        fprintf(stderr, "gen_world: missing or non-string field \"%s\" (%s) in %s\n",
                key, ctx, g_input);
        exit(EXIT_FAILURE);
    }
    if (strlen(it->valuestring) >= maxlen) {
        fprintf(stderr, "gen_world: field \"%s\" (%s) exceeds %zu bytes in %s\n",
                key, ctx, maxlen - 1, g_input);
        exit(EXIT_FAILURE);
    }
    return it->valuestring;
}

static double req_num(const cJSON *obj, const char *key, const char *ctx)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsNumber(it)) {
        fprintf(stderr, "gen_world: missing or non-numeric field \"%s\" (%s) in %s\n",
                key, ctx, g_input);
        exit(EXIT_FAILURE);
    }
    return it->valuedouble;
}

/* Emit a C string literal, escaping " and \\ and any non-printable byte as a
 * 3-digit octal escape (octal is fixed-width, so it can't swallow a following
 * digit the way \\x can).  Preset values are ASCII i18n keys today; the
 * escaping keeps future UTF-8 presets safe. */
static void emit_cstr(FILE *out, const char *s)
{
    fputc('"', out);
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        if (*p == '"' || *p == '\\') {
            fputc('\\', out);
            fputc((int)*p, out);
        } else if (*p >= 0x20 && *p < 0x7f) {
            fputc((int)*p, out);
        } else {
            fprintf(out, "\\%03o", (unsigned)*p);
        }
    }
    fputc('"', out);
}

static int find_id(char *const *ids, int n, const char *s)
{
    for (int i = 0; i < n; i++)
        if (strcmp(ids[i], s) == 0) return i;
    return -1;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input.jsonc> <output.c>\n", argv[0]);
        return 2;
    }
    g_input = argv[1];
    const char *outpath = argv[2];

    size_t len = 0;
    char *buf = read_file(argv[1], &len);
    cJSON_Minify(buf); /* strip line and block comments in place */

    cJSON *root = cJSON_ParseWithLength(buf, strlen(buf));
    if (!root || !cJSON_IsObject(root)) {
        cJSON_Delete(root);
        free(buf);
        fail("invalid JSON (after comment stripping)", NULL);
    }

    const char *world_id = req_str(root, "id", CARMEN_MAX_NAME_LEN, "root");

    cJSON *cities = cJSON_GetObjectItemCaseSensitive(root, "cities");
    if (!cJSON_IsArray(cities)) fail("\"cities\" must be an array", NULL);
    int ncities = cJSON_GetArraySize(cities);
    if (ncities < 1) fail("world must define at least one city", NULL);
    if (ncities > CARMEN_MAX_CITIES)
        fail("too many cities (exceeds CARMEN_MAX_CITIES)", NULL);

    char **ids = calloc((size_t)ncities, sizeof *ids);
    int   *deg = calloc((size_t)ncities, sizeof *deg);
    if (!ids || !deg) fail("out of memory", NULL);

    /* --- validate cities --- */
    int ci = 0;
    cJSON *city = NULL;
    cJSON_ArrayForEach(city, cities) {
        if (!cJSON_IsObject(city)) fail("each city must be an object", NULL);

        const char *id = req_str(city, "id", CARMEN_MAX_NAME_LEN, "city");
        if (find_id(ids, ci, id) >= 0) fail("duplicate city id", id);
        ids[ci] = (char *)id;

        (void)req_str(city, "name",       CARMEN_MAX_NAME_LEN, id);
        (void)req_str(city, "local_name", CARMEN_MAX_NAME_LEN, id);
        (void)req_str(city, "country",    CARMEN_MAX_NAME_LEN, id);
        (void)req_str(city, "continent",  CARMEN_MAX_NAME_LEN, id);
        (void)req_str(city, "sphere",     CARMEN_MAX_NAME_LEN, id);
        (void)req_num(city, "lat", id);
        (void)req_num(city, "lon", id);

        cJSON *sites = cJSON_GetObjectItemCaseSensitive(city, "sites");
        if (!cJSON_IsArray(sites)) fail("\"sites\" must be an array", id);
        if (cJSON_GetArraySize(sites) > CARMEN_MAX_SITES)
            fail("too many sites (exceeds CARMEN_MAX_SITES)", id);
        cJSON *site = NULL;
        cJSON_ArrayForEach(site, sites) {
            if (!cJSON_IsObject(site)) fail("each site must be an object", id);
            (void)req_str(site, "id",   CARMEN_MAX_NAME_LEN, id);
            (void)req_str(site, "name", CARMEN_MAX_NAME_LEN, id);
            (void)req_str(site, "type", CARMEN_MAX_NAME_LEN, id);
        }

        cJSON *clues = cJSON_GetObjectItemCaseSensitive(city, "inbound_clues");
        if (!cJSON_IsArray(clues)) fail("\"inbound_clues\" must be an array", id);
        if (cJSON_GetArraySize(clues) > CARMEN_MAX_INBOUND_CLUES)
            fail("too many inbound clues (exceeds CARMEN_MAX_INBOUND_CLUES)", id);
        cJSON *clue = NULL;
        cJSON_ArrayForEach(clue, clues) {
            if (!cJSON_IsString(clue) || clue->valuestring == NULL)
                fail("each inbound clue must be a string", id);
            if (strlen(clue->valuestring) >= CARMEN_MAX_CLUE_LEN)
                fail("inbound clue exceeds CARMEN_MAX_CLUE_LEN", clue->valuestring);
        }
        ci++;
    }

    /* --- validate routes (reference integrity + per-city degree cap) --- */
    cJSON *routes = cJSON_GetObjectItemCaseSensitive(root, "routes");
    if (routes && !cJSON_IsArray(routes))
        fail("\"routes\" must be an array", NULL);
    cJSON *route = NULL;
    cJSON_ArrayForEach(route, routes) {
        if (!cJSON_IsObject(route)) fail("each route must be an object", NULL);
        const char *from = req_str(route, "from", CARMEN_MAX_NAME_LEN, "route");
        const char *to   = req_str(route, "to",   CARMEN_MAX_NAME_LEN, "route");
        (void)req_num(route, "km", "route");
        (void)req_str(route, "mode", CARMEN_MAX_NAME_LEN, "route");

        int fi = find_id(ids, ncities, from);
        int ti = find_id(ids, ncities, to);
        if (fi < 0) fail("route \"from\" names an unknown city", from);
        if (ti < 0) fail("route \"to\" names an unknown city", to);
        if (fi == ti) fail("route connects a city to itself", from);
        if (++deg[fi] > CARMEN_MAX_CONNECTIONS)
            fail("city exceeds CARMEN_MAX_CONNECTIONS", from);
        if (++deg[ti] > CARMEN_MAX_CONNECTIONS)
            fail("city exceeds CARMEN_MAX_CONNECTIONS", to);
    }

    /* --- emit --- */
    FILE *out = fopen(outpath, "wb");
    if (!out) fail("cannot open output for writing", outpath);

    fprintf(out,
        "/* GENERATED FILE -- DO NOT EDIT.\n"
        "   Produced by tools/gen_world.c from the world preset '%s'.\n"
        "   To change the world, edit the preset and rebuild. */\n\n"
        "#include \"carmen/world_islamic.h\"\n"
        "#include \"seed_helpers.h\"\n\n"
        "void carmen_world_build_islamic(CarmenWorld *w)\n"
        "{\n"
        "    if (!w) return;\n"
        "    carmen_world_init(w);\n\n",
        world_id);

    fprintf(out, "    /* ------------------------------------------------------------ cities */\n");
    cJSON_ArrayForEach(city, cities) {
        fprintf(out, "    carmen_world_add_city(w, ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "id")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "name")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "local_name")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "country")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "continent")->valuestring);
        fprintf(out, ", %.17g, %.17g);\n",
                cJSON_GetObjectItemCaseSensitive(city, "lat")->valuedouble,
                cJSON_GetObjectItemCaseSensitive(city, "lon")->valuedouble);
    }

    fprintf(out, "\n    /* ----------------------------------------------------------- spheres */\n");
    cJSON_ArrayForEach(city, cities) {
        fprintf(out, "    set_sphere(w, ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "id")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(city, "sphere")->valuestring);
        fprintf(out, ");\n");
    }

    fprintf(out, "\n    /* ------------------------------------------------------------- sites */\n");
    cJSON_ArrayForEach(city, cities) {
        const char *cid = cJSON_GetObjectItemCaseSensitive(city, "id")->valuestring;
        cJSON *sites = cJSON_GetObjectItemCaseSensitive(city, "sites");
        cJSON *site = NULL;
        cJSON_ArrayForEach(site, sites) {
            fprintf(out, "    add_site(w, ");
            emit_cstr(out, cid);
            fprintf(out, ", ");
            emit_cstr(out, cJSON_GetObjectItemCaseSensitive(site, "id")->valuestring);
            fprintf(out, ", ");
            emit_cstr(out, cJSON_GetObjectItemCaseSensitive(site, "name")->valuestring);
            fprintf(out, ", ");
            emit_cstr(out, cJSON_GetObjectItemCaseSensitive(site, "type")->valuestring);
            fprintf(out, ");\n");
        }
    }

    fprintf(out, "\n    /* --------------------------------------------------- inbound clue pools */\n");
    cJSON_ArrayForEach(city, cities) {
        const char *cid = cJSON_GetObjectItemCaseSensitive(city, "id")->valuestring;
        cJSON *clues = cJSON_GetObjectItemCaseSensitive(city, "inbound_clues");
        int n = cJSON_GetArraySize(clues);
        if (n == 0) continue;
        fprintf(out, "    { const char *k[] = {");
        cJSON *clue = NULL;
        int first = 1;
        cJSON_ArrayForEach(clue, clues) {
            fprintf(out, "%s ", first ? "" : ",");
            emit_cstr(out, clue->valuestring);
            first = 0;
        }
        fprintf(out, " };\n      add_inbound_clues(w, ");
        emit_cstr(out, cid);
        fprintf(out, ", k, %d); }\n", n);
    }

    fprintf(out, "\n    /* ---------------------------------------------------------- connections */\n");
    cJSON_ArrayForEach(route, routes) {
        fprintf(out, "    add_route(w, ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(route, "from")->valuestring);
        fprintf(out, ", ");
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(route, "to")->valuestring);
        fprintf(out, ", %d, ",
                (int)cJSON_GetObjectItemCaseSensitive(route, "km")->valuedouble);
        emit_cstr(out, cJSON_GetObjectItemCaseSensitive(route, "mode")->valuestring);
        fprintf(out, ");\n");
    }

    fprintf(out, "}\n");

    if (fclose(out) != 0) fail("error writing output", outpath);

    free(ids);
    free(deg);
    cJSON_Delete(root);
    free(buf);
    return 0;
}
