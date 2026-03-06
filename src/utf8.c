#include "carmen/utf8.h"
#include <stdio.h>
#include <string.h>

size_t carmen_utf8_copy(char *dst, size_t dst_size, const char *src)
{
    if (!dst || dst_size == 0) return 0;
    if (!src) { dst[0] = '\0'; return 0; }

    int n = snprintf(dst, dst_size, "%s", src);
    size_t len = ((size_t)n < dst_size) ? (size_t)n : dst_size - 1;

    if ((size_t)n < dst_size)
        return len;

    /* Truncation occurred -- trim any incomplete trailing codepoint.
       Walk backwards past continuation bytes (10xxxxxx). */
    size_t end = len;
    while (end > 0 && ((unsigned char)dst[end - 1] & 0xC0) == 0x80)
        end--;

    if (end == 0) {
        dst[0] = '\0';
        return 0;
    }

    unsigned char lead = (unsigned char)dst[end - 1];
    int expected;
    if (lead < 0x80)         expected = 1;
    else if ((lead & 0xE0) == 0xC0) expected = 2;
    else if ((lead & 0xF0) == 0xE0) expected = 3;
    else if ((lead & 0xF8) == 0xF0) expected = 4;
    else { dst[end - 1] = '\0'; return end - 1; }

    if ((int)(len - (end - 1)) == expected)
        return len;

    dst[end - 1] = '\0';
    return end - 1;
}
