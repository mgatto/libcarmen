#include "carmen/carmen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LINE       "============================================================"
#define RULE       "------------------------------------------------------------"
#define EXPAND_BUF 512

/* ── Helpers ─────────────────────────────────────────────────────── */

static int valid_locale_id(const char *id)
{
    for (const char *p = id; *p; p++)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') ||
              *p == '_' || *p == '-'))
            return 0;
    return id[0] != '\0';
}

/* How Arabic/RTL text is emitted to the terminal.
 *
 * BIDI_LOGICAL: print the raw logical-order UTF-8 and let the terminal do its
 *   own bidi reordering and Arabic shaping (correct for Terminal.app, VTE, ...).
 * BIDI_VISUAL:  pre-reorder to visual order and shape to presentation forms via
 *   carmen_utf8_bidi_visual, for terminals that do NOT do their own bidi
 *   (xterm.js/VSCode, iTerm2, legacy Windows consoles).
 *
 * There is no portable way to query a terminal for bidi support, so the mode is
 * chosen by --bidi / CARMEN_BIDI / an env heuristic; see resolve_bidi_mode. */
typedef enum { BIDI_LOGICAL, BIDI_VISUAL } BidiMode;

static BidiMode g_bidi_mode = BIDI_LOGICAL;

/* Map a mode string. Returns 1 on a recognized value (LOGICAL/VISUAL written to
 * *out; "auto" sets *is_auto). Returns 0 for NULL/unknown values. */
static int parse_bidi_mode(const char *s, BidiMode *out, int *is_auto)
{
    *is_auto = 0;
    if (!s) return 0;
    if (strcmp(s, "logical") == 0) {
        *out = BIDI_LOGICAL;
        return 1;
    }
    if (strcmp(s, "visual") == 0) {
        *out = BIDI_VISUAL;
        return 1;
    }
    if (strcmp(s, "auto") == 0) {
        *is_auto = 1;
        return 1;
    }
    return 0;
}

/* Best-effort guess from the environment. There is no capability query for bidi,
 * so we key on emulator-specific variables and default to logical (the
 * Unicode-correct byte stream, safe for pipes/copy-paste). */
static BidiMode detect_bidi_auto(void)
{
    const char *term_program = getenv("TERM_PROGRAM");
    if (term_program) {
        if (strcmp(term_program, "Apple_Terminal") == 0) return BIDI_LOGICAL;
        if (strcmp(term_program, "vscode") == 0) return BIDI_VISUAL;
        if (strcmp(term_program, "iTerm.app") == 0) return BIDI_VISUAL;
    }
    if (getenv("WT_SESSION")) return BIDI_VISUAL;
    if (getenv("VTE_VERSION")) return BIDI_LOGICAL;
#ifdef _WIN32
    return BIDI_VISUAL;
#else
    return BIDI_LOGICAL;
#endif
}

/* Resolve the effective mode: --bidi=<mode> flag beats the CARMEN_BIDI env var,
 * which beats the auto heuristic; "auto" (or an invalid value, with a warning)
 * runs the heuristic. cli_value is the --bidi argument if present, else NULL. */
static BidiMode resolve_bidi_mode(const char *cli_value)
{
    BidiMode mode    = BIDI_LOGICAL;
    int      is_auto = 0;

    if (cli_value) {
        if (parse_bidi_mode(cli_value, &mode, &is_auto)) return is_auto ? detect_bidi_auto() : mode;
        fprintf(stderr, "Invalid --bidi value: %s (using auto)\n", cli_value);
        return detect_bidi_auto();
    }

    const char *env = getenv("CARMEN_BIDI");
    if (env && env[0]) {
        if (parse_bidi_mode(env, &mode, &is_auto)) return is_auto ? detect_bidi_auto() : mode;
        fprintf(stderr, "Invalid CARMEN_BIDI value: %s (using auto)\n", env);
        return detect_bidi_auto();
    }

    return detect_bidi_auto();
}

static void print_bidi(const char *s)
{
    if (g_bidi_mode == BIDI_VISUAL) {
        char visual[EXPAND_BUF];
        carmen_utf8_bidi_visual(s, visual, sizeof visual);
        printf("%s", visual);
    } else {
        printf("%s", s);
    }
}

