#ifndef CARMEN_I18N_H
#define CARMEN_I18N_H

#include <stddef.h>
#include "carmen/carmen_export.h"

typedef struct CarmenI18n CarmenI18n;

CARMEN_API CarmenI18n *carmen_i18n_load(const char *json_path);
CARMEN_API const char *carmen_i18n_get(const CarmenI18n *ctx, const char *key);
CARMEN_API void        carmen_i18n_free(CarmenI18n *ctx);

#endif
