#include <stdio.h>
#include <string.h>
#include "carmen/connection.h"

void carmen_connection_init(CarmenConnection *c, const char *dest_id,
                            int distance_km, const char *transport_mode)
{
    if (!c || !dest_id) return;
    memset(c, 0, sizeof(*c));
    snprintf(c->destination_id, CARMEN_MAX_NAME_LEN, "%s", dest_id);
    c->distance_km = distance_km;
    snprintf(c->transport_mode, CARMEN_MAX_NAME_LEN, "%s",
             transport_mode ? transport_mode : "flight");
}

int carmen_connection_to_string(const CarmenConnection *c,
                                char *buf, int buf_size)
{
    if (!c) return 0;
    if (!buf || buf_size <= 0)
        return snprintf(NULL, 0, "-> %s via %s (%d km)",
                        c->destination_id, c->transport_mode, c->distance_km);
    return snprintf(buf, (size_t)buf_size, "-> %s via %s (%d km)",
                    c->destination_id, c->transport_mode, c->distance_km);
}

void carmen_connection_print(const CarmenConnection *c)
{
    if (!c) return;
    char buf[CARMEN_MAX_NAME_LEN * 2 + 32];
    carmen_connection_to_string(c, buf, (int)sizeof(buf));
    fputs(buf, stdout);
}
