#ifndef CARMEN_ARTIFACT_H
#define CARMEN_ARTIFACT_H

#include "carmen_export.h"

#define CARMEN_ARTIFACT_COUNT 18

typedef struct {
    const char *id;
    const char *name;            /* i18n key */
    const char *origin_city_id;  /* city the artifact was stolen from */
    const char *description;     /* i18n key */
} CarmenArtifact;

CARMEN_API extern const CarmenArtifact CARMEN_ARTIFACTS[CARMEN_ARTIFACT_COUNT];

#endif
