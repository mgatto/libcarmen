#ifndef CARMEN_VILLAIN_H
#define CARMEN_VILLAIN_H

#include <stddef.h>
#include "carmen_export.h"

#define FITNA_MAX_ID_CLUES   4
#define FITNA_VILLAIN_COUNT 16

typedef struct {
    const char *id;
    const char *name;
    const char *alias;
    char        gender;   /* ASCII 'M' or 'F'; use carmen_villain_expand_clue()
                             for pronoun expansion rather than reading directly */
    const char *id_clues[FITNA_MAX_ID_CLUES];
} FitnaVillain;

CARMEN_API extern const FitnaVillain FITNA_VILLAINS[FITNA_VILLAIN_COUNT];

/*
 * Read-only accessors for the villain catalog, so clients (UI, bindings)
 * can list villains without reaching into the FITNA_VILLAINS[] global.
 * carmen_villain_at returns a borrowed pointer into the static
 * FITNA_VILLAINS[] catalog; valid for the program lifetime and must not
 * be freed.  Returns NULL for an out-of-range index.
 */
CARMEN_API int                 carmen_villain_count(void);
CARMEN_API const FitnaVillain *carmen_villain_at(int index);

/*
 * Expand pronoun template tokens in a clue string.
 *
 * Tokens: {They}/{they}, {Their}/{their}, {them},
 *         {They}'s/{they}'s, {They}'d/{they}'d
 *
 * Replaced with He/She (etc.) based on `gender` ('M' or 'F').
 * Result is written to `out` (at most `size` bytes including NUL).
 */
CARMEN_API void carmen_villain_expand_clue(const char *tmpl, char gender,
                                  char *out, size_t size);

#endif
