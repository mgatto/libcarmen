#ifndef CARMEN_SITE_H
#define CARMEN_SITE_H

#include "clue.h"

typedef struct {
    char       id[CARMEN_MAX_NAME_LEN];
    char       name[CARMEN_MAX_NAME_LEN];
    char       site_type[CARMEN_MAX_NAME_LEN];
    CarmenClue clues[CARMEN_MAX_CLUES];
    int        clue_count;
} CarmenSite;

CARMEN_API void              carmen_site_init(CarmenSite *s, const char *id,
                                              const char *name,
                                              const char *site_type);
CARMEN_API void              carmen_site_add_clue(CarmenSite *s,
                                                  const char *text,
                                                  const char *target_city_id,
                                                  CarmenClueType type);
CARMEN_API const CarmenClue *carmen_site_random_clue(const CarmenSite *s);

/* Write "Name (type)" into buf.  Returns the number of characters that
   would have been written (excluding '\0'), like snprintf. */
CARMEN_API int  carmen_site_to_string(const CarmenSite *s,
                                      char *buf, int buf_size);
CARMEN_API void carmen_site_print(const CarmenSite *s);

#endif
