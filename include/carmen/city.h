#ifndef CARMEN_CITY_H
#define CARMEN_CITY_H

#include "carmen/carmen_export.h"
#include "carmen/site.h"
#include "carmen/connection.h"

#define CARMEN_MAX_SITES       6
#define CARMEN_MAX_CONNECTIONS 10

typedef struct {
    char             id[CARMEN_MAX_NAME_LEN];
    char             name[CARMEN_MAX_NAME_LEN];
    char             local_name[CARMEN_MAX_NAME_LEN];
    char             country[CARMEN_MAX_NAME_LEN];
    char             continent[CARMEN_MAX_NAME_LEN];
    double           latitude;
    double           longitude;
    CarmenSite       sites[CARMEN_MAX_SITES];
    int              site_count;
    CarmenConnection connections[CARMEN_MAX_CONNECTIONS];
    int              connection_count;
} CarmenCity;

CARMEN_API void              carmen_city_init(CarmenCity *c, const char *id,
                                              const char *name,
                                              const char *local_name,
                                              const char *country,
                                              const char *continent,
                                              double latitude,
                                              double longitude);
CARMEN_API void              carmen_city_add_site(CarmenCity *c,
                                                  const CarmenSite *s);
CARMEN_API void              carmen_city_add_connection(
                                  CarmenCity *c,
                                  const CarmenConnection *conn);
CARMEN_API int               carmen_city_has_connection_to(
                                  const CarmenCity *c,
                                  const char *dest_id);
CARMEN_API int               carmen_city_sites_of_type(
                                  const CarmenCity *c, const char *type,
                                  const CarmenSite **out, int max_out);
CARMEN_API const CarmenClue *carmen_city_random_clue(const CarmenCity *c);

/* Write "Name, Country (Continent)" into buf. */
CARMEN_API int  carmen_city_to_string(const CarmenCity *c,
                                      char *buf, int buf_size);
/* Write "<City:id Name, Country (Continent) sites=N connections=N>" into buf. */
CARMEN_API int  carmen_city_detail_to_string(const CarmenCity *c,
                                             char *buf, int buf_size);
CARMEN_API void carmen_city_print(const CarmenCity *c);
CARMEN_API void carmen_city_print_detail(const CarmenCity *c);

#endif
