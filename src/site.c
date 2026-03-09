#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carmen/site.h"
#include "carmen/utf8.h"

static carmen_rand_fn g_rand_fn  = NULL;
static void          *g_rand_ctx = NULL;

void carmen_set_rand(carmen_rand_fn fn, void *ctx)
{
    g_rand_fn  = fn;
    g_rand_ctx = ctx;
}

int carmen_random(void)
{
    if (g_rand_fn)
        return g_rand_fn(g_rand_ctx);
    return rand();
}

void carmen_site_init(CarmenSite *s, const char *id,
                      const char *name, const char *site_type)
{
    if (!s || !id || !name || !site_type) return;
    memset(s, 0, sizeof(*s));
    carmen_utf8_copy(s->id, CARMEN_MAX_NAME_LEN, id);
    carmen_utf8_copy(s->name, CARMEN_MAX_NAME_LEN, name);
    carmen_utf8_copy(s->site_type, CARMEN_MAX_NAME_LEN, site_type);
}

void carmen_site_add_clue(CarmenSite *s, const char *text,
                          const char *target_city_id, CarmenClueType type)
{
    if (!s || !text) return;
    if (s->clue_count < CARMEN_MAX_CLUES) {
        CarmenClue *c = &s->clues[s->clue_count];
        memset(c, 0, sizeof(*c));
        carmen_utf8_copy(c->text, CARMEN_MAX_CLUE_LEN, text);
        if (target_city_id)
            carmen_utf8_copy(c->target_city_id, CARMEN_MAX_NAME_LEN,
                             target_city_id);
        c->type = type;
        s->clue_count++;
    }
}

const CarmenClue *carmen_site_random_clue(const CarmenSite *s)
{
    if (!s || s->clue_count == 0)
        return NULL;
    return &s->clues[carmen_random() % s->clue_count];
}

int carmen_site_to_string(const CarmenSite *s, char *buf, int buf_size)
{
    if (!s) return 0;
    if (!buf || buf_size <= 0)
        return snprintf(NULL, 0, "%s (%s)", s->name, s->site_type);
    return snprintf(buf, (size_t)buf_size, "%s (%s)", s->name, s->site_type);
}

void carmen_site_print(const CarmenSite *s)
{
    if (!s) return;
    char buf[CARMEN_MAX_NAME_LEN * 2 + 4];
    carmen_site_to_string(s, buf, (int)sizeof(buf));
    fputs(buf, stdout);
}
