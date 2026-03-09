#ifndef CARMEN_VILLAIN_H
#define CARMEN_VILLAIN_H

#include <stddef.h>
#include "carmen/carmen_export.h"

#define FITNA_MAX_ID_CLUES   4
#define FITNA_VILLAIN_COUNT 16

typedef struct {
    const char *id;
    const char *name;
    const char *alias;
    char        gender;   /* 'M' or 'F' */
    const char *id_clues[FITNA_MAX_ID_CLUES];
} FitnaVillain;

CARMEN_API extern const FitnaVillain FITNA_VILLAINS[FITNA_VILLAIN_COUNT];

/*
 * Expand pronoun template tokens in a clue string.
 *
 * Tokens: {They}/{they}, {Their}/{their}, {them},
 *         {They}'s/{they}'s, {They}'d/{they}'d
 *
 * Replaced with He/She (etc.) based on `gender` ('M' or 'F').
 * Result is written to `out` (at most `size` bytes including NUL).
 */
CARMEN_API void fitna_expand_clue(const char *tmpl, char gender,
                                  char *out, size_t size);

#endif
