#ifndef CARMEN_SITE_H
#define CARMEN_SITE_H

#include "clue.h"

/*
 * A site is just a place to investigate.  Clues no longer live on the
 * site: they are drawn at case-generation time from the destination
 * city's inbound clue pool (see city.h / clue.h).
 */
typedef struct {
    char       id[CARMEN_MAX_NAME_LEN];
    char       name[CARMEN_MAX_NAME_LEN];
    char       site_type[CARMEN_MAX_NAME_LEN];
} CarmenSite;

CARMEN_API void              carmen_site_init(CarmenSite *s, const char *id,
                                              const char *name,
                                              const char *site_type);

/* Write "Name (type)" into buf.  Returns the number of characters that
   would have been written (excluding '\0'), like snprintf. */
CARMEN_API int  carmen_site_to_string(const CarmenSite *s,
                                      char *buf, int buf_size);
CARMEN_API void carmen_site_print(const CarmenSite *s);

#endif
