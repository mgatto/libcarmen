#include "carmen/carmen.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LINE "============================================================"
#define RULE "------------------------------------------------------------"
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

static void print_bidi(const char *s)
{
    char visual[EXPAND_BUF];
    carmen_utf8_bidi_visual(s, visual, sizeof visual);
    printf("%s", visual);
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
    const char *raw = carmen_i18n_get(i18n, clue->text);
    char expanded[EXPAND_BUF];
    carmen_villain_expand_clue(raw, villain_gender, expanded, sizeof expanded);
    const char *tag = clue->type == CARMEN_CLUE_POSITIVE ? "+" : "-";
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

int main(int argc, char *argv[])
{
  const char *locale = (argc > 1) ? argv[1] : "en";
  const char *settings_path = (argc > 2) ? argv[2] : NULL;
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

  CarmenSession session;
  CarmenCaseSettings settings = carmen_case_settings_default();
  if (settings_path && !carmen_case_settings_load(&settings, settings_path)) {
    fprintf(stderr, "Failed to load settings: %s (using defaults)\n",
            settings_path);
    settings = carmen_case_settings_default();
  }
  if (!carmen_session_start(&session, world, &settings)) {
    fprintf(stderr, "Failed to generate case\n");
    carmen_world_free(world);
    carmen_i18n_free(i18n);
    return 1;
  }

  const CarmenCase *cas = carmen_session_case(&session);
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
         carmen_i18n_get(i18n, cas->difficulty == CARMEN_DIFFICULTY_EASY
                                   ? "ui.difficulty_easy"
                               : cas->difficulty == CARMEN_DIFFICULTY_HARD
                                   ? "ui.difficulty_hard"
                                   : "ui.difficulty_medium"));
  printf("%s\n", LINE);

  /* ── Game loop ───────────────────────────────────────────────── */
  while (carmen_session_status(&session) == CARMEN_STATUS_PLAYING) {
    const CarmenCity *city = carmen_session_current_city(&session);
    if (!city)
      break;

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
    int nactive = carmen_session_active_sites(&session, active,
                                              CARMEN_TRAIL_SITES);

    printf("\n  %s\n", carmen_i18n_get(i18n, "ui.sites_header"));
    if (nactive > 0) {
      for (int s = 0; s < nactive; s++)
        printf("    [%d] %s (%s)\n", s + 1,
               carmen_i18n_get(i18n, city->sites[active[s]].name),
               carmen_i18n_get(i18n, city->sites[active[s]].site_type));
    } else {
      printf("    %s\n", carmen_i18n_get(i18n, "ui.off_trail"));
      for (int s = 0; s < city->site_count; s++)
        printf("    [%d] %s (%s)\n", s + 1,
               carmen_i18n_get(i18n, city->sites[s].name),
               carmen_i18n_get(i18n, city->sites[s].site_type));
    }

    /* Show connections */
    printf("\n  Connections:\n");
    const CarmenConnection *conns[CARMEN_MAX_CONNECTIONS];
    int nconns = carmen_session_connections(&session, conns,
                                            CARMEN_MAX_CONNECTIONS);
    for (int c = 0; c < nconns; c++) {
      CarmenCity *dest = carmen_world_find(world, conns[c]->destination_id);
      if (dest) {
        printf("    - ");
        print_city_name(i18n, dest);
        printf(" (%s, %d km)\n",
               carmen_i18n_get(i18n, conns[c]->transport_mode),
               conns[c]->distance_km);
      }
    }

    print_evidence(i18n, &session);
    print_notebook(i18n, &session, villain->gender);

    int site_limit = nactive > 0 ? nactive : city->site_count;
    printf("\n  ");
    printf(carmen_i18n_get(i18n, "ui.investigate_prompt"), site_limit);
    printf(" ");

    char input[64];
    if (!read_line(input, sizeof input))
      break;

    if (input[0] == 't' || input[0] == 'T') {
      printf("  %s ", carmen_i18n_get(i18n, "ui.travel_prompt"));
      char dest_buf[CARMEN_MAX_NAME_LEN];
      if (!read_line(dest_buf, sizeof dest_buf))
        break;
      for (char *p = dest_buf; *p; p++)
        *p = (char)tolower((unsigned char)*p);

      int result = carmen_session_travel(&session, dest_buf);
      if (result == 0) {
        const CarmenCity *nc = carmen_session_current_city(&session);
        if (nc) {
          printf("  %s ", carmen_i18n_get(i18n, "ui.travel_ok"));
          print_city_name(i18n, nc);
          printf(".\n");
        }
      } else if (result == -2) {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_timeout"));
      } else {
        printf("  %s\n", carmen_i18n_get(i18n, "ui.travel_fail"));
      }
    } else if (input[0] == 'w' || input[0] == 'W') {
      printf("  %s\n", carmen_i18n_get(i18n, "ui.warrant_prompt"));
      int villain_count = carmen_villain_count();
      for (int v = 0; v < villain_count; v++) {
        const FitnaVillain *vp = carmen_villain_at(v);
        printf("    [%2d] %s, a.k.a. \"%s\"\n", v + 1,
               carmen_i18n_get(i18n, vp->name),
               carmen_i18n_get(i18n, vp->alias));
      }
      printf("  > ");
      int choice = read_int();
      int wr = carmen_session_issue_warrant(&session, choice - 1);
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
      case CARMEN_STATUS_WON:
        break;
      case CARMEN_STATUS_LOST_WRONG_ARREST:
        printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_wrong"));
        break;
      case CARMEN_STATUS_LOST_NO_WARRANT:
        printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_no_warrant"));
        break;
      case CARMEN_STATUS_NOT_AT_HIDEOUT:
        printf("  %s\n", carmen_i18n_get(i18n, "ui.arrest_not_here"));
        break;
      default:
        break;
      }
    } else {
      int choice = atoi(input) - 1;
      int real_idx = -1;
      if (nactive > 0 && choice >= 0 && choice < nactive)
        real_idx = active[choice];
      else if (nactive == 0 && choice >= 0 && choice < city->site_count)
        real_idx = choice;

      if (real_idx >= 0) {
        bool could_warrant = carmen_session_can_issue_warrant(&session);
        const CarmenClue *clue =
            carmen_session_investigate(&session, real_idx);
        if (clue) {
          const char *raw = carmen_i18n_get(i18n, clue->text);
          char expanded[EXPAND_BUF];
          carmen_villain_expand_clue(raw, villain->gender, expanded,
                                     sizeof expanded);
          const char *tag = clue->type == CARMEN_CLUE_POSITIVE ? "+" : "-";
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
    printf("  (It was %s, a.k.a. \"%s\")\n",
           carmen_i18n_get(i18n, villain->name),
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
