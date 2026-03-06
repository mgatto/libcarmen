#include "carmen/utf8.h"
#include <string.h>

#include "utf8.h" /* vendor/utf8/utf8.h (sheredom) */

size_t carmen_utf8_copy(char *dst, size_t dst_size, const char *src)
{
    if (!dst || dst_size == 0) return 0;
    if (!src) { dst[0] = '\0'; return 0; }

    utf8ncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';

    return strlen(dst);
}
