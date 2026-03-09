#include <string.h>
#include "carmen/villain.h"

const FitnaVillain FITNA_VILLAINS[FITNA_VILLAIN_COUNT] = {
    {"qamar",   "villain.qamar.name",   "villain.qamar.alias",   'F',
     {"villain.qamar.0", "villain.qamar.1",
      "villain.qamar.2", "villain.qamar.3"}},
    {"saffron", "villain.saffron.name", "villain.saffron.alias", 'M',
     {"villain.saffron.0", "villain.saffron.1",
      "villain.saffron.2", "villain.saffron.3"}},
    {"layla",   "villain.layla.name",   "villain.layla.alias",   'F',
     {"villain.layla.0", "villain.layla.1",
      "villain.layla.2", "villain.layla.3"}},
    {"tariq",   "villain.tariq.name",   "villain.tariq.alias",   'M',
     {"villain.tariq.0", "villain.tariq.1",
      "villain.tariq.2", "villain.tariq.3"}},
    {"minara",  "villain.minara.name",  "villain.minara.alias",  'F',
     {"villain.minara.0", "villain.minara.1",
      "villain.minara.2", "villain.minara.3"}},
    {"fakhr",   "villain.fakhr.name",   "villain.fakhr.alias",   'M',
     {"villain.fakhr.0", "villain.fakhr.1",
      "villain.fakhr.2", "villain.fakhr.3"}},
    {"dinar",   "villain.dinar.name",   "villain.dinar.alias",   'M',
     {"villain.dinar.0", "villain.dinar.1",
      "villain.dinar.2", "villain.dinar.3"}},
    {"henna",   "villain.henna.name",   "villain.henna.alias",   'F',
     {"villain.henna.0", "villain.henna.1",
      "villain.henna.2", "villain.henna.3"}},
    {"kohl",    "villain.kohl.name",    "villain.kohl.alias",    'F',
     {"villain.kohl.0", "villain.kohl.1",
      "villain.kohl.2", "villain.kohl.3"}},
    {"murad",   "villain.murad.name",   "villain.murad.alias",   'M',
     {"villain.murad.0", "villain.murad.1",
      "villain.murad.2", "villain.murad.3"}},
    {"soraya",  "villain.soraya.name",  "villain.soraya.alias",  'F',
     {"villain.soraya.0", "villain.soraya.1",
      "villain.soraya.2", "villain.soraya.3"}},
    {"nasreen", "villain.nasreen.name", "villain.nasreen.alias", 'F',
     {"villain.nasreen.0", "villain.nasreen.1",
      "villain.nasreen.2", "villain.nasreen.3"}},
    {"jalal",   "villain.jalal.name",   "villain.jalal.alias",   'M',
     {"villain.jalal.0", "villain.jalal.1",
      "villain.jalal.2", "villain.jalal.3"}},
    {"zubayr",  "villain.zubayr.name",  "villain.zubayr.alias",  'M',
     {"villain.zubayr.0", "villain.zubayr.1",
      "villain.zubayr.2", "villain.zubayr.3"}},
    {"ivory",   "villain.ivory.name",   "villain.ivory.alias",   'M',
     {"villain.ivory.0", "villain.ivory.1",
      "villain.ivory.2", "villain.ivory.3"}},
    {"rumi",    "villain.rumi.name",    "villain.rumi.alias",    'M',
     {"villain.rumi.0", "villain.rumi.1",
      "villain.rumi.2", "villain.rumi.3"}},
};

void carmen_villain_expand_clue(const char *tmpl, char gender, char *out, size_t size)
{
    int male = (gender == 'M');
    const char *r = tmpl;
    char *w = out;
    char *end = out + size - 1;

    while (*r && w < end) {
        if (*r == '{') {
            const char *rep = NULL;

            if (strncmp(r, "{They}'s", 8) == 0) {
                rep = male ? "he's"  : "she's";  r += 8;
            } else if (strncmp(r, "{They}'d", 8) == 0) {
                rep = male ? "he'd"  : "she'd";  r += 8;
            } else if (strncmp(r, "{they}'s", 8) == 0) {
                rep = male ? "he's"  : "she's";  r += 8;
            } else if (strncmp(r, "{they}'d", 8) == 0) {
                rep = male ? "he'd"  : "she'd";  r += 8;
            } else if (strncmp(r, "{They}",  6) == 0) {
                rep = male ? "He"    : "She";    r += 6;
            } else if (strncmp(r, "{they}",  6) == 0) {
                rep = male ? "he"    : "she";    r += 6;
            } else if (strncmp(r, "{Their}", 7) == 0) {
                rep = male ? "His"   : "Her";    r += 7;
            } else if (strncmp(r, "{their}", 7) == 0) {
                rep = male ? "his"   : "her";    r += 7;
            } else if (strncmp(r, "{them}",  6) == 0) {
                rep = male ? "him"   : "her";    r += 6;
            }

            if (rep) {
                while (*rep && w < end)
                    *w++ = *rep++;
                continue;
            }
        }
        *w++ = *r++;
    }
    *w = '\0';
}
