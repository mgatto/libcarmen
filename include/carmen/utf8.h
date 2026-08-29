#ifndef CARMEN_UTF8_H
#define CARMEN_UTF8_H

#include "carmen_export.h"
#include <stddef.h>

/* Copy src into dst (at most dst_size bytes including NUL).
   Guarantees valid UTF-8: any trailing incomplete codepoint produced
   by truncation is removed.  Always NUL-terminates.
   Returns the number of bytes written (excluding NUL). */
CARMEN_API size_t carmen_utf8_copy(char *dst, size_t dst_size, const char *src);

/* Reorder src to visual order for terminal display (BiDi + Arabic shaping).
   dst receives UTF-8; always NUL-terminated.
   Returns bytes written (excluding NUL), or 0 on error/empty input.
   Pure ASCII/LTR strings are copied unchanged. */
CARMEN_API size_t carmen_utf8_bidi_visual(const char *src, char *dst, size_t dst_size);

#endif
