#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carmen/i18n.h"
#include "stb_ds.h"
#include "cJSON.h"

typedef struct {
    char *key;
    char *value;
} I18nEntry;

struct CarmenI18n {
    I18nEntry *map;
};

static int valid_i18n_key(const char *key)
{
    for (const char *p = key; *p; p++)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') ||
              *p == '.' || *p == '_'))
            return 0;
    return key[0] != '\0';
}

CarmenI18n *carmen_i18n_load(const char *json_path)
{
    if (!json_path) return NULL;

    FILE *f = fopen(json_path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (len <= 0 || (size_t)len > CARMEN_I18N_MAX_FILE_SIZE) {
        fclose(f);
        return NULL;
    }

    char *buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t nread = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[nread] = '\0';

    CarmenI18n *ctx = carmen_i18n_load_json(buf, nread);
    free(buf);
    return ctx;
}

CarmenI18n *carmen_i18n_load_json(const char *json, size_t len)
{
    if (!json) return NULL;

    cJSON *root = cJSON_ParseWithLength(json, len);
    if (!root || !cJSON_IsObject(root)) {
        cJSON_Delete(root);
        return NULL;
    }

    CarmenI18n *ctx = malloc(sizeof(*ctx));
    if (!ctx) { cJSON_Delete(root); return NULL; }
    ctx->map = NULL;
    sh_new_strdup(ctx->map);

    int entry_count = 0;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, root) {
        if (entry_count >= CARMEN_I18N_MAX_ENTRIES) {
            fprintf(stderr, "i18n: entry limit reached (%d), remaining keys ignored\n",
                    CARMEN_I18N_MAX_ENTRIES);
            break;
        }
        if (!item->string || item->string[0] == '_' || !cJSON_IsString(item))
            continue;

        size_t klen = strlen(item->string);
        size_t vlen = strlen(item->valuestring);

        if (klen >= CARMEN_I18N_MAX_KEY_LEN) {
            fprintf(stderr, "i18n: skipping oversized key \"%.*s...\" (%zu bytes)\n",
                    20, item->string, klen);
            continue;
        }
        if (!valid_i18n_key(item->string))
            continue;
        if (vlen >= CARMEN_I18N_MAX_VALUE_LEN) {
            fprintf(stderr, "i18n: skipping oversized value for key \"%s\" (%zu bytes)\n",
                    item->string, vlen);
            continue;
        }

        char *dup = strdup(item->valuestring);
        if (!dup) continue;
        shput(ctx->map, item->string, dup);
        entry_count++;
    }

    cJSON_Delete(root);
    return ctx;
}

const char *carmen_i18n_get(const CarmenI18n *ctx, const char *key)
{
    if (!key) return "";
    if (!ctx) return key;

    I18nEntry *map = ctx->map;
    ptrdiff_t idx = shgeti(map, key);
    if (idx < 0) return key;
    return map[idx].value;
}

void carmen_i18n_free(CarmenI18n *ctx)
{
    if (!ctx) return;
    for (size_t i = 0; i < shlenu(ctx->map); i++)
        free(ctx->map[i].value);
    shfree(ctx->map);
    free(ctx);
}
