/*
 * i18n.c -- load a locale file and look up a few strings. The library resolves
 * all user-facing text through these keys, so front-ends stay language-neutral.
 * Run from the repository root (the path is resolved against the CWD, like the
 * demo).
 */
#include "carmen/carmen.h"
#include <stdio.h>

int main(void)
{
    CarmenI18n *i18n = carmen_i18n_load("locales/en.json");
    if (!i18n) {
        fprintf(stderr, "carmen_i18n_load(\"locales/en.json\") failed\n");
        return 1;
    }

    printf("city name (en): %s\n", carmen_i18n_get(i18n, "city.istanbul.name"));
    printf("ui title (en):  %s\n", carmen_i18n_get(i18n, "ui.title"));

    carmen_i18n_free(i18n);
    return 0;
}