static void print_city_name(const CarmenI18n *i18n, const CarmenCity *c)
{
    const char *name  = carmen_i18n_get(i18n, c->name);
    const char *local = carmen_i18n_get(i18n, c->local_name);
    char        composed[EXPAND_BUF];
    if (local[0] && strcmp(name, local) != 0)
        snprintf(composed, sizeof composed, "%s (%s)", name, local);
    else
        snprintf(composed, sizeof composed, "%s", name);
    print_bidi(composed);
}

static void print_status_bar(const CarmenI18n *i18n, const CarmenSession *s)
{
    printf("  %s %d %s  |  %s %d\n", carmen_i18n_get(i18n, "ui.time_left"),
           carmen_session_time_remaining(s), carmen_i18n_get(i18n, "ui.hours"),
           carmen_i18n_get(i18n, "ui.moves"), carmen_session_moves(s));
}

static void print_evidence(const CarmenI18n *i18n, const CarmenSession *s)
{
    int count = carmen_session_evidence_count(s);
    if (count == 0) return;
    int required = carmen_session_evidence_required(s);
    printf("\n  ");
    printf(carmen_i18n_get(i18n, "ui.evidence_header"), count, required);
    printf("\n");
    const FitnaVillain *villain = carmen_session_villain(s);
    for (int i = 0; i < count; i++) {
        char expanded[EXPAND_BUF];
        carmen_villain_expand_clue(carmen_i18n_get(i18n, carmen_session_evidence_at(s, i)),
                                   villain->gender, expanded, sizeof expanded);
        printf("    %d. %s\n", i + 1, expanded);
    }
}

static void print_notebook(const CarmenI18n *i18n, const CarmenSession *s, char villain_gender)
{
    int count = carmen_session_notebook_count(s);
    if (count == 0) return;
    printf("\n  %s (%d)\n", carmen_i18n_get(i18n, "ui.notebook_header"), count);
    for (int i = 0; i < count; i++) {
        const CarmenClue *clue = carmen_session_notebook_at(s, i);
        const char       *raw  = carmen_i18n_get(i18n, clue->text);
        char              expanded[EXPAND_BUF];
        carmen_villain_expand_clue(raw, villain_gender, expanded, sizeof expanded);
        const char *tag = clue->type == CARMEN_CLUE_POSITIVE ? "+"
                        : clue->type == CARMEN_CLUE_IDENTITY ? "*"
                                                             : "-";
        printf("    [%s] \"%s\"\n", tag, expanded);
    }
}

static void print_route(const CarmenI18n *i18n, const CarmenSession *s, CarmenWorld *world)
{
    int count = carmen_session_visited_count(s);
    if (count <= 1) return;
    printf("\n  %s ", carmen_i18n_get(i18n, "ui.route"));
    for (int i = 0; i < count; i++) {
        const char       *id = carmen_session_visited_at(s, i);
        const CarmenCity *c  = carmen_world_find(world, id);
        printf("%s", c ? carmen_i18n_get(i18n, c->name) : id);
        if (i < count - 1) printf(" -> ");
    }
    printf("\n");
}

static int read_int(void)
{
    char buf[64];
    if (!fgets(buf, sizeof buf, stdin)) return -1;
    return atoi(buf);
}

