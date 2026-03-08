#include "carmen/carmen.h"
#include "carmen/villain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRAIL_LEN 5
#define MAX_RETRIES 20
#define LINE "============================================================"
#define RULE "------------------------------------------------------------"

/* ── Trail helpers ────────────────────────────────────────────────── */

static int in_trail(const char *trail[], int len, const char *id) {
  for (int i = 0; i < len; i++)
    if (strcmp(trail[i], id) == 0)
      return 1;
  return 0;
}

/* Count how many distinct sites in city have a positive clue → target. */
static int sites_covering(const CarmenCity *city, const char *target) {
  int count = 0;
  for (int s = 0; s < city->site_count; s++) {
    const CarmenSite *site = &city->sites[s];
    for (int c = 0; c < site->clue_count; c++) {
      if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
          strcmp(site->clues[c].target_city_id, target) == 0) {
        count++;
        break;
      }
    }
  }
  return count;
}

/* Collect all sites that have a positive clue pointing to target.
   Returns count written to out[] (up to max). */
static int find_sites_for(const CarmenCity *city, const char *target,
                          const CarmenSite **out, int max) {
  int n = 0;
  for (int s = 0; s < city->site_count && n < max; s++) {
    const CarmenSite *site = &city->sites[s];
    for (int c = 0; c < site->clue_count; c++) {
      if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
          strcmp(site->clues[c].target_city_id, target) == 0) {
        out[n++] = site;
        break;
      }
    }
  }
  return n;
}

/* From a site, pick the positive clue that points to target. */
static const CarmenClue *pick_clue_to(const CarmenSite *site,
                                      const char *target) {
  for (int c = 0; c < site->clue_count; c++)
    if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
        strcmp(site->clues[c].target_city_id, target) == 0)
      return &site->clues[c];
  return NULL;
}

/* From a site, pick a positive clue that does NOT point to target (herring). */
static const CarmenClue *pick_herring_from(const CarmenSite *site,
                                           const char *target) {
  for (int c = 0; c < site->clue_count; c++)
    if (site->clues[c].type == CARMEN_CLUE_POSITIVE &&
        strcmp(site->clues[c].target_city_id, target) != 0)
      return &site->clues[c];
  return NULL;
}

/* From a site, pick its negative clue. */
static const CarmenClue *pick_negative(const CarmenSite *site) {
  for (int c = 0; c < site->clue_count; c++)
    if (site->clues[c].type == CARMEN_CLUE_NEGATIVE)
      return &site->clues[c];
  return NULL;
}

/* Pick a random 3rd site (not s1 or s2) that can provide noise. */
static const CarmenSite *pick_noise_site(const CarmenCity *city,
                                         const CarmenSite *s1,
                                         const CarmenSite *s2) {
  const CarmenSite *pool[CARMEN_MAX_SITES];
  int n = 0;
  for (int s = 0; s < city->site_count; s++) {
    const CarmenSite *site = &city->sites[s];
    if (site != s1 && site != s2)
      pool[n++] = site;
  }
  if (n == 0)
    return NULL;
  return pool[carmen_random() % n];
}

static int build_trail(CarmenWorld *w, const char *trail[]) {
  trail[0] = w->storage[carmen_random() % w->city_count].id;

  for (int i = 1; i < TRAIL_LEN; i++) {
    CarmenCity *cur = carmen_world_find(w, trail[i - 1]);
    if (!cur)
      return i;

    CarmenCity *nbr[CARMEN_MAX_CONNECTIONS];
    int n = carmen_world_destinations_from(w, trail[i - 1], nbr,
                                           CARMEN_MAX_CONNECTIONS);

    /* Prefer neighbors with 2+ sites covering them (for convergence). */
    const char *cand[CARMEN_MAX_CONNECTIONS];
    int nc = 0;
    for (int j = 0; j < n; j++)
      if (!in_trail(trail, i, nbr[j]->id) &&
          sites_covering(cur, nbr[j]->id) >= 2)
        cand[nc++] = nbr[j]->id;

    /* Fallback: any unvisited neighbor with at least 1 clue. */
    if (nc == 0)
      for (int j = 0; j < n; j++)
        if (!in_trail(trail, i, nbr[j]->id) &&
            sites_covering(cur, nbr[j]->id) >= 1)
          cand[nc++] = nbr[j]->id;

    /* Last resort: any unvisited neighbor. */
    if (nc == 0)
      for (int j = 0; j < n; j++)
        if (!in_trail(trail, i, nbr[j]->id))
          cand[nc++] = nbr[j]->id;

    if (nc == 0)
      return i;
    trail[i] = cand[carmen_random() % nc];
  }
  return TRAIL_LEN;
}

static void print_city_name(const CarmenCity *c) {
  printf("%s", c->name);
  if (c->local_name[0] && strcmp(c->name, c->local_name) != 0)
    printf(" (%s)", c->local_name);
}

/* ── Main ─────────────────────────────────────────────────────────── */

