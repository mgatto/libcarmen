#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carmen/settings.h"

/*
 * toml-c is distributed header-only: this single header contains both the
 * declarations and the implementation. It must therefore be included in
 * exactly one translation unit (this one) to avoid duplicate symbols.
 */
#include "toml-c.h"

static int clampi(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int clamp_warn(const char *name, int v, int lo, int hi)
{
    int c = clampi(v, lo, hi);
    if (c != v)
        fprintf(stderr,
                "settings: %s=%d out of range [%d, %d], clamped to %d\n",
                name, v, lo, hi, c);
    return c;
}

static int parse_difficulty(const char *str, CarmenDifficulty *out)
{
    if (strcmp(str, "easy") == 0)   { *out = CARMEN_DIFFICULTY_EASY;   return 1; }
    if (strcmp(str, "medium") == 0) { *out = CARMEN_DIFFICULTY_MEDIUM; return 1; }
    if (strcmp(str, "hard") == 0)   { *out = CARMEN_DIFFICULTY_HARD;   return 1; }
    return 0;
}

/* Clamp every field into its valid range, warning on any adjustment. */
static void settings_clamp(CarmenCaseSettings *s)
{
    if (s->difficulty != CARMEN_DIFFICULTY_EASY &&
        s->difficulty != CARMEN_DIFFICULTY_MEDIUM &&
        s->difficulty != CARMEN_DIFFICULTY_HARD) {
        fprintf(stderr, "settings: invalid difficulty, using medium\n");
        s->difficulty = CARMEN_DIFFICULTY_MEDIUM;
    }

    /* 0 means "derive from difficulty"; any other value is clamped. */
    if (s->trail_length != 0)
        s->trail_length = clamp_warn("trail_length", s->trail_length,
                                     2, CARMEN_MAX_TRAIL);

    /* 0 means "derive"; negatives are meaningless and reset to 0. */
    if (s->time_budget_hrs < 0) {
        fprintf(stderr, "settings: time_budget_hrs=%d invalid, using 0 (derive)\n",
                s->time_budget_hrs);
        s->time_budget_hrs = 0;
    }

    /* 0 means "derive from difficulty"; any other value is clamped. */
    if (s->active_sites_per_city != 0)
        s->active_sites_per_city = clamp_warn("active_sites_per_city",
                                              s->active_sites_per_city,
                                              1, CARMEN_TRAIL_SITES);

    if (s->positive_clues_per_stop != 0) {
        int hi = s->active_sites_per_city != 0
               ? s->active_sites_per_city : CARMEN_TRAIL_SITES;
        s->positive_clues_per_stop = clamp_warn("positive_clues_per_stop",
                                                s->positive_clues_per_stop,
                                                1, hi);
    }

    /* 0 means "unlimited"; negatives are meaningless and reset to 0. */
    if (s->move_limit < 0) {
        fprintf(stderr, "settings: move_limit=%d invalid, using 0 (unlimited)\n",
                s->move_limit);
        s->move_limit = 0;
    }

    /* 0 means "use full CARMEN_MAX_VISITED"; any other value is clamped. */
    if (s->visited_history_size != 0)
        s->visited_history_size = clamp_warn("visited_history_size",
                                             s->visited_history_size,
                                             1, CARMEN_MAX_VISITED);
}

CarmenCaseSettings carmen_case_settings_default(void)
{
    CarmenCaseSettings s;
    s.difficulty             = CARMEN_DIFFICULTY_MEDIUM;
    s.trail_length           = 0;                  /* derive from difficulty */
    s.time_budget_hrs        = 0;                  /* derive from difficulty */
    s.active_sites_per_city  = 0;                  /* derive from difficulty */
    s.positive_clues_per_stop = 0;                 /* derive from difficulty */
    s.move_limit             = 0;                  /* unlimited */
    s.visited_history_size   = CARMEN_MAX_VISITED; /* 24 */
    return s;
}

int carmen_case_settings_load(CarmenCaseSettings *out, const char *toml_path)
{
    if (!out || !toml_path) return 0;

    FILE *f = fopen(toml_path, "rb");
    if (!f) {
        fprintf(stderr, "settings: cannot open %s\n", toml_path);
        return 0;
    }

    char errbuf[256];
    toml_table_t *tbl = toml_parse_file(f, errbuf, (int)sizeof errbuf);
    fclose(f);
    if (!tbl) {
        fprintf(stderr, "settings: parse error in %s: %s\n", toml_path, errbuf);
        return 0;
    }

    CarmenCaseSettings s = carmen_case_settings_default();

    toml_value_t v = toml_table_string(tbl, "difficulty");
    if (v.ok) {
        CarmenDifficulty d;
        if (parse_difficulty(v.u.s, &d))
            s.difficulty = d;
        else
            fprintf(stderr, "settings: unknown difficulty \"%s\", using default\n",
                    v.u.s);
        free(v.u.s);
    }

    v = toml_table_int(tbl, "trail_length");
    if (v.ok) s.trail_length = (int)v.u.i;

    v = toml_table_int(tbl, "time_budget_hrs");
    if (v.ok) s.time_budget_hrs = (int)v.u.i;

    v = toml_table_int(tbl, "active_sites_per_city");
    if (v.ok) s.active_sites_per_city = (int)v.u.i;

    v = toml_table_int(tbl, "positive_clues_per_stop");
    if (v.ok) s.positive_clues_per_stop = (int)v.u.i;

    v = toml_table_int(tbl, "move_limit");
    if (v.ok) s.move_limit = (int)v.u.i;

    v = toml_table_int(tbl, "visited_history_size");
    if (v.ok) s.visited_history_size = (int)v.u.i;

    toml_free(tbl);

    settings_clamp(&s);
    *out = s;
    return 1;
}
