#include "carmen/seed_data_islamic.h"
#include "seed_helpers.h"

/* ---- world builder: Modern Muslim Traveler ---- */
/* Clue design: clues are no longer baked onto sites.  Each city owns an
   inbound clue pool -- targetless descriptor strings that "point to it".
   At case generation the engine draws positives from the next trail
   city's pool and decoys from wrong neighbors' pools, assigning
   target_city_id at runtime (see src/case.c). */

void carmen_seed_build_islamic_world(CarmenWorld *w)
{
    if (!w) return;
    carmen_world_init(w);

    /* ------------------------------------------------------------ cities */
    carmen_world_add_city(w, "istanbul",     "city.istanbul.name",     "city.istanbul.local_name",     "city.istanbul.country",     "city.istanbul.continent",     41.01,   28.98);
    carmen_world_add_city(w, "sarajevo",     "city.sarajevo.name",     "city.sarajevo.local_name",     "city.sarajevo.country",     "city.sarajevo.continent",     43.86,   18.41);
    carmen_world_add_city(w, "cairo",        "city.cairo.name",        "city.cairo.local_name",        "city.cairo.country",        "city.cairo.continent",        30.04,   31.24);
    carmen_world_add_city(w, "casablanca",   "city.casablanca.name",   "city.casablanca.local_name",   "city.casablanca.country",   "city.casablanca.continent",   33.57,   -7.59);
    carmen_world_add_city(w, "marrakech",    "city.marrakech.name",    "city.marrakech.local_name",    "city.marrakech.country",    "city.marrakech.continent",    31.63,   -8.00);
    carmen_world_add_city(w, "fez",          "city.fez.name",          "city.fez.local_name",          "city.fez.country",          "city.fez.continent",          34.03,   -5.00);
    carmen_world_add_city(w, "dakar",        "city.dakar.name",        "city.dakar.local_name",        "city.dakar.country",        "city.dakar.continent",        14.72,  -17.47);
    carmen_world_add_city(w, "dubai",        "city.dubai.name",        "city.dubai.local_name",        "city.dubai.country",        "city.dubai.continent",        25.20,   55.27);
    carmen_world_add_city(w, "doha",         "city.doha.name",         "city.doha.local_name",         "city.doha.country",         "city.doha.continent",         25.29,   51.53);
    carmen_world_add_city(w, "abu_dhabi",    "city.abu_dhabi.name",    "city.abu_dhabi.local_name",    "city.abu_dhabi.country",    "city.abu_dhabi.continent",    24.45,   54.65);
    carmen_world_add_city(w, "zanzibar",     "city.zanzibar.name",     "city.zanzibar.local_name",     "city.zanzibar.country",     "city.zanzibar.continent",     -6.16,   39.19);
    carmen_world_add_city(w, "hyderabad",    "city.hyderabad.name",    "city.hyderabad.local_name",    "city.hyderabad.country",    "city.hyderabad.continent",    17.38,   78.49);
    carmen_world_add_city(w, "muscat",       "city.muscat.name",       "city.muscat.local_name",       "city.muscat.country",       "city.muscat.continent",       23.59,   58.55);
    carmen_world_add_city(w, "amman",        "city.amman.name",        "city.amman.local_name",        "city.amman.country",        "city.amman.continent",        31.95,   35.93);
    carmen_world_add_city(w, "beirut",       "city.beirut.name",       "city.beirut.local_name",       "city.beirut.country",       "city.beirut.continent",       33.89,   35.50);
    carmen_world_add_city(w, "isfahan",      "city.isfahan.name",      "city.isfahan.local_name",      "city.isfahan.country",      "city.isfahan.continent",      32.65,   51.68);
    carmen_world_add_city(w, "konya",        "city.konya.name",        "city.konya.local_name",        "city.konya.country",        "city.konya.continent",        37.87,   32.48);
    carmen_world_add_city(w, "tashkent",     "city.tashkent.name",     "city.tashkent.local_name",     "city.tashkent.country",     "city.tashkent.continent",     41.30,   69.28);
    carmen_world_add_city(w, "lahore",       "city.lahore.name",       "city.lahore.local_name",       "city.lahore.country",       "city.lahore.continent",       31.55,   74.35);
    carmen_world_add_city(w, "dhaka",        "city.dhaka.name",        "city.dhaka.local_name",        "city.dhaka.country",        "city.dhaka.continent",        23.81,   90.41);
    carmen_world_add_city(w, "kuala_lumpur", "city.kuala_lumpur.name", "city.kuala_lumpur.local_name", "city.kuala_lumpur.country", "city.kuala_lumpur.continent",  3.14,  101.69);
    carmen_world_add_city(w, "jakarta",      "city.jakarta.name",      "city.jakarta.local_name",      "city.jakarta.country",      "city.jakarta.continent",      -6.21,  106.85);
    carmen_world_add_city(w, "bandung",      "city.bandung.name",      "city.bandung.local_name",      "city.bandung.country",      "city.bandung.continent",      -6.91,  107.61);

    /* --------------------------------------------------------------- sites */
    /* Sites are investigation locations only; they carry no clues. */

    add_site(w, "istanbul", "suleymaniye",     "site.istanbul.suleymaniye",     "type.mosque");
    add_site(w, "istanbul", "istanbul_modern", "site.istanbul.istanbul_modern", "type.museum");
    add_site(w, "istanbul", "kadikoy",         "site.istanbul.kadikoy",         "type.market");
    add_site(w, "istanbul", "camlica",         "site.istanbul.camlica",         "type.mosque");

    add_site(w, "dubai", "museum_future", "site.dubai.museum_future", "type.museum");
    add_site(w, "dubai", "al_fahidi",     "site.dubai.al_fahidi",     "type.landmark");
    add_site(w, "dubai", "dubai_mall",    "site.dubai.dubai_mall",    "type.market");
    add_site(w, "dubai", "jumeirah",      "site.dubai.jumeirah",      "type.mosque");

    add_site(w, "kuala_lumpur", "petronas",      "site.kuala_lumpur.petronas",      "type.landmark");
    add_site(w, "kuala_lumpur", "islamic_arts",  "site.kuala_lumpur.islamic_arts",  "type.museum");
    add_site(w, "kuala_lumpur", "jalan_alor",    "site.kuala_lumpur.jalan_alor",    "type.market");
    add_site(w, "kuala_lumpur", "masjid_negara", "site.kuala_lumpur.masjid_negara", "type.mosque");

    add_site(w, "doha", "islamic_art",    "site.doha.islamic_art",    "type.museum");
    add_site(w, "doha", "souq_waqif",     "site.doha.souq_waqif",     "type.market");
    add_site(w, "doha", "katara",         "site.doha.katara",         "type.landmark");
    add_site(w, "doha", "education_city", "site.doha.education_city", "type.landmark");

    add_site(w, "amman", "citadel",        "site.amman.citadel",        "type.landmark");
    add_site(w, "amman", "rainbow_street", "site.amman.rainbow_street", "type.cafe");
    add_site(w, "amman", "king_abdullah",  "site.amman.king_abdullah",  "type.mosque");

    add_site(w, "casablanca", "hassan_ii",    "site.casablanca.hassan_ii",    "type.mosque");
    add_site(w, "casablanca", "habous",       "site.casablanca.habous",       "type.market");
    add_site(w, "casablanca", "morocco_mall", "site.casablanca.morocco_mall", "type.market");
    add_site(w, "casablanca", "art_deco",     "site.casablanca.art_deco",     "type.landmark");

    add_site(w, "jakarta", "istiqlal",        "site.jakarta.istiqlal",        "type.mosque");
    add_site(w, "jakarta", "national_museum", "site.jakarta.national_museum", "type.museum");
    add_site(w, "jakarta", "kota_tua",        "site.jakarta.kota_tua",        "type.landmark");
    add_site(w, "jakarta", "menteng",         "site.jakarta.menteng",         "type.cafe");

    add_site(w, "sarajevo", "bascarsija",  "site.sarajevo.bascarsija",  "type.market");
    add_site(w, "sarajevo", "gazi_husrev", "site.sarajevo.gazi_husrev", "type.mosque");
    add_site(w, "sarajevo", "war_tunnel",  "site.sarajevo.war_tunnel",  "type.museum");

    add_site(w, "tashkent", "khast_imam",     "site.tashkent.khast_imam",     "type.mosque");
    add_site(w, "tashkent", "chorsu",         "site.tashkent.chorsu",         "type.market");
    add_site(w, "tashkent", "tashkent_metro", "site.tashkent.tashkent_metro", "type.landmark");
    add_site(w, "tashkent", "minor_mosque",   "site.tashkent.minor_mosque",   "type.mosque");

    add_site(w, "cairo", "azhar_park",    "site.cairo.azhar_park",    "type.park");
    add_site(w, "cairo", "tahrir",        "site.cairo.tahrir",        "type.landmark");
    add_site(w, "cairo", "khan_khalili",  "site.cairo.khan_khalili",  "type.market");
    add_site(w, "cairo", "sultan_hassan", "site.cairo.sultan_hassan", "type.mosque");

    add_site(w, "beirut", "al_amin",         "site.beirut.al_amin",         "type.mosque");
    add_site(w, "beirut", "gemmayzeh",       "site.beirut.gemmayzeh",       "type.landmark");
    add_site(w, "beirut", "national_museum", "site.beirut.national_museum", "type.museum");

    add_site(w, "marrakech", "jemaa",     "site.marrakech.jemaa",     "type.landmark");
    add_site(w, "marrakech", "macaal",    "site.marrakech.macaal",    "type.museum");
    add_site(w, "marrakech", "le_jardin", "site.marrakech.le_jardin", "type.park");

    add_site(w, "lahore", "badshahi",    "site.lahore.badshahi",    "type.mosque");
    add_site(w, "lahore", "lahore_fort", "site.lahore.lahore_fort", "type.landmark");
    add_site(w, "lahore", "food_street", "site.lahore.food_street", "type.market");
    add_site(w, "lahore", "anarkali",    "site.lahore.anarkali",    "type.market");

    add_site(w, "konya", "mevlana",  "site.konya.mevlana",  "type.museum");
    add_site(w, "konya", "alaeddin", "site.konya.alaeddin", "type.mosque");
    add_site(w, "konya", "sille",    "site.konya.sille",    "type.landmark");

    add_site(w, "abu_dhabi", "sheikh_zayed",  "site.abu_dhabi.sheikh_zayed",  "type.mosque");
    add_site(w, "abu_dhabi", "louvre_ad",     "site.abu_dhabi.louvre_ad",     "type.museum");
    add_site(w, "abu_dhabi", "mangrove_park", "site.abu_dhabi.mangrove_park", "type.park");
    add_site(w, "abu_dhabi", "qasr_watan",    "site.abu_dhabi.qasr_watan",    "type.landmark");

    add_site(w, "muscat", "sultan_qaboos", "site.muscat.sultan_qaboos", "type.mosque");
    add_site(w, "muscat", "bimmah",        "site.muscat.bimmah",        "type.landmark");
    add_site(w, "muscat", "royal_opera",   "site.muscat.royal_opera",   "type.landmark");

    add_site(w, "dakar", "grande_mosquee",      "site.dakar.grande_mosquee",      "type.mosque");
    add_site(w, "dakar", "goree",               "site.dakar.goree",               "type.landmark");
    add_site(w, "dakar", "african_renaissance", "site.dakar.african_renaissance", "type.landmark");

    add_site(w, "bandung", "braga",       "site.bandung.braga",       "type.cafe");
    add_site(w, "bandung", "masjid_raya", "site.bandung.masjid_raya", "type.mosque");
    add_site(w, "bandung", "tangkuban",   "site.bandung.tangkuban",   "type.landmark");

    add_site(w, "isfahan", "naqsh_e_jahan", "site.isfahan.naqsh_e_jahan", "type.landmark");
    add_site(w, "isfahan", "vank",          "site.isfahan.vank",          "type.landmark");
    add_site(w, "isfahan", "si_o_se_pol",   "site.isfahan.si_o_se_pol",   "type.cafe");

    add_site(w, "dhaka", "baitul_mukarram", "site.dhaka.baitul_mukarram", "type.mosque");
    add_site(w, "dhaka", "star_mosque",     "site.dhaka.star_mosque",     "type.mosque");
    add_site(w, "dhaka", "lalbagh",         "site.dhaka.lalbagh",         "type.landmark");
    add_site(w, "dhaka", "sadarghat",       "site.dhaka.sadarghat",       "type.landmark");

    add_site(w, "fez", "qarawiyyin", "site.fez.qarawiyyin", "type.mosque");
    add_site(w, "fez", "fez_medina", "site.fez.fez_medina", "type.market");
    add_site(w, "fez", "borj_nord",  "site.fez.borj_nord",  "type.landmark");

    add_site(w, "zanzibar", "stone_town", "site.zanzibar.stone_town", "type.landmark");
    add_site(w, "zanzibar", "hamamni",    "site.zanzibar.hamamni",    "type.landmark");
    add_site(w, "zanzibar", "malindi",    "site.zanzibar.malindi",    "type.mosque");
    add_site(w, "zanzibar", "forodhani",  "site.zanzibar.forodhani",  "type.market");

    add_site(w, "hyderabad", "charminar",    "site.hyderabad.charminar",    "type.landmark");
    add_site(w, "hyderabad", "mecca_masjid", "site.hyderabad.mecca_masjid", "type.mosque");
    add_site(w, "hyderabad", "salar_jung",   "site.hyderabad.salar_jung",   "type.museum");
    add_site(w, "hyderabad", "laad_bazaar",  "site.hyderabad.laad_bazaar",  "type.market");

    /* --------------------------------------------------- inbound clue pools */
    /* Targetless descriptor keys; each pool "points to" its own city. */

    { const char *k[] = {"clue.istanbul.inbound.0", "clue.istanbul.inbound.1", "clue.istanbul.inbound.2", "clue.istanbul.inbound.3"};
      add_inbound_clues(w, "istanbul", k, 4); }
    { const char *k[] = {"clue.sarajevo.inbound.0", "clue.sarajevo.inbound.1", "clue.sarajevo.inbound.2", "clue.sarajevo.inbound.3"};
      add_inbound_clues(w, "sarajevo", k, 4); }
    { const char *k[] = {"clue.cairo.inbound.0", "clue.cairo.inbound.1", "clue.cairo.inbound.2", "clue.cairo.inbound.3"};
      add_inbound_clues(w, "cairo", k, 4); }
    { const char *k[] = {"clue.casablanca.inbound.0", "clue.casablanca.inbound.1", "clue.casablanca.inbound.2", "clue.casablanca.inbound.3"};
      add_inbound_clues(w, "casablanca", k, 4); }
    { const char *k[] = {"clue.marrakech.inbound.0", "clue.marrakech.inbound.1", "clue.marrakech.inbound.2", "clue.marrakech.inbound.3"};
      add_inbound_clues(w, "marrakech", k, 4); }
    { const char *k[] = {"clue.fez.inbound.0", "clue.fez.inbound.1", "clue.fez.inbound.2", "clue.fez.inbound.3"};
      add_inbound_clues(w, "fez", k, 4); }
    { const char *k[] = {"clue.dakar.inbound.0", "clue.dakar.inbound.1", "clue.dakar.inbound.2", "clue.dakar.inbound.3"};
      add_inbound_clues(w, "dakar", k, 4); }
    { const char *k[] = {"clue.dubai.inbound.0", "clue.dubai.inbound.1", "clue.dubai.inbound.2", "clue.dubai.inbound.3"};
      add_inbound_clues(w, "dubai", k, 4); }
    { const char *k[] = {"clue.doha.inbound.0", "clue.doha.inbound.1", "clue.doha.inbound.2", "clue.doha.inbound.3"};
      add_inbound_clues(w, "doha", k, 4); }
    { const char *k[] = {"clue.abu_dhabi.inbound.0", "clue.abu_dhabi.inbound.1", "clue.abu_dhabi.inbound.2", "clue.abu_dhabi.inbound.3"};
      add_inbound_clues(w, "abu_dhabi", k, 4); }
    { const char *k[] = {"clue.zanzibar.inbound.0", "clue.zanzibar.inbound.1", "clue.zanzibar.inbound.2", "clue.zanzibar.inbound.3"};
      add_inbound_clues(w, "zanzibar", k, 4); }
    { const char *k[] = {"clue.hyderabad.inbound.0", "clue.hyderabad.inbound.1", "clue.hyderabad.inbound.2", "clue.hyderabad.inbound.3"};
      add_inbound_clues(w, "hyderabad", k, 4); }
    { const char *k[] = {"clue.muscat.inbound.0", "clue.muscat.inbound.1", "clue.muscat.inbound.2", "clue.muscat.inbound.3"};
      add_inbound_clues(w, "muscat", k, 4); }
    { const char *k[] = {"clue.amman.inbound.0", "clue.amman.inbound.1", "clue.amman.inbound.2", "clue.amman.inbound.3"};
      add_inbound_clues(w, "amman", k, 4); }
    { const char *k[] = {"clue.beirut.inbound.0", "clue.beirut.inbound.1", "clue.beirut.inbound.2", "clue.beirut.inbound.3"};
      add_inbound_clues(w, "beirut", k, 4); }
    { const char *k[] = {"clue.isfahan.inbound.0", "clue.isfahan.inbound.1", "clue.isfahan.inbound.2", "clue.isfahan.inbound.3"};
      add_inbound_clues(w, "isfahan", k, 4); }
    { const char *k[] = {"clue.konya.inbound.0", "clue.konya.inbound.1", "clue.konya.inbound.2", "clue.konya.inbound.3"};
      add_inbound_clues(w, "konya", k, 4); }
    { const char *k[] = {"clue.tashkent.inbound.0", "clue.tashkent.inbound.1", "clue.tashkent.inbound.2", "clue.tashkent.inbound.3"};
      add_inbound_clues(w, "tashkent", k, 4); }
    { const char *k[] = {"clue.lahore.inbound.0", "clue.lahore.inbound.1", "clue.lahore.inbound.2", "clue.lahore.inbound.3"};
      add_inbound_clues(w, "lahore", k, 4); }
    { const char *k[] = {"clue.dhaka.inbound.0", "clue.dhaka.inbound.1", "clue.dhaka.inbound.2", "clue.dhaka.inbound.3"};
      add_inbound_clues(w, "dhaka", k, 4); }
    { const char *k[] = {"clue.kuala_lumpur.inbound.0", "clue.kuala_lumpur.inbound.1", "clue.kuala_lumpur.inbound.2", "clue.kuala_lumpur.inbound.3"};
      add_inbound_clues(w, "kuala_lumpur", k, 4); }
    { const char *k[] = {"clue.jakarta.inbound.0", "clue.jakarta.inbound.1", "clue.jakarta.inbound.2", "clue.jakarta.inbound.3"};
      add_inbound_clues(w, "jakarta", k, 4); }
    { const char *k[] = {"clue.bandung.inbound.0", "clue.bandung.inbound.1", "clue.bandung.inbound.2", "clue.bandung.inbound.3"};
      add_inbound_clues(w, "bandung", k, 4); }

    /* ---------------------------------------------------------- connections */
    /* Europe */
    add_route(w, "istanbul",   "sarajevo",     950,  "flight");
    add_route(w, "istanbul",   "konya",        660,  "train");

    /* Europe - Middle East */
    add_route(w, "istanbul",   "cairo",        1250, "flight");
    add_route(w, "istanbul",   "beirut",       1100, "flight");

    /* Levant */
    add_route(w, "cairo",      "amman",        400,  "flight");
    add_route(w, "beirut",     "amman",        280,  "flight");

    /* Levant - Gulf */
    add_route(w, "amman",      "dubai",        2400, "flight");

    /* Gulf */
    add_route(w, "dubai",      "doha",         350,  "flight");
    add_route(w, "dubai",      "muscat",       450,  "boat");
    add_route(w, "dubai",      "abu_dhabi",    140,  "train");
    add_route(w, "doha",       "abu_dhabi",    320,  "flight");
    add_route(w, "abu_dhabi",  "muscat",       470,  "boat");

    /* Gulf - Iran / Central-South Asia */
    add_route(w, "dubai",      "isfahan",      1200, "flight");
    add_route(w, "dubai",      "lahore",       2400, "flight");

    /* Iran - Turkey / Central Asia */
    add_route(w, "isfahan",    "konya",        2200, "flight");
    add_route(w, "isfahan",    "tashkent",     2000, "flight");

    /* Central - South Asia */
    add_route(w, "tashkent",   "lahore",       2200, "flight");
    add_route(w, "lahore",     "dhaka",        1800, "flight");
    add_route(w, "muscat",     "dhaka",        4500, "boat");

    /* South - Southeast Asia */
    add_route(w, "dhaka",      "kuala_lumpur", 3200, "flight");
    add_route(w, "kuala_lumpur","jakarta",     1200, "flight");
    add_route(w, "kuala_lumpur","bandung",     1300, "flight");
    add_route(w, "jakarta",    "bandung",      150,  "train");

    /* North Africa */
    add_route(w, "cairo",      "casablanca",   3350, "flight");
    add_route(w, "cairo",      "abu_dhabi",    2400, "flight");
    add_route(w, "casablanca", "marrakech",    240,  "train");
    add_route(w, "casablanca", "fez",          300,  "train");
    add_route(w, "casablanca", "dakar",        2600, "flight");
    add_route(w, "marrakech",  "fez",          400,  "train");

    /* East Africa */
    add_route(w, "zanzibar",   "cairo",        3800, "flight");
    add_route(w, "zanzibar",   "muscat",       3200, "boat");
    add_route(w, "zanzibar",   "dakar",        6200, "flight");

    /* India */
    add_route(w, "hyderabad",  "dubai",        2700, "flight");
    add_route(w, "hyderabad",  "lahore",       1500, "flight");
    add_route(w, "hyderabad",  "dhaka",        1500, "flight");
    add_route(w, "hyderabad",  "kuala_lumpur", 3900, "flight");

    /* Cross-regional */
    add_route(w, "fez",        "sarajevo",     2200, "flight");
    add_route(w, "dakar",      "cairo",        4700, "flight");
}
