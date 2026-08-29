#include "carmen/utf8.h"
#include <string.h>

#include "utf8.h" /* vendor/utf8/utf8.h (sheredom) */

#include <fribidi.h>

#define CARMEN_BIDI_MAX_CHARS 512

static int utf8_needs_bidi(const char *s)
{
    FriBidiChar           ucs4[CARMEN_BIDI_MAX_CHARS];
    const FriBidiCharSet  cs  = FRIBIDI_CHAR_SET_UTF8;
    const FriBidiStrIndex len = fribidi_charset_to_unicode(cs, s, (FriBidiStrIndex)strlen(s), ucs4);
    if (len <= 0) return 0;

    for (FriBidiStrIndex i = 0; i < len; i++) {
        const FriBidiCharType t = fribidi_get_bidi_type(ucs4[i]);
        if (FRIBIDI_IS_ARABIC(t)) return 1;
        if ((t & (FRIBIDI_MASK_LETTER | FRIBIDI_MASK_RTL)) ==
            (FRIBIDI_MASK_LETTER | FRIBIDI_MASK_RTL))
            return 1;
    }
    return 0;
}

size_t carmen_utf8_copy(char *dst, size_t dst_size, const char *src)
{
    if (!dst || dst_size == 0) return 0;
    if (!src) {
        dst[0] = '\0';
        return 0;
    }

    utf8ncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';

    return strlen(dst);
}

size_t carmen_utf8_bidi_visual(const char *src, char *dst, size_t dst_size)
{
    if (!dst || dst_size == 0) return 0;
    if (!src || src[0] == '\0') {
        dst[0] = '\0';
        return 0;
    }
    if (!utf8_needs_bidi(src)) return carmen_utf8_copy(dst, dst_size, src);

    FriBidiChar          logical[CARMEN_BIDI_MAX_CHARS];
    FriBidiChar          visual[CARMEN_BIDI_MAX_CHARS];
    FriBidiCharType      bidi_types[CARMEN_BIDI_MAX_CHARS];
    FriBidiBracketType   bracket_types[CARMEN_BIDI_MAX_CHARS];
    FriBidiArabicProp    ar_props[CARMEN_BIDI_MAX_CHARS];
    FriBidiLevel         levels[CARMEN_BIDI_MAX_CHARS];
    FriBidiStrIndex      pos_map[CARMEN_BIDI_MAX_CHARS];
    const FriBidiCharSet cs       = FRIBIDI_CHAR_SET_UTF8;
    FriBidiParType       base_dir = FRIBIDI_PAR_ON;
    const FriBidiFlags   flags =
        FRIBIDI_FLAGS_DEFAULT | FRIBIDI_FLAGS_ARABIC | FRIBIDI_FLAG_SHAPE_ARAB_CONSOLE;

    const FriBidiStrIndex len =
        fribidi_charset_to_unicode(cs, src, (FriBidiStrIndex)strlen(src), logical);
    if (len <= 0 || len >= CARMEN_BIDI_MAX_CHARS) return carmen_utf8_copy(dst, dst_size, src);

    fribidi_get_bidi_types(logical, len, bidi_types);
    fribidi_get_bracket_types(logical, len, bidi_types, bracket_types);
    if (fribidi_get_par_embedding_levels_ex(bidi_types, bracket_types, len, &base_dir, levels) == 0)
        return carmen_utf8_copy(dst, dst_size, src);

    memcpy(visual, logical, (size_t)len * sizeof(FriBidiChar));

    fribidi_get_joining_types(logical, len, ar_props);
    fribidi_join_arabic(bidi_types, len, levels, ar_props);
    fribidi_shape(flags, levels, len, ar_props, visual);

    for (FriBidiStrIndex i = 0; i < len; i++)
        pos_map[i] = i;
    if (fribidi_reorder_line(flags, bidi_types, len, 0, base_dir, levels, visual, pos_map) == 0)
        return carmen_utf8_copy(dst, dst_size, src);

    fribidi_unicode_to_charset(cs, visual, len, dst);
    dst[dst_size - 1] = '\0';
    return strlen(dst);
}
