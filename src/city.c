#include <stdio.h>
#include <string.h>
#include "carmen/city.h"
#include "carmen/utf8.h"

void carmen_city_init(CarmenCity *c, const char *id, const char *name,
                      const char *local_name, const char *country,
                      const char *continent, double latitude, double longitude)
{
    if (!c || !id || !name || !country || !continent) return;
    memset(c, 0, sizeof(*c));
    carmen_utf8_copy(c->id, CARMEN_MAX_NAME_LEN, id);
    carmen_utf8_copy(c->name, CARMEN_MAX_NAME_LEN, name);
    if (local_name)
        carmen_utf8_copy(c->local_name, CARMEN_MAX_NAME_LEN, local_name);
    carmen_utf8_copy(c->country, CARMEN_MAX_NAME_LEN, country);
    carmen_utf8_copy(c->continent, CARMEN_MAX_NAME_LEN, continent);
    c->latitude  = latitude;
    c->longitude = longitude;
}

void carmen_city_add_site(CarmenCity *c, const CarmenSite *s)
{
    if (!c || !s) return;
    if (c->site_count < CARMEN_MAX_SITES) {
        c->sites[c->site_count] = *s;
        c->site_count++;
    }
}

void carmen_city_add_connection(CarmenCity *c, const CarmenConnection *conn)
{
    if (!c || !conn) return;
    if (c->connection_count < CARMEN_MAX_CONNECTIONS) {
        c->connections[c->connection_count] = *conn;
        c->connection_count++;
    }
}

int carmen_city_has_connection_to(const CarmenCity *c, const char *dest_id)
{
    if (!c || !dest_id) return 0;
    for (int i = 0; i < c->connection_count; i++) {
        if (strcmp(c->connections[i].destination_id, dest_id) == 0)
            return 1;
    }
    return 0;
}

int carmen_city_sites_of_type(const CarmenCity *c, const char *type,
                              const CarmenSite **out, int max_out)
{
    if (!c || !type || !out || max_out <= 0) return 0;
    int count = 0;
    for (int i = 0; i < c->site_count && count < max_out; i++) {
        if (strcmp(c->sites[i].site_type, type) == 0) {
            out[count++] = &c->sites[i];
        }
    }
    return count;
}

const CarmenClue *carmen_city_random_clue(const CarmenCity *c)
{
    if (!c || c->site_count == 0)
        return NULL;

    int total = 0;
    for (int i = 0; i < c->site_count; i++)
        total += c->sites[i].clue_count;
    if (total == 0)
        return NULL;

    int pick = carmen_random() % total;
    for (int i = 0; i < c->site_count; i++) {
        if (pick < c->sites[i].clue_count)
            return &c->sites[i].clues[pick];
        pick -= c->sites[i].clue_count;
    }
    return NULL;
}

int carmen_city_to_string(const CarmenCity *c, char *buf, int buf_size)
{
    if (!c) return 0;
    if (!buf || buf_size <= 0)
        return snprintf(NULL, 0, "%s, %s (%s)",
                        c->name, c->country, c->continent);
    return snprintf(buf, (size_t)buf_size, "%s, %s (%s)",
                    c->name, c->country, c->continent);
}

int carmen_city_detail_to_string(const CarmenCity *c, char *buf, int buf_size)
{
    if (!c) return 0;
    if (!buf || buf_size <= 0)
        return snprintf(NULL, 0, "<City:%s %s, %s (%s) sites=%d connections=%d>",
                        c->id, c->name, c->country, c->continent,
                        c->site_count, c->connection_count);
    return snprintf(buf, (size_t)buf_size,
                    "<City:%s %s, %s (%s) sites=%d connections=%d>",
                    c->id, c->name, c->country, c->continent,
                    c->site_count, c->connection_count);
}

void carmen_city_print(const CarmenCity *c)
{
    if (!c) return;
    char buf[CARMEN_MAX_NAME_LEN * 3 + 8];
    carmen_city_to_string(c, buf, (int)sizeof(buf));
    fputs(buf, stdout);
}

void carmen_city_print_detail(const CarmenCity *c)
{
    if (!c) return;
    char buf[CARMEN_MAX_NAME_LEN * 4 + 64];
    carmen_city_detail_to_string(c, buf, (int)sizeof(buf));
    fputs(buf, stdout);
}
