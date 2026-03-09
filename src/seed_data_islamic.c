#include "carmen/seed_data_islamic.h"
#include "seed_helpers.h"

/* ---- world builder: Modern Muslim Traveler ---- */
/* Clue design: each site has 2 positive clues pointing to 2 DIFFERENT
   connected cities, plus 1 negative (dead end).  This lets the game
   engine pick 2 sites that converge on the same destination. */

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

    /* -------------------------------------------------------- sites & clues */

    /* Istanbul  (connections: sarajevo, cairo, konya, beirut)
       Suleymaniye→sarajevo+cairo  Istanbul Modern→cairo+konya
       Kadikoy→konya+beirut        Camlica→beirut+sarajevo       */
    { ClueData c[] = {{"clue.istanbul.suleymaniye.0",      "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.suleymaniye.1",      "cairo",    CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.suleymaniye.2",      NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "suleymaniye", "site.istanbul.suleymaniye", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.istanbul.istanbul_modern.0",  "cairo",    CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.istanbul_modern.1",  "konya",    CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.istanbul_modern.2",  NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "istanbul_modern", "site.istanbul.istanbul_modern", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.istanbul.kadikoy.0",          "konya",    CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.kadikoy.1",          "beirut",   CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.kadikoy.2",          NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "kadikoy", "site.istanbul.kadikoy", "type.market", c, 3); }
    { ClueData c[] = {{"clue.istanbul.camlica.0",          "beirut",   CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.camlica.1",          "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"clue.istanbul.camlica.2",          NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "camlica", "site.istanbul.camlica", "type.mosque", c, 3); }

    /* Dubai  (connections: doha, muscat, abu_dhabi, isfahan, lahore, amman)
       Museum→doha+muscat      Al Fahidi→muscat+lahore
       Dubai Mall→lahore+isfahan  Jumeirah→isfahan+doha          */
    { ClueData c[] = {{"clue.dubai.museum_future.0",       "doha",    CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.museum_future.1",       "muscat",  CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.museum_future.2",       NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "museum_future", "site.dubai.museum_future", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.dubai.al_fahidi.0",           "muscat",  CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.al_fahidi.1",           "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.al_fahidi.2",           NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "al_fahidi", "site.dubai.al_fahidi", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.dubai.dubai_mall.0",          "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.dubai_mall.1",          "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.dubai_mall.2",          NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "dubai_mall", "site.dubai.dubai_mall", "type.market", c, 3); }
    { ClueData c[] = {{"clue.dubai.jumeirah.0",            "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.jumeirah.1",            "doha",    CARMEN_CLUE_POSITIVE},
                      {"clue.dubai.jumeirah.2",            NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "jumeirah", "site.dubai.jumeirah", "type.mosque", c, 3); }

    /* Kuala Lumpur  (connections: bandung, jakarta, dhaka, hyderabad)
       Petronas→bandung+hyderabad  Islamic Arts→jakarta+dhaka
       Jalan Alor→dhaka+hyderabad  Masjid Negara→jakarta+bandung  */
    { ClueData c[] = {{"clue.kuala_lumpur.petronas.0",     "bandung",      CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.petronas.1",     "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.petronas.2",     NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "petronas", "site.kuala_lumpur.petronas", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.kuala_lumpur.islamic_arts.0", "jakarta",     CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.islamic_arts.1", "dhaka",       CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.islamic_arts.2", NULL,          CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "islamic_arts", "site.kuala_lumpur.islamic_arts", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.kuala_lumpur.jalan_alor.0",   "dhaka",       CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.jalan_alor.1",   "hyderabad",   CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.jalan_alor.2",   NULL,          CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "jalan_alor", "site.kuala_lumpur.jalan_alor", "type.market", c, 3); }
    { ClueData c[] = {{"clue.kuala_lumpur.masjid_negara.0", "jakarta",     CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.masjid_negara.1", "bandung",     CARMEN_CLUE_POSITIVE},
                      {"clue.kuala_lumpur.masjid_negara.2", NULL,          CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "masjid_negara", "site.kuala_lumpur.masjid_negara", "type.mosque", c, 3); }

    /* Doha  (connections: dubai, abu_dhabi)
       Museum→abu_dhabi+dubai   Souq Waqif→dubai+abu_dhabi
       Katara→abu_dhabi+dubai   Education→dubai+abu_dhabi      */
    { ClueData c[] = {{"clue.doha.islamic_art.0",          "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"clue.doha.islamic_art.1",          "dubai",     CARMEN_CLUE_POSITIVE},
                      {"clue.doha.islamic_art.2",          NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "islamic_art", "site.doha.islamic_art", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.doha.souq_waqif.0",          "dubai",     CARMEN_CLUE_POSITIVE},
                      {"clue.doha.souq_waqif.1",          "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"clue.doha.souq_waqif.2",          NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "souq_waqif", "site.doha.souq_waqif", "type.market", c, 3); }
    { ClueData c[] = {{"clue.doha.katara.0",              "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"clue.doha.katara.1",              "dubai",     CARMEN_CLUE_POSITIVE},
                      {"clue.doha.katara.2",              NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "katara", "site.doha.katara", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.doha.education_city.0",      "dubai",     CARMEN_CLUE_POSITIVE},
                      {"clue.doha.education_city.1",      "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"clue.doha.education_city.2",      NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "education_city", "site.doha.education_city", "type.landmark", c, 3); }

    /* Amman  (connections: cairo, beirut, dubai)
       Citadel→cairo+beirut  Rainbow→beirut+dubai  King Abdullah→dubai+cairo */
    { ClueData c[] = {{"clue.amman.citadel.0",            "cairo",  CARMEN_CLUE_POSITIVE},
                      {"clue.amman.citadel.1",            "beirut", CARMEN_CLUE_POSITIVE},
                      {"clue.amman.citadel.2",            NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "citadel", "site.amman.citadel", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.amman.rainbow_street.0",     "beirut", CARMEN_CLUE_POSITIVE},
                      {"clue.amman.rainbow_street.1",     "dubai",  CARMEN_CLUE_POSITIVE},
                      {"clue.amman.rainbow_street.2",     NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "rainbow_street", "site.amman.rainbow_street", "type.cafe", c, 3); }
    { ClueData c[] = {{"clue.amman.king_abdullah.0",      "dubai",  CARMEN_CLUE_POSITIVE},
                      {"clue.amman.king_abdullah.1",      "cairo",  CARMEN_CLUE_POSITIVE},
                      {"clue.amman.king_abdullah.2",      NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "king_abdullah", "site.amman.king_abdullah", "type.mosque", c, 3); }

    /* Casablanca  (connections: marrakech, fez, dakar, cairo)
       Hassan II→marrakech+dakar  Habous→dakar+cairo
       Morocco Mall→cairo+fez     Art Deco→fez+marrakech         */
    { ClueData c[] = {{"clue.casablanca.hassan_ii.0",     "marrakech", CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.hassan_ii.1",     "dakar",     CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.hassan_ii.2",     NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "hassan_ii", "site.casablanca.hassan_ii", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.casablanca.habous.0",        "dakar",     CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.habous.1",        "cairo",     CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.habous.2",        NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "habous", "site.casablanca.habous", "type.market", c, 3); }
    { ClueData c[] = {{"clue.casablanca.morocco_mall.0",  "cairo",     CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.morocco_mall.1",  "fez",       CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.morocco_mall.2",  NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "morocco_mall", "site.casablanca.morocco_mall", "type.market", c, 3); }
    { ClueData c[] = {{"clue.casablanca.art_deco.0",      "fez",       CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.art_deco.1",      "marrakech", CARMEN_CLUE_POSITIVE},
                      {"clue.casablanca.art_deco.2",      NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "art_deco", "site.casablanca.art_deco", "type.landmark", c, 3); }

    /* Jakarta  (connections: kuala_lumpur, bandung)
       Istiqlal→kuala_lumpur+bandung  National Museum→bandung+kuala_lumpur
       Kota Tua→kuala_lumpur+bandung  Menteng→bandung+kuala_lumpur  */
    { ClueData c[] = {{"clue.jakarta.istiqlal.0",         "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.istiqlal.1",         "bandung",      CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.istiqlal.2",         NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "istiqlal", "site.jakarta.istiqlal", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.jakarta.national_museum.0",  "bandung",      CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.national_museum.1",  "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.national_museum.2",  NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "national_museum", "site.jakarta.national_museum", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.jakarta.kota_tua.0",         "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.kota_tua.1",         "bandung",      CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.kota_tua.2",         NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "kota_tua", "site.jakarta.kota_tua", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.jakarta.menteng.0",          "bandung",      CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.menteng.1",          "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.jakarta.menteng.2",          NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "menteng", "site.jakarta.menteng", "type.cafe", c, 3); }

    /* Sarajevo  (connections: istanbul, fez)
       Bascarsija→istanbul+fez  Gazi Husrev-beg→fez+istanbul
       War Tunnel→istanbul+fez                                 */
    { ClueData c[] = {{"clue.sarajevo.bascarsija.0",      "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.bascarsija.1",      "fez",      CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.bascarsija.2",      NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "bascarsija", "site.sarajevo.bascarsija", "type.market", c, 3); }
    { ClueData c[] = {{"clue.sarajevo.gazi_husrev.0",     "fez",      CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.gazi_husrev.1",     "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.gazi_husrev.2",     NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "gazi_husrev", "site.sarajevo.gazi_husrev", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.sarajevo.war_tunnel.0",      "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.war_tunnel.1",      "fez",      CARMEN_CLUE_POSITIVE},
                      {"clue.sarajevo.war_tunnel.2",      NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "war_tunnel", "site.sarajevo.war_tunnel", "type.museum", c, 3); }

    /* Tashkent  (connections: isfahan, lahore)
       Khast Imam→isfahan+lahore  Chorsu→lahore+isfahan
       Metro→lahore+isfahan       Minor→isfahan+lahore          */
    { ClueData c[] = {{"clue.tashkent.khast_imam.0",      "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.khast_imam.1",      "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.khast_imam.2",      NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "khast_imam", "site.tashkent.khast_imam", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.tashkent.chorsu.0",          "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.chorsu.1",          "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.chorsu.2",          NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "chorsu", "site.tashkent.chorsu", "type.market", c, 3); }
    { ClueData c[] = {{"clue.tashkent.tashkent_metro.0",  "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.tashkent_metro.1",  "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.tashkent_metro.2",  NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "tashkent_metro", "site.tashkent.tashkent_metro", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.tashkent.minor_mosque.0",    "isfahan", CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.minor_mosque.1",    "lahore",  CARMEN_CLUE_POSITIVE},
                      {"clue.tashkent.minor_mosque.2",    NULL,      CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "minor_mosque", "site.tashkent.minor_mosque", "type.mosque", c, 3); }

    /* Cairo  (connections: amman, casablanca, abu_dhabi, zanzibar, dakar)
       Al-Azhar→amman+casablanca   Tahrir→casablanca+abu_dhabi
       Khan el-Khalili→abu_dhabi+amman  Sultan Hassan→amman+casablanca */
    { ClueData c[] = {{"clue.cairo.azhar_park.0",         "amman",      CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.azhar_park.1",         "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.azhar_park.2",         NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "azhar_park", "site.cairo.azhar_park", "type.park", c, 3); }
    { ClueData c[] = {{"clue.cairo.tahrir.0",             "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.tahrir.1",             "abu_dhabi",  CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.tahrir.2",             NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "tahrir", "site.cairo.tahrir", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.cairo.khan_khalili.0",       "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.khan_khalili.1",       "amman",     CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.khan_khalili.2",       NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "khan_khalili", "site.cairo.khan_khalili", "type.market", c, 3); }
    { ClueData c[] = {{"clue.cairo.sultan_hassan.0",      "amman",      CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.sultan_hassan.1",      "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.cairo.sultan_hassan.2",      NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "sultan_hassan", "site.cairo.sultan_hassan", "type.mosque", c, 3); }

    /* Beirut  (connections: amman, istanbul)
       Mohammad Al-Amin→amman+istanbul  Gemmayzeh→istanbul+amman
       National Museum→amman+istanbul                          */
    { ClueData c[] = {{"clue.beirut.al_amin.0",           "amman",    CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.al_amin.1",           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.al_amin.2",           NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "al_amin", "site.beirut.al_amin", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.beirut.gemmayzeh.0",         "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.gemmayzeh.1",         "amman",    CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.gemmayzeh.2",         NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "gemmayzeh", "site.beirut.gemmayzeh", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.beirut.national_museum.0",   "amman",    CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.national_museum.1",   "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.beirut.national_museum.2",   NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "national_museum", "site.beirut.national_museum", "type.museum", c, 3); }

    /* Marrakech  (connections: casablanca, fez)
       Jemaa→fez+casablanca  MACAAL→casablanca+fez
       Le Jardin→fez+casablanca                                */
    { ClueData c[] = {{"clue.marrakech.jemaa.0",          "fez",        CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.jemaa.1",          "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.jemaa.2",          NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "jemaa", "site.marrakech.jemaa", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.marrakech.macaal.0",         "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.macaal.1",         "fez",        CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.macaal.2",         NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "macaal", "site.marrakech.macaal", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.marrakech.le_jardin.0",      "fez",        CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.le_jardin.1",      "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.marrakech.le_jardin.2",      NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "le_jardin", "site.marrakech.le_jardin", "type.park", c, 3); }

    /* Lahore  (connections: dhaka, tashkent, dubai, hyderabad)
       Badshahi→dhaka+tashkent  Fort→tashkent+dubai
       Food Street→dubai+hyderabad  Anarkali→hyderabad+dhaka   */
    { ClueData c[] = {{"clue.lahore.badshahi.0",          "dhaka",     CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.badshahi.1",          "tashkent",  CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.badshahi.2",          NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "badshahi", "site.lahore.badshahi", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.lahore.lahore_fort.0",       "tashkent", CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.lahore_fort.1",       "dubai",    CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.lahore_fort.2",       NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "lahore_fort", "site.lahore.lahore_fort", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.lahore.food_street.0",       "dubai",     CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.food_street.1",       "hyderabad", CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.food_street.2",       NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "food_street", "site.lahore.food_street", "type.market", c, 3); }
    { ClueData c[] = {{"clue.lahore.anarkali.0",          "hyderabad", CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.anarkali.1",          "dhaka",     CARMEN_CLUE_POSITIVE},
                      {"clue.lahore.anarkali.2",          NULL,        CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "anarkali", "site.lahore.anarkali", "type.market", c, 3); }

    /* Konya  (connections: istanbul, isfahan)
       Mevlana→isfahan+istanbul  Alaeddin→istanbul+isfahan
       Sille→istanbul+isfahan                                  */
    { ClueData c[] = {{"clue.konya.mevlana.0",            "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"clue.konya.mevlana.1",            "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.konya.mevlana.2",            NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "mevlana", "site.konya.mevlana", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.konya.alaeddin.0",           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.konya.alaeddin.1",           "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"clue.konya.alaeddin.2",           NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "alaeddin", "site.konya.alaeddin", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.konya.sille.0",              "istanbul", CARMEN_CLUE_POSITIVE},
                      {"clue.konya.sille.1",              "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"clue.konya.sille.2",              NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "sille", "site.konya.sille", "type.landmark", c, 3); }

    /* Abu Dhabi  (connections: dubai, doha, muscat, cairo)
       Zayed→muscat+doha  Louvre→doha+cairo
       Mangrove→cairo+dubai  Qasr→dubai+muscat                */
    { ClueData c[] = {{"clue.abu_dhabi.sheikh_zayed.0",   "muscat", CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.sheikh_zayed.1",   "doha",   CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.sheikh_zayed.2",   NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "sheikh_zayed", "site.abu_dhabi.sheikh_zayed", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.abu_dhabi.louvre_ad.0",      "doha",   CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.louvre_ad.1",      "cairo",  CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.louvre_ad.2",      NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "louvre_ad", "site.abu_dhabi.louvre_ad", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.abu_dhabi.mangrove_park.0",  "cairo",  CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.mangrove_park.1",  "dubai",  CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.mangrove_park.2",  NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "mangrove_park", "site.abu_dhabi.mangrove_park", "type.park", c, 3); }
    { ClueData c[] = {{"clue.abu_dhabi.qasr_watan.0",     "dubai",  CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.qasr_watan.1",     "muscat", CARMEN_CLUE_POSITIVE},
                      {"clue.abu_dhabi.qasr_watan.2",     NULL,     CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "qasr_watan", "site.abu_dhabi.qasr_watan", "type.landmark", c, 3); }

    /* Muscat  (connections: dubai, abu_dhabi, dhaka, zanzibar)
       Sultan Qaboos→zanzibar+dubai  Bimmah→dhaka+zanzibar
       Royal Opera→dubai+dhaka                                 */
    { ClueData c[] = {{"clue.muscat.sultan_qaboos.0",     "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.sultan_qaboos.1",     "dubai",    CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.sultan_qaboos.2",     NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "sultan_qaboos", "site.muscat.sultan_qaboos", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.muscat.bimmah.0",            "dhaka",    CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.bimmah.1",            "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.bimmah.2",            NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "bimmah", "site.muscat.bimmah", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.muscat.royal_opera.0",       "dubai",    CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.royal_opera.1",       "dhaka",    CARMEN_CLUE_POSITIVE},
                      {"clue.muscat.royal_opera.2",       NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "royal_opera", "site.muscat.royal_opera", "type.landmark", c, 3); }

    /* Dakar  (connections: casablanca, cairo, zanzibar)
       Grande Mosquee→casablanca+cairo  Goree→cairo+zanzibar
       African Renaissance→zanzibar+casablanca                 */
    { ClueData c[] = {{"clue.dakar.grande_mosquee.0",     "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.grande_mosquee.1",     "cairo",      CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.grande_mosquee.2",     NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "grande_mosquee", "site.dakar.grande_mosquee", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.dakar.goree.0",              "cairo",      CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.goree.1",              "zanzibar",   CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.goree.2",              NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "goree", "site.dakar.goree", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.dakar.african_renaissance.0", "zanzibar",   CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.african_renaissance.1", "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.dakar.african_renaissance.2", NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "african_renaissance", "site.dakar.african_renaissance", "type.landmark", c, 3); }

    /* Bandung  (connections: kuala_lumpur, jakarta)
       Braga→kuala_lumpur+jakarta  Masjid Raya→jakarta+kuala_lumpur
       Tangkuban→kuala_lumpur+jakarta                          */
    { ClueData c[] = {{"clue.bandung.braga.0",            "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.braga.1",            "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.braga.2",            NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "braga", "site.bandung.braga", "type.cafe", c, 3); }
    { ClueData c[] = {{"clue.bandung.masjid_raya.0",      "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.masjid_raya.1",      "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.masjid_raya.2",      NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "masjid_raya", "site.bandung.masjid_raya", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.bandung.tangkuban.0",        "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.tangkuban.1",        "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.bandung.tangkuban.2",        NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "tangkuban", "site.bandung.tangkuban", "type.landmark", c, 3); }

    /* Isfahan  (connections: dubai, konya, tashkent)
       Naqsh-e Jahan→dubai+konya  Vank→konya+tashkent
       Si-o-se-pol→tashkent+dubai                              */
    { ClueData c[] = {{"clue.isfahan.naqsh_e_jahan.0",    "dubai",    CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.naqsh_e_jahan.1",    "konya",    CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.naqsh_e_jahan.2",    NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "naqsh_e_jahan", "site.isfahan.naqsh_e_jahan", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.isfahan.vank.0",             "konya",    CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.vank.1",             "tashkent", CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.vank.2",             NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "vank", "site.isfahan.vank", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.isfahan.si_o_se_pol.0",      "tashkent", CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.si_o_se_pol.1",      "dubai",    CARMEN_CLUE_POSITIVE},
                      {"clue.isfahan.si_o_se_pol.2",      NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "si_o_se_pol", "site.isfahan.si_o_se_pol", "type.cafe", c, 3); }

    /* Dhaka  (connections: lahore, kuala_lumpur, muscat, hyderabad)
       Baitul Mukarram→kuala_lumpur+lahore  Star Mosque→lahore+muscat
       Lalbagh→muscat+hyderabad  Sadarghat→hyderabad+kuala_lumpur */
    { ClueData c[] = {{"clue.dhaka.baitul_mukarram.0",    "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.baitul_mukarram.1",    "lahore",       CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.baitul_mukarram.2",    NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "baitul_mukarram", "site.dhaka.baitul_mukarram", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.dhaka.star_mosque.0",        "lahore",       CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.star_mosque.1",        "muscat",       CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.star_mosque.2",        NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "star_mosque", "site.dhaka.star_mosque", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.dhaka.lalbagh.0",            "muscat",       CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.lalbagh.1",            "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.lalbagh.2",            NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "lalbagh", "site.dhaka.lalbagh", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.dhaka.sadarghat.0",          "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.sadarghat.1",          "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.dhaka.sadarghat.2",          NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "sadarghat", "site.dhaka.sadarghat", "type.landmark", c, 3); }

    /* Fez  (connections: casablanca, marrakech, sarajevo)
       Al-Qarawiyyin→sarajevo+casablanca  Medina→marrakech+sarajevo
       Borj Nord→casablanca+marrakech                          */
    { ClueData c[] = {{"clue.fez.qarawiyyin.0",           "sarajevo",   CARMEN_CLUE_POSITIVE},
                      {"clue.fez.qarawiyyin.1",           "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.fez.qarawiyyin.2",           NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "qarawiyyin", "site.fez.qarawiyyin", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.fez.fez_medina.0",           "marrakech",  CARMEN_CLUE_POSITIVE},
                      {"clue.fez.fez_medina.1",           "sarajevo",   CARMEN_CLUE_POSITIVE},
                      {"clue.fez.fez_medina.2",           NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "fez_medina", "site.fez.fez_medina", "type.market", c, 3); }
    { ClueData c[] = {{"clue.fez.borj_nord.0",            "casablanca", CARMEN_CLUE_POSITIVE},
                      {"clue.fez.borj_nord.1",            "marrakech",  CARMEN_CLUE_POSITIVE},
                      {"clue.fez.borj_nord.2",            NULL,         CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "borj_nord", "site.fez.borj_nord", "type.landmark", c, 3); }

    /* Zanzibar  (connections: cairo, muscat, dakar)
       Stone Town→muscat+dakar  Hamamni→dakar+cairo
       Malindi→cairo+muscat  Forodhani→muscat+cairo            */
    { ClueData c[] = {{"clue.zanzibar.stone_town.0",      "muscat",   CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.stone_town.1",      "dakar",    CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.stone_town.2",      NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "stone_town", "site.zanzibar.stone_town", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.zanzibar.hamamni.0",         "dakar",    CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.hamamni.1",         "cairo",    CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.hamamni.2",         NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "hamamni", "site.zanzibar.hamamni", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.zanzibar.malindi.0",         "cairo",    CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.malindi.1",         "muscat",   CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.malindi.2",         NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "malindi", "site.zanzibar.malindi", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.zanzibar.forodhani.0",       "muscat",   CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.forodhani.1",       "cairo",    CARMEN_CLUE_POSITIVE},
                      {"clue.zanzibar.forodhani.2",       NULL,       CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "forodhani", "site.zanzibar.forodhani", "type.market", c, 3); }

    /* Hyderabad  (connections: dubai, lahore, dhaka, kuala_lumpur)
       Charminar→lahore+dubai  Mecca Masjid→dubai+kuala_lumpur
       Salar Jung→kuala_lumpur+dhaka  Laad Bazaar→dhaka+lahore */
    { ClueData c[] = {{"clue.hyderabad.charminar.0",      "lahore",       CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.charminar.1",      "dubai",        CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.charminar.2",      NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "charminar", "site.hyderabad.charminar", "type.landmark", c, 3); }
    { ClueData c[] = {{"clue.hyderabad.mecca_masjid.0",   "dubai",        CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.mecca_masjid.1",   "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.mecca_masjid.2",   NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "mecca_masjid", "site.hyderabad.mecca_masjid", "type.mosque", c, 3); }
    { ClueData c[] = {{"clue.hyderabad.salar_jung.0",     "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.salar_jung.1",     "dhaka",        CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.salar_jung.2",     NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "salar_jung", "site.hyderabad.salar_jung", "type.museum", c, 3); }
    { ClueData c[] = {{"clue.hyderabad.laad_bazaar.0",    "dhaka",        CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.laad_bazaar.1",    "lahore",       CARMEN_CLUE_POSITIVE},
                      {"clue.hyderabad.laad_bazaar.2",    NULL,           CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "laad_bazaar", "site.hyderabad.laad_bazaar", "type.market", c, 3); }

    /* ---------------------------------------------------------- connections */
    /* Europe */
    add_route(w, "istanbul",   "sarajevo",     950);
    add_route(w, "istanbul",   "konya",        660);

    /* Europe - Middle East */
    add_route(w, "istanbul",   "cairo",        1250);
    add_route(w, "istanbul",   "beirut",       1100);

    /* Levant */
    add_route(w, "cairo",      "amman",        400);
    add_route(w, "beirut",     "amman",        280);

    /* Levant - Gulf */
    add_route(w, "amman",      "dubai",        2400);

    /* Gulf */
    add_route(w, "dubai",      "doha",         350);
    add_route(w, "dubai",      "muscat",       450);
    add_route(w, "dubai",      "abu_dhabi",    140);
    add_route(w, "doha",       "abu_dhabi",    320);
    add_route(w, "abu_dhabi",  "muscat",       470);

    /* Gulf - Iran / Central-South Asia */
    add_route(w, "dubai",      "isfahan",      1200);
    add_route(w, "dubai",      "lahore",       2400);

    /* Iran - Turkey / Central Asia */
    add_route(w, "isfahan",    "konya",        2200);
    add_route(w, "isfahan",    "tashkent",     2000);

    /* Central - South Asia */
    add_route(w, "tashkent",   "lahore",       2200);
    add_route(w, "lahore",     "dhaka",        1800);
    add_route(w, "muscat",     "dhaka",        4500);

    /* South - Southeast Asia */
    add_route(w, "dhaka",      "kuala_lumpur", 3200);
    add_route(w, "kuala_lumpur","jakarta",     1200);
    add_route(w, "kuala_lumpur","bandung",     1300);
    add_route(w, "jakarta",    "bandung",      150);

    /* North Africa */
    add_route(w, "cairo",      "casablanca",   3350);
    add_route(w, "cairo",      "abu_dhabi",    2400);
    add_route(w, "casablanca", "marrakech",    240);
    add_route(w, "casablanca", "fez",          300);
    add_route(w, "casablanca", "dakar",        2600);
    add_route(w, "marrakech",  "fez",          400);

    /* East Africa */
    add_route(w, "zanzibar",   "cairo",        3800);
    add_route(w, "zanzibar",   "muscat",       3200);
    add_route(w, "zanzibar",   "dakar",        6200);

    /* India */
    add_route(w, "hyderabad",  "dubai",        2700);
    add_route(w, "hyderabad",  "lahore",       1500);
    add_route(w, "hyderabad",  "dhaka",        1500);
    add_route(w, "hyderabad",  "kuala_lumpur", 3900);

    /* Cross-regional */
    add_route(w, "fez",        "sarajevo",     2200);
    add_route(w, "dakar",      "cairo",        4700);
}