static int read_line(char *buf, int size)
{
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

/* Read a file path, substituting def when the user enters an empty line. */
static void read_path_with_default(char *buf, int size, const char *def)
{
    read_line(buf, size);
    if (buf[0] == '\0') snprintf(buf, (size_t)size, "%s", def);
}

/* ── Main ─────────────────────────────────────────────────────────── */

static void print_usage(const char *prog)
{
    printf("Usage: %s [locale] [settings.toml] [--bidi=logical|visual|auto]\n\n", prog);
    printf("Positional arguments:\n");
    printf("  locale          locale id (default: en); loads locales/<locale>.json\n");
    printf("  settings.toml   path to a game settings file (default: built-in defaults)\n\n");
    printf("Options:\n");
    printf("  --bidi=MODE     Arabic/RTL rendering mode (default: auto):\n");
    printf("                    logical  emit raw logical order; the terminal reorders\n");
    printf("                             and shapes (Terminal.app, GNOME Terminal/VTE)\n");
    printf("                    visual   pre-reorder and shape for terminals that don't\n");
    printf("                             (VSCode, iTerm2, legacy Windows consoles)\n");
    printf("                    auto     guess from the environment (see below)\n");
    printf("  --help, -h      show this help and exit\n\n");
    printf("The CARMEN_BIDI environment variable accepts the same values and is used\n");
    printf("when --bidi is absent. Precedence: --bidi > CARMEN_BIDI > auto heuristic.\n");
}

int main(int argc, char *argv[])
{
    const char *locale        = NULL;
    const char *settings_path = NULL;
    const char *bidi_value    = NULL;
    int         positional    = 0;
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strncmp(arg, "--bidi=", 7) == 0) {
            bidi_value = arg + 7;
            continue;
        }
        if (arg[0] == '-' && arg[1] != '\0') {
            fprintf(stderr, "Unknown option: %s\n", arg);
            print_usage(argv[0]);
            return 1;
        }
        if (positional == 0)
            locale = arg;
        else if (positional == 1)
            settings_path = arg;
        else {
            fprintf(stderr, "Unexpected argument: %s\n", arg);
            print_usage(argv[0]);
            return 1;
        }
        positional++;
    }
    if (!locale) locale = "en";

    g_bidi_mode = resolve_bidi_mode(bidi_value);

    if (!valid_locale_id(locale)) {
        fprintf(stderr, "Invalid locale id: %s\n", locale);
        return 1;
    }
    char path[256];
    snprintf(path, sizeof path, "locales/%s.json", locale);

    CarmenI18n *i18n = carmen_i18n_load(path);
    if (!i18n) {
        fprintf(stderr, "Failed to load locale: %s\n", path);
        return 1;
    }

    srand((unsigned)time(NULL));

    CarmenWorld *world = carmen_world_create();
    if (!world) {
        fprintf(stderr, "Failed to allocate world\n");
        carmen_i18n_free(i18n);
        return 1;
    }
    carmen_world_build_islamic(world);

    CarmenSession      session;
    CarmenCaseSettings settings = carmen_case_settings_default();
    if (settings_path && !carmen_case_settings_load(&settings, settings_path)) {
        fprintf(stderr, "Failed to load settings: %s (using defaults)\n", settings_path);
        settings = carmen_case_settings_default();
    }
    if (!carmen_session_start(&session, world, &settings)) {
        fprintf(stderr, "Failed to generate case\n");
        carmen_world_free(world);
        carmen_i18n_free(i18n);
        return 1;
    }

    const CarmenCase   *cas     = carmen_session_case(&session);
    const FitnaVillain *villain = carmen_case_villain(cas);

    /* ── Banner ──────────────────────────────────────────────────── */
    printf("\n%s\n", LINE);
    printf("  %s\n", carmen_i18n_get(i18n, "ui.title"));
    printf("  %s\n", carmen_i18n_get(i18n, "ui.intel"));
    printf("  %s\n", carmen_i18n_get(i18n, "ui.instruction"));
    char briefing[EXPAND_BUF];
    carmen_case_briefing_text(cas, world, i18n, briefing, sizeof briefing);
    printf("\n  %s\n", briefing);
    printf("  [%s]\n",
           carmen_i18n_get(i18n, cas->difficulty == CARMEN_DIFFICULTY_EASY ? "ui.difficulty_easy"
                                 : cas->difficulty == CARMEN_DIFFICULTY_HARD
                                     ? "ui.difficulty_hard"
                                     : "ui.difficulty_medium"));
    printf("%s\n", LINE);

    /* ── Game loop ───────────────────────────────────────────────── */
    while (carmen_session_status(&session) == CARMEN_STATUS_PLAYING) {
        const CarmenCity *city = carmen_session_current_city(&session);
        if (!city) break;

        printf("\n%s\n", RULE);
        printf("  ");
        print_city_name(i18n, city);
        printf("\n  %s, %s\n", carmen_i18n_get(i18n, city->country),
               carmen_i18n_get(i18n, city->continent));
        print_status_bar(i18n, &session);
        printf("%s\n", RULE);

        if (strcmp(city->id, cas->hideout_id) == 0)
            printf("\n  %s\n", carmen_i18n_get(i18n, "ui.hideout_reached"));

        int active[CARMEN_TRAIL_SITES];
        int nactive = carmen_session_active_sites(&session, active, CARMEN_TRAIL_SITES);

        printf("\n  %s\n", carmen_i18n_get(i18n, "ui.sites_header"));
        if (nactive > 0) {
            for (int s = 0; s < nactive; s++)
                printf("    [%d] %s (%s)\n", s + 1,
                       carmen_i18n_get(i18n, city->sites[active[s]].name),
                       carmen_i18n_get(i18n, city->sites[active[s]].site_type));
        } else {
            printf("    %s\n", carmen_i18n_get(i18n, "ui.off_trail"));
            for (int s = 0; s < city->site_count; s++)
                printf("    [%d] %s (%s)\n", s + 1, carmen_i18n_get(i18n, city->sites[s].name),
                       carmen_i18n_get(i18n, city->sites[s].site_type));
        }

        /* Show connections */
        printf("\n  %s\n", carmen_i18n_get(i18n, "ui.connections_header"));
        const CarmenConnection *conns[CARMEN_MAX_CONNECTIONS];
        int nconns = carmen_session_connections(&session, conns, CARMEN_MAX_CONNECTIONS);
        for (int c = 0; c < nconns; c++) {
            CarmenCity *dest = carmen_world_find(world, conns[c]->destination_id);
            if (dest) {
                char        city_part[EXPAND_BUF];
                char        line[EXPAND_BUF];
                const char *name  = carmen_i18n_get(i18n, dest->name);
                const char *local = carmen_i18n_get(i18n, dest->local_name);
                if (local[0] && strcmp(name, local) != 0)
                    snprintf(city_part, sizeof city_part, "%s (%s)", name, local);
                else
                    snprintf(city_part, sizeof city_part, "%s", name);
                snprintf(line, sizeof line, "    [%d] %s (%s, %d km)", c + 1, city_part,
                         carmen_i18n_get(i18n, conns[c]->transport_mode), conns[c]->distance_km);
                print_bidi(line);
                printf("\n");
            }
        }

        print_route(i18n, &session, world);
        print_evidence(i18n, &session);
        print_notebook(i18n, &session, villain->gender);

        int site_limit = nactive > 0 ? nactive : city->site_count;
        printf("\n  ");
        printf(carmen_i18n_get(i18n, "ui.investigate_prompt"), site_limit);
        printf(" ");

        char input[64];
        if (!read_line(input, sizeof input)) break;

        if (input[0] == 't' || input[0] == 'T') {
            if (nconns == 0) {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_fail"));
                continue;
            }
            printf("  ");
            printf(carmen_i18n_get(i18n, "ui.travel_prompt"), nconns);
            int choice = read_int();
            if (choice < 1 || choice > nconns) {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_cancel"));
                continue;
            }

            int result = carmen_session_travel(&session, conns[choice - 1]->destination_id);
            if (result == 0) {
                const CarmenCity *nc = carmen_session_current_city(&session);
                if (nc) {
                    printf("  %s ", carmen_i18n_get(i18n, "ui.travel_ok"));
                    print_city_name(i18n, nc);
                    printf(".\n");
                }
            } else if (result == -2) {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_timeout"));
            } else if (result == -4) {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.lost_moves"));
            } else {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_fail"));
            }
        } else if (input[0] == 'w' || input[0] == 'W') {
            int villain_count = carmen_villain_count();
            for (int v = 0; v < villain_count; v++) {
                const FitnaVillain *vp = carmen_villain_at(v);
                printf("    [%2d] %s, a.k.a. \"%s\"\n", v + 1, carmen_i18n_get(i18n, vp->name),
                       carmen_i18n_get(i18n, vp->alias));
            }
            printf("  ");
            printf(carmen_i18n_get(i18n, "ui.warrant_prompt"), villain_count);
            printf(" ");
            int choice = read_int();
            int wr     = carmen_session_issue_warrant(&session, choice - 1);
            if (wr == 0) {
                const FitnaVillain *chosen = carmen_villain_at(choice - 1);
                printf("  %s %s.\n", carmen_i18n_get(i18n, "ui.warrant_ok"),
                       carmen_i18n_get(i18n, chosen ? chosen->name : ""));
            } else if (wr == -2) {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.warrant_need_evidence"));
            } else {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.warrant_fail"));
            }
        } else if (input[0] == 'a' || input[0] == 'A') {
            CarmenSessionStatus result = carmen_session_arrest(&session);
            switch (result) {
                case CARMEN_STATUS_WON: break;
                case CARMEN_STATUS_LOST_WRONG_ARREST:
                    printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_wrong"));
                    break;
                case CARMEN_STATUS_LOST_NO_WARRANT:
                    printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_no_warrant"));
                    break;
                case CARMEN_STATUS_NOT_AT_HIDEOUT:
                    printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_not_here"));
                    break;
                default: break;
            }
        } else if (input[0] == 's' || input[0] == 'S') {
            printf("  %s ", carmen_i18n_get(i18n, "ui.save_prompt"));
            char file_buf[256];
            read_path_with_default(file_buf, sizeof file_buf, "savegame.json");
            if (carmen_session_save_file(&session, file_buf) == 1)
                printf("  %s %s\n", carmen_i18n_get(i18n, "ui.save_ok"), file_buf);
            else
                printf("  %s\n", carmen_i18n_get(i18n, "ui.save_fail"));
        } else if (input[0] == 'l' || input[0] == 'L') {
            printf("  %s ", carmen_i18n_get(i18n, "ui.load_prompt"));
            char file_buf[256];
            read_path_with_default(file_buf, sizeof file_buf, "savegame.json");
            if (carmen_session_load_file(&session, world, file_buf) == 1) {
                cas     = carmen_session_case(&session);
                villain = carmen_session_villain(&session);
                printf("  %s\n", carmen_i18n_get(i18n, "ui.load_ok"));
            } else {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.load_fail"));
            }
        } else if (input[0] == 'r' || input[0] == 'R') {
            if (carmen_session_reset(&session)) {
                cas     = carmen_session_case(&session);
                villain = carmen_session_villain(&session);
                printf("  %s\n", carmen_i18n_get(i18n, "ui.restart_ok"));
            } else {
                printf("  %s\n", carmen_i18n_get(i18n, "ui.restart_fail"));
            }
        } else {
            int choice   = atoi(input) - 1;
            int real_idx = -1;
            if (nactive > 0 && choice >= 0 && choice < nactive)
                real_idx = active[choice];
            else if (nactive == 0 && choice >= 0 && choice < city->site_count)
                real_idx = choice;

            if (real_idx >= 0) {
                bool              could_warrant = carmen_session_can_issue_warrant(&session);
                const CarmenClue *clue          = carmen_session_investigate(&session, real_idx);
                if (clue) {
                    const char *raw = carmen_i18n_get(i18n, clue->text);
                    char        expanded[EXPAND_BUF];
                    carmen_villain_expand_clue(raw, villain->gender, expanded, sizeof expanded);
                    const char *tag = clue->type == CARMEN_CLUE_POSITIVE ? "+"
                                    : clue->type == CARMEN_CLUE_IDENTITY ? "*"
                                                                         : "-";
                    printf("\n    [%s] \"%s\"\n", tag, expanded);
                    if (!could_warrant && carmen_session_can_issue_warrant(&session))
                        printf("\n  %s\n", carmen_i18n_get(i18n, "ui.warrant_ready"));
                } else {
                    printf("  %s\n", carmen_i18n_get(i18n, "ui.no_clues"));
                }
            }
        }
    }

    /* ── Endgame ─────────────────────────────────────────────────── */
    printf("\n%s\n", LINE);
    CarmenSessionStatus final = carmen_session_status(&session);
    if (final == CARMEN_STATUS_WON) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.won"));
        printf("  (It was %s, a.k.a. \"%s\")\n", carmen_i18n_get(i18n, villain->name),
               carmen_i18n_get(i18n, villain->alias));
        printf("  ");
        printf(carmen_i18n_get(i18n, "ui.score"), carmen_session_score(&session));
        printf("\n");
    } else if (final == CARMEN_STATUS_LOST_TIME) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.lost_time"));
    } else if (final == CARMEN_STATUS_LOST_MOVES) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.lost_moves"));
    } else if (final == CARMEN_STATUS_LOST_WRONG_ARREST) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_wrong"));
    } else if (final == CARMEN_STATUS_LOST_NO_WARRANT) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_no_warrant"));
    }
    printf("%s\n\n", LINE);

    carmen_world_free(world);
    carmen_i18n_free(i18n);
    return 0;
}
