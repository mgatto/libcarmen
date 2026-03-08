#include <string.h>
#include "carmen/villain.h"

const FitnaVillain FITNA_VILLAINS[FITNA_VILLAIN_COUNT] = {
    {"Qamar Samarkandi",   "The Moon",         'F',
     {"The suspect left a crescent moon sticker on the display case",
      "A witness said the suspect was quoting Persian poetry under her breath",
      "The suspect was wearing white silk gloves -- the kind a conservator would use",
      "The suspect asked the curator about provenance records before the theft"}},
    {"Saffron Suleiman",   "The Spice",        'M',
     {"The suspect smelled strongly of saffron and cardamom",
      "He was carrying a small brass merchant's scale",
      "The suspect's fingers were stained yellow -- turmeric, perhaps",
      "A spice pouch was found near the broken lock"}},
    {"Layla Lapis",        "Midnight Blue",    'F',
     {"The suspect wore a striking lapis lazuli pendant",
      "The suspect was only spotted after dark",
      "The suspect's nails were painted a deep midnight blue",
      "A turquoise bead was found at the scene"}},
    {"Tariq al-Tariq",     "The Morning Star", 'M',
     {"The break-in happened just before the dawn prayer",
      "He carried a set of antique brass keys on his belt",
      "The suspect entered through a window no one thought could open",
      "A night guard said the suspect moved like someone who already knew the layout"}},
    {"Minara Mirage",      "The Tower",        'F',
     {"Plaster dust was found under the suspect's fingernails",
      "The suspect was photographing the tilework on the mihrab",
      "The suspect carried a small chisel in a velvet pouch",
      "A carved stone screen panel was surgically removed from the wall"}},
    {"Fakhr the Forger",   "The Pride",        'M',
     {"The suspect's hands were stained with walnut ink",
      "The suspect was seen practicing Arabic calligraphy in a cafe",
      "He carried a reed qalam pen behind his ear",
      "A magnifying loupe was found near the empty manuscript case"}},
    {"Dinar Dervish",      "The Spin",         'M',
     {"The guard said the suspect was spinning slowly, almost hypnotically",
      "Gold coin wrappers were found in the stairwell",
      "He was humming a Sufi devotional melody",
      "The vault was emptied while every guard watched the courtyard performance"}},
    {"Henna Hazara",       "The Mark",         'F',
     {"A tiny henna dot was found on the doorframe",
      "Security footage shows the suspect in three different outfits in one hour",
      "The suspect changed appearance between the market and the mosque",
      "A wig and colored contact lenses were discarded in the alley"}},
    {"Kohl Phantom",       "The Shadow",       'F',
     {"No one actually saw the suspect -- only her shadow",
      "A miniature surveillance camera was found taped under a bench",
      "The suspect's eyes were lined with thick kohl",
      "The suspect seemed to know exactly where the blind spots in the cameras were"}},
    {"Murad al-Mutayyah",  "The Maze",         'M',
     {"He vanished into the medina and no one could follow",
      "The suspect wore leather sandals worn thin from running",
      "A crumpled map of the old city's back alleys was found in a drain",
      "The police chase ended at a dead-end wall -- but the suspect was gone"}},
    {"Soraya Samum",       "The Sandstorm",    'F',
     {"A smoke device was triggered just before the theft",
      "Witnesses described a sudden cloud of dust in the gallery",
      "The suspect wore a desert scarf wrapped loosely around her face",
      "The fire alarm went off on the opposite side of the building -- a diversion"}},
    {"Nasreen Nightshade", "The Rose",         'F',
     {"The guard fell asleep at his post -- a faint floral scent lingered",
      "A rose petal was found on the empty pedestal",
      "The suspect was carrying a small glass vial in her handbag",
      "The suspect asked about local medicinal herbs at the souq"}},
    {"Jalal al-Jinn",      "The Wishmaker",    'M',
     {"The museum director said a charming visitor offered to double the endowment",
      "The suspect made promises that sounded too good to be true",
      "The suspect was wearing an expensive suit and a signet ring",
      "A forged donation receipt was left on the administrator's desk"}},
    {"Zubayr Zero",        "The Cipher",       'M',
     {"The security system was bypassed -- no alarms, no logs",
      "He was seen sketching geometric star patterns on a napkin",
      "The suspect left a sequence of numbers scratched into the glass",
      "A USB device was found plugged into the back of the ticket kiosk"}},
    {"Ivory Ismail",       "The Trader",       'M',
     {"The suspect was overheard negotiating a price in three languages",
      "He had callused hands -- the hands of someone who packs crates",
      "A shipping manifest for Zanzibar was found in the trash",
      "An antique ivory-handled knife was used to pry open the case"}},
    {"Rumi the Riddle",    "The Poet",         'M',
     {"A riddle in Ottoman script was left pinned to the empty frame",
      "He was muttering couplets to himself as he left",
      "The suspect quoted Hafez to a bewildered security guard",
      "A handwritten verse was tucked inside the broken display"}},
};

void fitna_expand_clue(const char *tmpl, char gender, char *out, size_t size)
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
