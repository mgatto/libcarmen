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
