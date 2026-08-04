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
    carmen_world_add_city(w, "fez",          "city.fez.name",          "city.fez.local_name",          "city.fez.country",          "city.fez.continent",          34.03,   -5.00);
    carmen_world_add_city(w, "dakar",        "city.dakar.name",        "city.dakar.local_name",        "city.dakar.country",        "city.dakar.continent",        14.72,  -17.47);
    carmen_world_add_city(w, "dubai",        "city.dubai.name",        "city.dubai.local_name",        "city.dubai.country",        "city.dubai.continent",        25.20,   55.27);
    carmen_world_add_city(w, "zanzibar",     "city.zanzibar.name",     "city.zanzibar.local_name",     "city.zanzibar.country",     "city.zanzibar.continent",     -6.16,   39.19);
    carmen_world_add_city(w, "hyderabad",    "city.hyderabad.name",    "city.hyderabad.local_name",    "city.hyderabad.country",    "city.hyderabad.continent",    17.38,   78.49);
    carmen_world_add_city(w, "muscat",       "city.muscat.name",       "city.muscat.local_name",       "city.muscat.country",       "city.muscat.continent",       23.59,   58.55);
    carmen_world_add_city(w, "isfahan",      "city.isfahan.name",      "city.isfahan.local_name",      "city.isfahan.country",      "city.isfahan.continent",      32.65,   51.68);
    carmen_world_add_city(w, "konya",        "city.konya.name",        "city.konya.local_name",        "city.konya.country",        "city.konya.continent",        37.87,   32.48);
    carmen_world_add_city(w, "lahore",       "city.lahore.name",       "city.lahore.local_name",       "city.lahore.country",       "city.lahore.continent",       31.55,   74.35);
    carmen_world_add_city(w, "dhaka",        "city.dhaka.name",        "city.dhaka.local_name",        "city.dhaka.country",        "city.dhaka.continent",        23.81,   90.41);
    carmen_world_add_city(w, "kuala_lumpur", "city.kuala_lumpur.name", "city.kuala_lumpur.local_name", "city.kuala_lumpur.country", "city.kuala_lumpur.continent",  3.14,  101.69);
    carmen_world_add_city(w, "jakarta",      "city.jakarta.name",      "city.jakarta.local_name",      "city.jakarta.country",      "city.jakarta.continent",      -6.21,  106.85);
    carmen_world_add_city(w, "tunis",        "city.tunis.name",        "city.tunis.local_name",        "city.tunis.country",        "city.tunis.continent",        36.81,   10.18);
    carmen_world_add_city(w, "damascus",     "city.damascus.name",     "city.damascus.local_name",     "city.damascus.country",     "city.damascus.continent",     33.51,   36.29);
    carmen_world_add_city(w, "kano",         "city.kano.name",         "city.kano.local_name",         "city.kano.country",         "city.kano.continent",         12.00,    8.52);
    carmen_world_add_city(w, "samarkand",    "city.samarkand.name",    "city.samarkand.local_name",    "city.samarkand.country",    "city.samarkand.continent",    39.63,   66.98);
    carmen_world_add_city(w, "kabul",        "city.kabul.name",        "city.kabul.local_name",        "city.kabul.country",        "city.kabul.continent",        34.53,   69.17);
    carmen_world_add_city(w, "yogyakarta",   "city.yogyakarta.name",   "city.yogyakarta.local_name",   "city.yogyakarta.country",   "city.yogyakarta.continent",   -7.80,  110.36);

    /* ------------------------------------------------------------- spheres */
    /* Internal cultural sub-sphere grouping (doc/future/cross-sphere-connectivity.md). */
    set_sphere(w, "casablanca",   "maghreb");
    set_sphere(w, "fez",          "maghreb");
    set_sphere(w, "tunis",        "maghreb");
    set_sphere(w, "istanbul",     "turkey_balkans");
    set_sphere(w, "konya",        "turkey_balkans");
    set_sphere(w, "sarajevo",     "turkey_balkans");
    set_sphere(w, "cairo",        "arab_east");
    set_sphere(w, "damascus",     "arab_east");
    set_sphere(w, "dubai",        "arab_east");
    set_sphere(w, "muscat",       "arab_east");
    set_sphere(w, "dakar",        "sub_saharan_africa");
    set_sphere(w, "zanzibar",     "sub_saharan_africa");
    set_sphere(w, "kano",         "sub_saharan_africa");
    set_sphere(w, "isfahan",      "iran_central_asia");
    set_sphere(w, "samarkand",    "iran_central_asia");
    set_sphere(w, "kabul",        "iran_central_asia");
    set_sphere(w, "lahore",       "south_asia");
    set_sphere(w, "dhaka",        "south_asia");
    set_sphere(w, "hyderabad",    "south_asia");
    set_sphere(w, "kuala_lumpur", "southeast_asia");
    set_sphere(w, "jakarta",      "southeast_asia");
    set_sphere(w, "yogyakarta",   "southeast_asia");

    /* --------------------------------------------------------------- sites */
    /* Sites are investigation locations only; they carry no clues. */

    add_site(w, "istanbul", "suleymaniye",     "site.istanbul.suleymaniye",     "type.mosque");
    add_site(w, "istanbul", "istanbul_modern", "site.istanbul.istanbul_modern", "type.museum");
    add_site(w, "istanbul", "kadikoy",         "site.istanbul.kadikoy",         "type.market");
    add_site(w, "istanbul", "topkapi",         "site.istanbul.topkapi",         "type.landmark");

    add_site(w, "dubai", "museum_future", "site.dubai.museum_future", "type.museum");
    add_site(w, "dubai", "al_fahidi",     "site.dubai.al_fahidi",     "type.landmark");
    add_site(w, "dubai", "dubai_mall",    "site.dubai.dubai_mall",    "type.market");
    add_site(w, "dubai", "jumeirah",      "site.dubai.jumeirah",      "type.mosque");

    add_site(w, "kuala_lumpur", "petronas",      "site.kuala_lumpur.petronas",      "type.landmark");
    add_site(w, "kuala_lumpur", "islamic_arts",  "site.kuala_lumpur.islamic_arts",  "type.museum");
    add_site(w, "kuala_lumpur", "jalan_alor",    "site.kuala_lumpur.jalan_alor",    "type.market");
    add_site(w, "kuala_lumpur", "masjid_negara", "site.kuala_lumpur.masjid_negara", "type.mosque");

    add_site(w, "casablanca", "hassan_ii",    "site.casablanca.hassan_ii",    "type.mosque");
    add_site(w, "casablanca", "habous",       "site.casablanca.habous",       "type.market");
    add_site(w, "casablanca", "slaoui",       "site.casablanca.slaoui",       "type.museum");
    add_site(w, "casablanca", "art_deco",     "site.casablanca.art_deco",     "type.landmark");

    add_site(w, "jakarta", "istiqlal",        "site.jakarta.istiqlal",        "type.mosque");
    add_site(w, "jakarta", "national_museum", "site.jakarta.national_museum", "type.museum");
    add_site(w, "jakarta", "kota_tua",        "site.jakarta.kota_tua",        "type.landmark");
    add_site(w, "jakarta", "menteng",         "site.jakarta.menteng",         "type.cafe");

    add_site(w, "sarajevo", "bascarsija",  "site.sarajevo.bascarsija",  "type.market");
    add_site(w, "sarajevo", "gazi_husrev", "site.sarajevo.gazi_husrev", "type.mosque");
    add_site(w, "sarajevo", "war_tunnel",  "site.sarajevo.war_tunnel",  "type.museum");
    add_site(w, "sarajevo", "vrelo_bosne", "site.sarajevo.vrelo_bosne", "type.park");

    add_site(w, "cairo", "azhar_park",    "site.cairo.azhar_park",    "type.park");
    add_site(w, "cairo", "tahrir",        "site.cairo.tahrir",        "type.landmark");
    add_site(w, "cairo", "khan_khalili",  "site.cairo.khan_khalili",  "type.market");
    add_site(w, "cairo", "sultan_hassan", "site.cairo.sultan_hassan", "type.mosque");

    add_site(w, "lahore", "badshahi",    "site.lahore.badshahi",    "type.mosque");
    add_site(w, "lahore", "lahore_fort", "site.lahore.lahore_fort", "type.landmark");
    add_site(w, "lahore", "food_street", "site.lahore.food_street", "type.market");
    add_site(w, "lahore", "lahore_museum", "site.lahore.lahore_museum", "type.museum");

    add_site(w, "konya", "mevlana",  "site.konya.mevlana",  "type.museum");
    add_site(w, "konya", "alaeddin", "site.konya.alaeddin", "type.mosque");
    add_site(w, "konya", "sille",    "site.konya.sille",    "type.landmark");
    add_site(w, "konya", "bedesten", "site.konya.bedesten", "type.market");

    add_site(w, "muscat", "sultan_qaboos", "site.muscat.sultan_qaboos", "type.mosque");
    add_site(w, "muscat", "bimmah",        "site.muscat.bimmah",        "type.landmark");
    add_site(w, "muscat", "royal_opera",   "site.muscat.royal_opera",   "type.landmark");
    add_site(w, "muscat", "mutrah_souq",   "site.muscat.mutrah_souq",   "type.market");

    add_site(w, "dakar", "grande_mosquee",      "site.dakar.grande_mosquee",      "type.mosque");
    add_site(w, "dakar", "goree",               "site.dakar.goree",               "type.landmark");
    add_site(w, "dakar", "african_renaissance", "site.dakar.african_renaissance", "type.landmark");
    add_site(w, "dakar", "ifan",                "site.dakar.ifan",                "type.museum");

    add_site(w, "isfahan", "naqsh_e_jahan",    "site.isfahan.naqsh_e_jahan",    "type.landmark");
    add_site(w, "isfahan", "sheikh_lotfollah", "site.isfahan.sheikh_lotfollah", "type.mosque");
    add_site(w, "isfahan", "vank",             "site.isfahan.vank",             "type.landmark");
    add_site(w, "isfahan", "si_o_se_pol",      "site.isfahan.si_o_se_pol",      "type.cafe");

    add_site(w, "dhaka", "baitul_mukarram", "site.dhaka.baitul_mukarram", "type.mosque");
    add_site(w, "dhaka", "shakhari_bazaar", "site.dhaka.shakhari_bazaar", "type.market");
    add_site(w, "dhaka", "lalbagh",         "site.dhaka.lalbagh",         "type.landmark");
    add_site(w, "dhaka", "national_museum", "site.dhaka.national_museum", "type.museum");

    add_site(w, "fez", "qarawiyyin", "site.fez.qarawiyyin", "type.mosque");
    add_site(w, "fez", "fez_medina", "site.fez.fez_medina", "type.market");
    add_site(w, "fez", "borj_nord",  "site.fez.borj_nord",  "type.landmark");
    add_site(w, "fez", "jnan_sbil",  "site.fez.jnan_sbil",  "type.park");

    add_site(w, "zanzibar", "stone_town", "site.zanzibar.stone_town", "type.landmark");
    add_site(w, "zanzibar", "hamamni",    "site.zanzibar.hamamni",    "type.landmark");
    add_site(w, "zanzibar", "malindi",    "site.zanzibar.malindi",    "type.mosque");
    add_site(w, "zanzibar", "forodhani",  "site.zanzibar.forodhani",  "type.market");

    add_site(w, "hyderabad", "charminar",    "site.hyderabad.charminar",    "type.landmark");
    add_site(w, "hyderabad", "mecca_masjid", "site.hyderabad.mecca_masjid", "type.mosque");
    add_site(w, "hyderabad", "salar_jung",   "site.hyderabad.salar_jung",   "type.museum");
    add_site(w, "hyderabad", "laad_bazaar",  "site.hyderabad.laad_bazaar",  "type.market");

    add_site(w, "tunis", "zitouna",  "site.tunis.zitouna",  "type.mosque");
    add_site(w, "tunis", "sidi_bou_said", "site.tunis.sidi_bou_said", "type.cafe");
    add_site(w, "tunis", "bardo",    "site.tunis.bardo",    "type.museum");
    add_site(w, "tunis", "carthage", "site.tunis.carthage", "type.landmark");

    add_site(w, "damascus", "umayyad",         "site.damascus.umayyad",         "type.mosque");
    add_site(w, "damascus", "hamidiyah",       "site.damascus.hamidiyah",       "type.market");
    add_site(w, "damascus", "nawfara",         "site.damascus.nawfara",         "type.cafe");
    add_site(w, "damascus", "azm_palace",      "site.damascus.azm_palace",      "type.landmark");

    add_site(w, "kano", "great_mosque", "site.kano.great_mosque", "type.mosque");
    add_site(w, "kano", "kurmi_market", "site.kano.kurmi_market", "type.market");
    add_site(w, "kano", "gidan_makama", "site.kano.gidan_makama", "type.museum");
    add_site(w, "kano", "dala_hill",    "site.kano.dala_hill",    "type.landmark");

    add_site(w, "samarkand", "bibi_khanym",  "site.samarkand.bibi_khanym",  "type.mosque");
    add_site(w, "samarkand", "siyob_bazaar", "site.samarkand.siyob_bazaar", "type.market");
    add_site(w, "samarkand", "registan",     "site.samarkand.registan",     "type.landmark");
    add_site(w, "samarkand", "shah_i_zinda", "site.samarkand.shah_i_zinda", "type.landmark");

    add_site(w, "kabul", "id_gah",          "site.kabul.id_gah",          "type.mosque");
    add_site(w, "kabul", "national_museum", "site.kabul.national_museum", "type.museum");
    add_site(w, "kabul", "babur_gardens",   "site.kabul.babur_gardens",   "type.park");
    add_site(w, "kabul", "darul_aman",      "site.kabul.darul_aman",      "type.landmark");

    add_site(w, "yogyakarta", "gede_kauman", "site.yogyakarta.gede_kauman", "type.mosque");
    add_site(w, "yogyakarta", "malioboro",   "site.yogyakarta.malioboro",   "type.market");
    add_site(w, "yogyakarta", "taman_sari",  "site.yogyakarta.taman_sari",  "type.park");
    add_site(w, "yogyakarta", "kraton",      "site.yogyakarta.kraton",      "type.landmark");

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
    { const char *k[] = {"clue.fez.inbound.0", "clue.fez.inbound.1", "clue.fez.inbound.2", "clue.fez.inbound.3"};
      add_inbound_clues(w, "fez", k, 4); }
    { const char *k[] = {"clue.dakar.inbound.0", "clue.dakar.inbound.1", "clue.dakar.inbound.2", "clue.dakar.inbound.3"};
      add_inbound_clues(w, "dakar", k, 4); }
    { const char *k[] = {"clue.dubai.inbound.0", "clue.dubai.inbound.1", "clue.dubai.inbound.2", "clue.dubai.inbound.3"};
      add_inbound_clues(w, "dubai", k, 4); }
    { const char *k[] = {"clue.zanzibar.inbound.0", "clue.zanzibar.inbound.1", "clue.zanzibar.inbound.2", "clue.zanzibar.inbound.3"};
      add_inbound_clues(w, "zanzibar", k, 4); }
    { const char *k[] = {"clue.hyderabad.inbound.0", "clue.hyderabad.inbound.1", "clue.hyderabad.inbound.2", "clue.hyderabad.inbound.3"};
      add_inbound_clues(w, "hyderabad", k, 4); }
    { const char *k[] = {"clue.muscat.inbound.0", "clue.muscat.inbound.1", "clue.muscat.inbound.2", "clue.muscat.inbound.3"};
      add_inbound_clues(w, "muscat", k, 4); }
    { const char *k[] = {"clue.isfahan.inbound.0", "clue.isfahan.inbound.1", "clue.isfahan.inbound.2", "clue.isfahan.inbound.3"};
      add_inbound_clues(w, "isfahan", k, 4); }
    { const char *k[] = {"clue.konya.inbound.0", "clue.konya.inbound.1", "clue.konya.inbound.2", "clue.konya.inbound.3"};
      add_inbound_clues(w, "konya", k, 4); }
    { const char *k[] = {"clue.lahore.inbound.0", "clue.lahore.inbound.1", "clue.lahore.inbound.2", "clue.lahore.inbound.3"};
      add_inbound_clues(w, "lahore", k, 4); }
    { const char *k[] = {"clue.dhaka.inbound.0", "clue.dhaka.inbound.1", "clue.dhaka.inbound.2", "clue.dhaka.inbound.3"};
      add_inbound_clues(w, "dhaka", k, 4); }
    { const char *k[] = {"clue.kuala_lumpur.inbound.0", "clue.kuala_lumpur.inbound.1", "clue.kuala_lumpur.inbound.2", "clue.kuala_lumpur.inbound.3"};
      add_inbound_clues(w, "kuala_lumpur", k, 4); }
    { const char *k[] = {"clue.jakarta.inbound.0", "clue.jakarta.inbound.1", "clue.jakarta.inbound.2", "clue.jakarta.inbound.3"};
      add_inbound_clues(w, "jakarta", k, 4); }
    { const char *k[] = {"clue.tunis.inbound.0", "clue.tunis.inbound.1", "clue.tunis.inbound.2", "clue.tunis.inbound.3"};
      add_inbound_clues(w, "tunis", k, 4); }
    { const char *k[] = {"clue.damascus.inbound.0", "clue.damascus.inbound.1", "clue.damascus.inbound.2", "clue.damascus.inbound.3"};
      add_inbound_clues(w, "damascus", k, 4); }
    { const char *k[] = {"clue.kano.inbound.0", "clue.kano.inbound.1", "clue.kano.inbound.2", "clue.kano.inbound.3"};
      add_inbound_clues(w, "kano", k, 4); }
    { const char *k[] = {"clue.samarkand.inbound.0", "clue.samarkand.inbound.1", "clue.samarkand.inbound.2", "clue.samarkand.inbound.3"};
      add_inbound_clues(w, "samarkand", k, 4); }
    { const char *k[] = {"clue.kabul.inbound.0", "clue.kabul.inbound.1", "clue.kabul.inbound.2", "clue.kabul.inbound.3"};
      add_inbound_clues(w, "kabul", k, 4); }
    { const char *k[] = {"clue.yogyakarta.inbound.0", "clue.yogyakarta.inbound.1", "clue.yogyakarta.inbound.2", "clue.yogyakarta.inbound.3"};
      add_inbound_clues(w, "yogyakarta", k, 4); }

    /* ---------------------------------------------------------- connections */
    /* This graph mirrors the reference network in
       doc/future/cross-sphere-connectivity.md: 22 cities, 46 undirected
       edges, every city in the 3-5 degree range and reaching at least two
       other cultural sub-spheres.  Distances are great-circle estimates. */

    /* A - Maghreb (Casablanca, Fez, Tunis) */
    add_route(w, "tunis",      "fez",          1350, "flight");

    /* B - Turkey / Balkans (Istanbul, Konya, Sarajevo) */
    add_route(w, "istanbul",   "sarajevo",     950,  "flight");

    /* C - Arab East (Cairo, Damascus, Dubai, Muscat) */
    add_route(w, "dubai",      "muscat",       450,  "boat");

    /* D - Sub-Saharan Africa (Dakar, Zanzibar, Kano) */
    add_route(w, "zanzibar",   "dakar",        6200, "flight");
    add_route(w, "dakar",      "kano",         2850, "flight");

    /* E - Iran / Central Asia (Isfahan, Samarkand, Kabul) */
    add_route(w, "isfahan",    "samarkand",    1650, "flight");
    add_route(w, "kabul",      "samarkand",    600,  "train");
    add_route(w, "kabul",      "isfahan",      1600, "flight");

    /* F - South Asia (Lahore, Dhaka, Hyderabad) */
    add_route(w, "lahore",     "dhaka",        1800, "flight");
    add_route(w, "hyderabad",  "lahore",       1500, "flight");
    add_route(w, "hyderabad",  "dhaka",        1500, "flight");

    /* G - Southeast Asia (Kuala Lumpur, Jakarta, Yogyakarta) */
    add_route(w, "kuala_lumpur","jakarta",     1200, "flight");
    add_route(w, "yogyakarta", "kuala_lumpur", 1550, "flight");

    /* Cross-sphere: Turkey/Balkans - Arab East - Maghreb */
    add_route(w, "istanbul",   "cairo",        1250, "flight");
    add_route(w, "istanbul",   "casablanca",   3150, "flight");
    add_route(w, "istanbul",   "kabul",        3700, "flight");
    add_route(w, "cairo",      "casablanca",   3350, "flight");

    /* Cross-sphere: Arab East - Sub-Saharan Africa */
    add_route(w, "cairo",      "zanzibar",     3800, "flight");
    add_route(w, "cairo",      "dakar",        4700, "flight");
    add_route(w, "cairo",      "kano",         3050, "flight");
    add_route(w, "casablanca", "dakar",        2600, "flight");

    /* Cross-sphere: Maghreb links */
    add_route(w, "fez",        "sarajevo",     2200, "flight");
    add_route(w, "fez",        "kuala_lumpur", 11400,"flight");
    add_route(w, "fez",        "kano",         2700, "flight");
    add_route(w, "fez",        "damascus",     3800, "flight");

    /* Cross-sphere: Gulf - South Asia / Iran / Southeast Asia */
    add_route(w, "dubai",      "lahore",       2400, "flight");
    add_route(w, "dubai",      "isfahan",      1200, "flight");
    add_route(w, "dubai",      "yogyakarta",   7300, "flight");

    /* Cross-sphere: Iran / Central Asia links */
    add_route(w, "isfahan",    "konya",        2200, "flight");
    add_route(w, "samarkand",  "lahore",       1150, "flight");
    add_route(w, "samarkand",  "damascus",     2800, "flight");

    /* Cross-sphere: South Asia - Southeast Asia */
    add_route(w, "dhaka",      "kuala_lumpur", 3200, "flight");
    add_route(w, "hyderabad",  "kuala_lumpur", 3900, "flight");

    /* Cross-sphere: Tunis reach */
    add_route(w, "tunis",      "sarajevo",     1050, "flight");
    add_route(w, "tunis",      "konya",        1950, "flight");
    add_route(w, "tunis",      "dhaka",        8600, "flight");

    /* Cross-sphere: Damascus reach */
    add_route(w, "damascus",   "isfahan",      1450, "flight");
    add_route(w, "damascus",   "konya",        650,  "train");
    add_route(w, "damascus",   "sarajevo",     2050, "flight");

    /* Cross-sphere: Kabul / South Asia */
    add_route(w, "kabul",      "lahore",       640,  "train");
    add_route(w, "kabul",      "hyderabad",    2100, "flight");

    /* Cross-sphere: Southeast Asia - South Asia - Arab East */
    add_route(w, "jakarta",    "dhaka",        3400, "flight");
    add_route(w, "jakarta",    "hyderabad",    4300, "flight");
    add_route(w, "jakarta",    "muscat",       6600, "flight");
    add_route(w, "muscat",     "zanzibar",     3200, "boat");
    add_route(w, "zanzibar",   "yogyakarta",   7900, "flight");
}
