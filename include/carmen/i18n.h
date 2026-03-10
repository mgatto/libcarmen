#ifndef CARMEN_I18N_H
#define CARMEN_I18N_H

#include <stddef.h>
#include "carmen_export.h"

#define CARMEN_I18N_MAX_FILE_SIZE  (1 << 20)   /* 1 MB */
#define CARMEN_I18N_MAX_KEY_LEN   64
#define CARMEN_I18N_MAX_VALUE_LEN 512
#define CARMEN_I18N_MAX_ENTRIES   2048

typedef struct CarmenI18n CarmenI18n;

CARMEN_API CarmenI18n *carmen_i18n_load(const char *json_path);
CARMEN_API const char *carmen_i18n_get(const CarmenI18n *ctx, const char *key);
CARMEN_API void        carmen_i18n_free(CarmenI18n *ctx);

#endif
