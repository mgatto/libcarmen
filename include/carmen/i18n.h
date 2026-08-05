#ifndef CARMEN_I18N_H
#define CARMEN_I18N_H

#include <stddef.h>
#include "carmen_export.h"

#define CARMEN_I18N_MAX_FILE_SIZE  (1 << 20)   /* 1 MB */
#define CARMEN_I18N_MAX_KEY_LEN   64
#define CARMEN_I18N_MAX_VALUE_LEN 512
#define CARMEN_I18N_MAX_ENTRIES   2048

typedef struct CarmenI18n CarmenI18n;

/* Load locale strings from a JSON file at json_path.  Returns a context to
   pass to carmen_i18n_get(), or NULL on open/read/parse failure or if
   json_path is NULL.  Caller frees with carmen_i18n_free(). */
CARMEN_API CarmenI18n *carmen_i18n_load(const char *json_path);

/* Load locale strings from an in-memory JSON buffer of length len (the buffer
   need not be NUL-terminated).  Behaves exactly like carmen_i18n_load() but
   takes no filesystem path, so it works in WASM/embedded callers with no
   filesystem.  Returns NULL on parse failure or if json is NULL.  Caller
   frees with carmen_i18n_free(). */
CARMEN_API CarmenI18n *carmen_i18n_load_json(const char *json, size_t len);
CARMEN_API const char *carmen_i18n_get(const CarmenI18n *ctx, const char *key);
CARMEN_API void        carmen_i18n_free(CarmenI18n *ctx);

#endif
