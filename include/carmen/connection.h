#ifndef CARMEN_CONNECTION_H
#define CARMEN_CONNECTION_H

#include "clue.h" /* for CARMEN_MAX_NAME_LEN */

typedef struct {
    char destination_id[CARMEN_MAX_NAME_LEN];
    int  distance_km;
    char transport_mode[CARMEN_MAX_NAME_LEN];
} CarmenConnection;

CARMEN_API void carmen_connection_init(CarmenConnection *c,
                                       const char *dest_id,
                                       int distance_km,
                                       const char *transport_mode);

/* Write "-> dest via mode (N km)" into buf.  Returns the number of
   characters that would have been written (excluding '\0'). */
CARMEN_API int  carmen_connection_to_string(const CarmenConnection *c,
                                            char *buf, int buf_size);
CARMEN_API void carmen_connection_print(const CarmenConnection *c);

/* Nominal speed in km/h for a transport mode string.
   "flight" = 800, "train" = 200, "boat" = 60, unknown = 800. */
CARMEN_API int  carmen_transport_speed_kph(const char *mode);

/* Estimated travel time in hours for this connection (rounded up). */
CARMEN_API int  carmen_connection_travel_hrs(const CarmenConnection *c);

#endif