int main(void) {
  srand((unsigned)time(NULL));

  CarmenWorld *world = carmen_world_create();
  if (!world) {
    fprintf(stderr, "Failed to allocate world\n");
    return 1;
  }
  carmen_seed_build_islamic_world(world);

  const char *trail[TRAIL_LEN];
  int len = 0;
  for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
    len = build_trail(world, trail);
    if (len == TRAIL_LEN)
      break;
  }
  if (len < TRAIL_LEN) {
    fprintf(stderr, "Could not build a trail of %d cities after %d attempts\n",
            TRAIL_LEN, MAX_RETRIES);
    carmen_world_free(world);
    return 1;
  }

  const FitnaVillain *villain =
      &FITNA_VILLAINS[carmen_random() % FITNA_VILLAIN_COUNT];

  printf("\n%s\n", LINE);
  printf("  CARMEN: THE MODERN MUSLIM TRAVELER\n");
  printf("  INTEL: A FITNA operative has been sighted.\n");
  printf("  Gather clues and identify the suspect.\n");
  printf("%s\n\n", LINE);

  printf("  Route: ");
  for (int i = 0; i < TRAIL_LEN; i++) {
    CarmenCity *c = carmen_world_find(world, trail[i]);
    if (i > 0)
      printf(" --> ");
    if (i == TRAIL_LEN - 1)
      printf("[%s]", c->name);
    else
      printf("%s", c->name);
  }
  printf("\n\n");

  for (int i = 0; i < TRAIL_LEN; i++) {
    CarmenCity *city = carmen_world_find(world, trail[i]);
    if (!city)
      continue;

    printf("%s\n", RULE);
    printf("  [%d/%d]  ", i + 1, TRAIL_LEN);
    print_city_name(city);
    printf("\n");
    printf("         %s, %s\n", city->country, city->continent);
    printf("%s\n\n", RULE);

    if (i == TRAIL_LEN - 1) {
      printf("  *** The suspect was found hiding here! ***\n\n");
      break;
    }

    const char *next_id = trail[i + 1];
    CarmenCity *next = carmen_world_find(world, next_id);

    /* Find 2 sites whose positive clues converge on next_id. */
    const CarmenSite *correct_sites[CARMEN_MAX_SITES];
    int ncorrect =
        find_sites_for(city, next_id, correct_sites, CARMEN_MAX_SITES);

    const CarmenSite *s1 = NULL, *s2 = NULL;
    const CarmenClue *c1 = NULL, *c2 = NULL;

    if (ncorrect >= 2) {
      int a = carmen_random() % ncorrect;
      int b;
      do {
        b = carmen_random() % ncorrect;
      } while (b == a);
      s1 = correct_sites[a];
      s2 = correct_sites[b];
      c1 = pick_clue_to(s1, next_id);
      c2 = pick_clue_to(s2, next_id);
    } else if (ncorrect == 1) {
      s1 = correct_sites[0];
      c1 = pick_clue_to(s1, next_id);
    }

    /* Pick a 3rd site for noise (red herring or dead end). */
    const CarmenSite *s3 = pick_noise_site(city, s1, s2);
    const CarmenClue *noise = NULL;
    const char *noise_tag = NULL;

    if (s3) {
      int use_herring = carmen_random() % 2;
      if (use_herring) {
        noise = pick_herring_from(s3, next_id);
        noise_tag = "?";
      }
      if (!noise) {
        noise = pick_negative(s3);
        noise_tag = "-";
      }
    }

    /* Build presentation slots and shuffle with Fisher-Yates. */
    typedef struct {
      const CarmenClue *clue;
      const CarmenSite *site;
      const char *tag;
    } Slot;

    Slot slots[3];
    int ns = 0;
    if (c1 && s1) {
      slots[ns].clue = c1;
      slots[ns].site = s1;
      slots[ns].tag = "+";
      ns++;
    }
    if (c2 && s2) {
      slots[ns].clue = c2;
      slots[ns].site = s2;
      slots[ns].tag = "+";
      ns++;
    }
    if (noise && s3) {
      slots[ns].clue = noise;
      slots[ns].site = s3;
      slots[ns].tag = noise_tag;
      ns++;
    }

    for (int j = ns - 1; j > 0; j--) {
      int k = carmen_random() % (j + 1);
      Slot tmp = slots[j];
      slots[j] = slots[k];
      slots[k] = tmp;
    }

    for (int j = 0; j < ns; j++) {
      char expanded[CARMEN_MAX_CLUE_LEN];
      fitna_expand_clue(slots[j].clue->text, villain->gender, expanded,
                        sizeof expanded);
      printf("  * %s (%s)\n", slots[j].site->name, slots[j].site->site_type);
      printf("    [%s] \"%s\"\n\n", slots[j].tag, expanded);
    }

    if (next) {
      printf("  >>> The clues point toward ");
      print_city_name(next);
      printf("...\n\n");
    }
  }

  printf("%s\n", LINE);
  printf("  Case closed.\n");
  printf("  (It was %s, a.k.a. \"%s\")\n", villain->name, villain->alias);
  printf("%s\n\n", LINE);

  carmen_world_free(world);
  return 0;
}
