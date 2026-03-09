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

CarmenI18n *carmen_i18n_load(const char *json_path)
{
    if (!json_path) return NULL;

    FILE *f = fopen(json_path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (len <= 0) { fclose(f); return NULL; }

    char *buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t read = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[read] = '\0';

    cJSON *root = cJSON_Parse(buf);
    free(buf);
    if (!root) return NULL;

    CarmenI18n *ctx = malloc(sizeof(*ctx));
    if (!ctx) { cJSON_Delete(root); return NULL; }
    ctx->map = NULL;
    sh_new_strdup(ctx->map);

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, root) {
        if (item->string && item->string[0] != '_' && cJSON_IsString(item)) {
            char *dup = strdup(item->valuestring);
            shput(ctx->map, item->string, dup);
        }
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
