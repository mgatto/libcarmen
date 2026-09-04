#ifndef CARMEN_CLUE_H
#define CARMEN_CLUE_H

#include "carmen_export.h"

#define CARMEN_MAX_NAME_LEN 48
#define CARMEN_MAX_CLUE_LEN 48

/* Per-city pool of "inbound" descriptor clues -- targetless strings that
   describe (point to) the city.  At case generation the engine draws from
   the next trail city's pool and assigns target_city_id at runtime. */
#define CARMEN_MAX_INBOUND_CLUES 6

/* ABI note: CarmenClueType enum values fit in int32 and bindings may read
   a CarmenClueType struct field as a 32-bit signed integer.  The enumerators
   are guaranteed to remain CARMEN_CLUE_POSITIVE=0, CARMEN_CLUE_NEGATIVE=1,
   CARMEN_CLUE_IDENTITY=2. */
typedef enum {
    CARMEN_CLUE_POSITIVE,   /* 0 -- points toward the correct next city */
    CARMEN_CLUE_NEGATIVE,   /* 1 -- dead end, no one saw the suspect */
    CARMEN_CLUE_IDENTITY    /* 2 -- describes the suspect; not a location
                                    pointer. text is a villain id-clue key;
                                    target_city_id is empty. */
} CarmenClueType;

typedef struct {
    char          text[CARMEN_MAX_CLUE_LEN];
    char          target_city_id[CARMEN_MAX_NAME_LEN];
    CarmenClueType type;
} CarmenClue;

/* Pluggable random number generator.  The callback must return a
   non-negative int.  Pass NULL to restore the default (stdlib rand). */
typedef int (*carmen_rand_fn)(void *ctx);
CARMEN_API void carmen_set_rand(carmen_rand_fn fn, void *ctx);
CARMEN_API int  carmen_random(void);

#endif
