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
    carmen_world_add_city(w, "istanbul",     "Istanbul",      "İstanbul",          "Turkey",      "Europe",  41.01,   28.98);
    carmen_world_add_city(w, "sarajevo",     "Sarajevo",      "Sarajevo",          "Bosnia",      "Europe",  43.86,   18.41);
    carmen_world_add_city(w, "cairo",        "Cairo",         "القاهرة",           "Egypt",       "Africa",  30.04,   31.24);
    carmen_world_add_city(w, "casablanca",   "Casablanca",    "الدار البيضاء",     "Morocco",     "Africa",  33.57,   -7.59);
    carmen_world_add_city(w, "marrakech",    "Marrakech",     "مراكش",             "Morocco",     "Africa",  31.63,   -8.00);
    carmen_world_add_city(w, "fez",          "Fez",           "فاس",              "Morocco",     "Africa",  34.03,   -5.00);
    carmen_world_add_city(w, "dakar",        "Dakar",         "Dakar",             "Senegal",     "Africa",  14.72,  -17.47);
    carmen_world_add_city(w, "dubai",        "Dubai",         "دبي",              "UAE",         "Asia",    25.20,   55.27);
    carmen_world_add_city(w, "doha",         "Doha",          "الدوحة",            "Qatar",       "Asia",    25.29,   51.53);
    carmen_world_add_city(w, "abu_dhabi",    "Abu Dhabi",     "أبوظبي",            "UAE",         "Asia",    24.45,   54.65);
    carmen_world_add_city(w, "zanzibar",     "Zanzibar",      "زنجبار",            "Tanzania",    "Africa",  -6.16,   39.19);
    carmen_world_add_city(w, "hyderabad",    "Hyderabad",     "حیدرآباد",          "India",       "Asia",    17.38,   78.49);
    carmen_world_add_city(w, "muscat",       "Muscat",        "مسقط",              "Oman",        "Asia",    23.59,   58.55);
    carmen_world_add_city(w, "amman",        "Amman",         "عمّان",              "Jordan",      "Asia",    31.95,   35.93);
    carmen_world_add_city(w, "beirut",       "Beirut",        "بيروت",             "Lebanon",     "Asia",    33.89,   35.50);
    carmen_world_add_city(w, "isfahan",      "Isfahan",       "اصفهان",            "Iran",        "Asia",    32.65,   51.68);
    carmen_world_add_city(w, "konya",        "Konya",         "Konya",             "Turkey",      "Asia",    37.87,   32.48);
    carmen_world_add_city(w, "tashkent",     "Tashkent",      "Тошкент",           "Uzbekistan",  "Asia",    41.30,   69.28);
    carmen_world_add_city(w, "lahore",       "Lahore",        "لاہور",             "Pakistan",    "Asia",    31.55,   74.35);
    carmen_world_add_city(w, "dhaka",        "Dhaka",         "ঢাকা",              "Bangladesh",  "Asia",    23.81,   90.41);
    carmen_world_add_city(w, "kuala_lumpur", "Kuala Lumpur",  "كوالالمبور",         "Malaysia",    "Asia",     3.14,  101.69);
    carmen_world_add_city(w, "jakarta",      "Jakarta",       "Jakarta",           "Indonesia",   "Asia",    -6.21,  106.85);
    carmen_world_add_city(w, "bandung",      "Bandung",       "Bandung",           "Indonesia",   "Asia",    -6.91,  107.61);

    /* -------------------------------------------------------- sites & clues */

    /* Istanbul  (connections: sarajevo, cairo, konya, beirut)
       Suleymaniye→sarajevo+cairo  Istanbul Modern→cairo+konya
       Kadikoy→konya+beirut        Camlica→beirut+sarajevo       */
    { ClueData c[] = {{"The suspect was googling Bosnian coffee recipes",                                          "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"{They} was comparing Ottoman tile patterns to Mamluk ones along the Nile",                    "cairo",    CARMEN_CLUE_POSITIVE},
                      {"The muezzin shook his head -- no strangers at today's prayer", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Suleymaniye Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"{They} kept staring at a Nile River postcard pinned to the gallery wall",                      "cairo",    CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned the Whirling Dervishes' hometown as {their} next cultural stop",                 "konya",    CARMEN_CLUE_POSITIVE},
                      {"The gallery staff haven't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Istanbul Modern",     "museum",   c, 3); }
    { ClueData c[] = {{"The suspect ordered a ferry ticket and asked about the Whirling Dervishes",                "konya",    CARMEN_CLUE_POSITIVE},
                      {"{They} was raving about the food scene in a city where church bells mix with the adhan",       "beirut",   CARMEN_CLUE_POSITIVE},
                      {"The fish vendor hasn't seen anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Kadikoy Food Market", "market",   c, 3); }
    { ClueData c[] = {{"{They} mentioned a city where you can ski in the morning and swim the Mediterranean by noon", "beirut",   CARMEN_CLUE_POSITIVE},
                      {"{They} was reading about Ottoman bridges in a Balkan city known for its coppersmiths",         "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"The imam has not seen any unfamiliar faces at today's prayers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Camlica Mosque",      "mosque",   c, 3); }

    /* Dubai  (connections: doha, muscat, abu_dhabi, isfahan, lahore, amman)
       Museum→doha+muscat      Al Fahidi→muscat+lahore
       Dubai Mall→lahore+isfahan  Jumeirah→isfahan+doha          */
    { ClueData c[] = {{"{They} said the world's best collection of Islamic calligraphy is just a short flight south",  "doha",    CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned pearl diving traditions across the Gulf to the southeast",                    "muscat",  CARMEN_CLUE_POSITIVE},
                      {"The receptionist hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Museum of the Future",      "museum",   c, 3); }
    { ClueData c[] = {{"{They} mentioned old dhow routes along the Omani coast",                                       "muscat",  CARMEN_CLUE_POSITIVE},
                      {"{They} had a PIA boarding pass tucked in {their} passport",                                        "lahore",  CARMEN_CLUE_POSITIVE},
                      {"The guide said no foreigners have visited this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Al Fahidi Historical Dist", "landmark", c, 3); }
    { ClueData c[] = {{"{They} was dreaming aloud about Mughal gardens and Wazir Khan's tilework",                     "lahore",  CARMEN_CLUE_POSITIVE},
                      {"{They} asked the imam about Persian tile patterns and dome geometry",                          "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The concierge hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Dubai Mall",                "market",   c, 3); }
    { ClueData c[] = {{"{They} was studying Farsi on {their} phone and comparing dome geometry",                           "isfahan", CARMEN_CLUE_POSITIVE},
                      {"{They} was checking flight times to a nearby Gulf peninsula capital",                          "doha",    CARMEN_CLUE_POSITIVE},
                      {"The mosque attendant has not noticed any unfamiliar visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Jumeirah Mosque",           "mosque",   c, 3); }

    /* Kuala Lumpur  (connections: bandung, jakarta, dhaka, hyderabad)
       Petronas→bandung+hyderabad  Islamic Arts→jakarta+dhaka
       Jalan Alor→dhaka+hyderabad  Masjid Negara→jakarta+bandung  */
    { ClueData c[] = {{"{They} was raving about volcanic highland coffee from Java's most artistic city",             "bandung",      CARMEN_CLUE_POSITIVE},
                      {"{They} was researching Charminar and biryani in India's Nizam city",                           "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"The observation deck staff haven't noticed anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Petronas Towers",       "landmark", c, 3); }
    { ClueData c[] = {{"{They} wanted to see the largest mosque in the world's most populous Muslim-majority country", "jakarta",     CARMEN_CLUE_POSITIVE},
                      {"The suspect was craving kacchi biryani from Bengal",                                        "dhaka",       CARMEN_CLUE_POSITIVE},
                      {"The curator said no one has asked about travel lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Islamic Arts Museum",   "museum",   c, 3); }
    { ClueData c[] = {{"{They} mentioned flying to Bangladesh for a friend's wedding",                                 "dhaka",       CARMEN_CLUE_POSITIVE},
                      {"{They} said the Laad Bazaar bangles in India's pearl city are even prettier",                  "hyderabad",   CARMEN_CLUE_POSITIVE},
                      {"The hawker stall owner shrugged -- too busy to notice anyone", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Jalan Alor",            "market",   c, 3); }
    { ClueData c[] = {{"{They} said the mihrab here can't match the one inside Istiqlal across the strait",            "jakarta",     CARMEN_CLUE_POSITIVE},
                      {"{They} compared the batik here to the highland city in Java where it's made by hand",          "bandung",     CARMEN_CLUE_POSITIVE},
                      {"The worshippers here don't recall seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Masjid Negara",         "mosque",   c, 3); }

    /* Doha  (connections: dubai, abu_dhabi)
       Museum→abu_dhabi+dubai   Souq Waqif→dubai+abu_dhabi
       Katara→abu_dhabi+dubai   Education→dubai+abu_dhabi      */
    { ClueData c[] = {{"{They} mentioned wanting to see the white marble mosque that seats 40,000 worshippers",        "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"{They} said the glittering skyline just an hour's drive away puts this one to shame",          "dubai",     CARMEN_CLUE_POSITIVE},
                      {"The security guard says no tourists have been asking unusual questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Museum of Islamic Art",     "museum",   c, 3); }
    { ClueData c[] = {{"{They} was comparing spice prices to the souks in the city with the world's tallest tower",    "dubai",     CARMEN_CLUE_POSITIVE},
                      {"{They} kept talking about the Louvre branch just across the Gulf",                             "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The spice merchant has not seen anyone suspicious this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Souq Waqif",               "market",   c, 3); }
    { ClueData c[] = {{"{They} had a campus visitor badge from a university in the Emirates' capital",                "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"{They} was checking Emirates flights to the mega-mall across the Gulf",                         "dubai",     CARMEN_CLUE_POSITIVE},
                      {"The cultural center staff have not seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Katara Cultural Village",  "landmark", c, 3); }
    { ClueData c[] = {{"{They} said the Museum of the Future in the Gulf's glitziest city is {their} next stop",           "dubai",     CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned studying at a Gulf university where East meets West academically",            "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"Nobody at the campus remembers seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Education City",            "landmark", c, 3); }

    /* Amman  (connections: cairo, beirut, dubai)
       Citadel→cairo+beirut  Rainbow→beirut+dubai  King Abdullah→dubai+cairo */
    { ClueData c[] = {{"The suspect was reading about Pharaonic architecture",                                     "cairo",  CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned a city where you can party till dawn and hear church bells with the adhan",    "beirut", CARMEN_CLUE_POSITIVE},
                      {"The site guards have not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "The Citadel",              "landmark", c, 3); }
    { ClueData c[] = {{"{They} was converting dinars to Lebanese pounds and checking nightlife reviews",               "beirut", CARMEN_CLUE_POSITIVE},
                      {"{They} asked about the city where they built the world's tallest tower in the desert",          "dubai",  CARMEN_CLUE_POSITIVE},
                      {"The barista doesn't recall anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "Rainbow Street Cafes",     "cafe",     c, 3); }
    { ClueData c[] = {{"{They} was checking Emirates airline schedules to a Gulf megacity",                            "dubai",  CARMEN_CLUE_POSITIVE},
                      {"{They} was exchanging dinars for Egyptian pounds at the currency desk",                         "cairo",  CARMEN_CLUE_POSITIVE},
                      {"The imam said no visitors have asked about travel recently", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "King Abdullah Mosque",     "mosque",   c, 3); }

    /* Casablanca  (connections: marrakech, fez, dakar, cairo)
       Hassan II→marrakech+dakar  Habous→dakar+cairo
       Morocco Mall→cairo+fez     Art Deco→fez+marrakech         */
    { ClueData c[] = {{"The suspect mentioned the Atlas Mountains are just a train ride away",                     "marrakech", CARMEN_CLUE_POSITIVE},
                      {"{They} was studying a map of West Africa with a city on the Atlantic coast circled",            "dakar",     CARMEN_CLUE_POSITIVE},
                      {"The mosque caretaker has not seen any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Hassan II Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"{They} was asking about flights to Senegal at a travel agency",                               "dakar",     CARMEN_CLUE_POSITIVE},
                      {"{They} had a guidebook for the Egyptian pyramids in {their} shopping bag",                          "cairo",     CARMEN_CLUE_POSITIVE},
                      {"The shopkeeper hasn't seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Habous Quarter",    "market",   c, 3); }
    { ClueData c[] = {{"{They} was exchanging dirhams for Egyptian currency at the bureau de change",                  "cairo",     CARMEN_CLUE_POSITIVE},
                      {"{They} said the city with the world's oldest university has finer examples of this style",      "fez",       CARMEN_CLUE_POSITIVE},
                      {"The mall security hasn't flagged anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Morocco Mall",      "market",   c, 3); }
    { ClueData c[] = {{"{They} said the medina in Morocco's oldest imperial city makes this look modern",               "fez",       CARMEN_CLUE_POSITIVE},
                      {"{They} had a riad reservation in the Red City",                                                "marrakech", CARMEN_CLUE_POSITIVE},
                      {"No one in the cafe recalls a traveler asking for directions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Art Deco Downtown", "landmark", c, 3); }

    /* Jakarta  (connections: kuala_lumpur, bandung)
       Istiqlal→kuala_lumpur+bandung  National Museum→bandung+kuala_lumpur
       Kota Tua→kuala_lumpur+bandung  Menteng→bandung+kuala_lumpur  */
    { ClueData c[] = {{"{They} said even the world's largest mosque can't match the Islamic arts collection across the strait", "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"{They} was checking train schedules to a volcanic highland city in West Java",                          "bandung",      CARMEN_CLUE_POSITIVE},
                      {"The security guard has not noticed anyone suspicious", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Istiqlal Mosque",    "mosque",   c, 3); }
    { ClueData c[] = {{"{They} was sketching Art Deco facades and said a Javanese highland city has even better ones", "bandung",      CARMEN_CLUE_POSITIVE},
                      {"{They} booked a flight north to Malaysia",                                                     "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The museum attendant doesn't recall any visitors asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "National Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"{They} ordered es kopi susu and researched street food near the twin towers up north",         "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"{They} said the volcanic highland city three hours by train is worth the trip",                 "bandung",      CARMEN_CLUE_POSITIVE},
                      {"The old Dutch building's caretaker saw nothing out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Kota Tua",           "landmark", c, 3); }
    { ClueData c[] = {{"{They} mentioned visiting the Paris of Java for its colonial architecture and hot springs",   "bandung",      CARMEN_CLUE_POSITIVE},
                      {"{They} said the nasi lemak across the strait in Malaysia is better than anything here",          "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The cafe owner hasn't seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Menteng Cafes",      "cafe",     c, 3); }

    /* Sarajevo  (connections: istanbul, fez)
       Bascarsija→istanbul+fez  Gazi Husrev-beg→fez+istanbul
       War Tunnel→istanbul+fez                                 */
    { ClueData c[] = {{"{They} was reading about the city where Sinan built his masterpiece mosques",                  "istanbul", CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned wanting to study at the world's oldest university in Morocco",                "fez",      CARMEN_CLUE_POSITIVE},
                      {"The coppersmiths haven't noticed any outsiders this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Bascarsija",             "market",   c, 3); }
    { ClueData c[] = {{"{They} asked about flights to Morocco's oldest imperial city",                                 "fez",      CARMEN_CLUE_POSITIVE},
                      {"{They} was pricing flights to the city that straddles two continents",                         "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The imam says no visitors have asked about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Gazi Husrev-beg Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect muttered something about the Bosphorus at sunset",                             "istanbul", CARMEN_CLUE_POSITIVE},
                      {"{They} had a printout of cheap flights to a Moroccan city with the oldest medina",             "fez",      CARMEN_CLUE_POSITIVE},
                      {"The museum staff don't remember any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "War Tunnel Museum",      "museum",   c, 3); }

    /* Tashkent  (connections: isfahan, lahore)
       Khast Imam→isfahan+lahore  Chorsu→lahore+isfahan
       Metro→lahore+isfahan       Minor→isfahan+lahore          */
    { ClueData c[] = {{"{They} was comparing the Silk Road tilework here to the ones in the city that's half the world", "isfahan", CARMEN_CLUE_POSITIVE},
                      {"{They} said the food scene in Pakistan's cultural capital would blow this bazaar away",          "lahore",  CARMEN_CLUE_POSITIVE},
                      {"The librarian guarding the Quran manuscript hasn't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Khast Imam Complex", "mosque",   c, 3); }
    { ClueData c[] = {{"{They} was buying dried fruits to bring to friends near the Badshahi Mosque",                  "lahore",  CARMEN_CLUE_POSITIVE},
                      {"{They} asked about direct flights to Iran",                                                    "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The bazaar vendors have not seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Chorsu Bazaar",      "market",   c, 3); }
    { ClueData c[] = {{"{They} photographed Soviet mosaics and said a Mughal city's tilework puts them to shame",     "lahore",  CARMEN_CLUE_POSITIVE},
                      {"{They} said the tilework here is beautiful but a certain Persian city's is unmatchable",         "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The metro staff haven't noticed anyone unusual riding the trains", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Tashkent Metro",     "landmark", c, 3); }
    { ClueData c[] = {{"A hotel booking in the city with Naqsh-e Jahan Square was on {their} phone",                   "isfahan", CARMEN_CLUE_POSITIVE},
                      {"{They} was telling a local that the Mughal fort in Pakistan's second city makes this look small", "lahore", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendant hasn't seen any foreigners this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Minor Mosque",       "mosque",   c, 3); }

    /* Cairo  (connections: amman, casablanca, abu_dhabi, zanzibar, dakar)
       Al-Azhar→amman+casablanca   Tahrir→casablanca+abu_dhabi
       Khan el-Khalili→abu_dhabi+amman  Sultan Hassan→amman+casablanca */
    { ClueData c[] = {{"{They} said this park's view rivals the ancient Citadel perched over a Levantine capital",     "amman",      CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned connecting through the Moroccan city that shares its name with a famous film", "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The park gardener hasn't noticed anyone out of place", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Al-Azhar Park",          "park",     c, 3); }
    { ClueData c[] = {{"{They} was checking Royal Air Maroc flights on {their} laptop",                                    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"{They} was comparing the massive scale here to a grand mosque in the Emirates' capital",         "abu_dhabi",  CARMEN_CLUE_POSITIVE},
                      {"The street vendors haven't noticed anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Tahrir Square",           "landmark", c, 3); }
    { ClueData c[] = {{"{They} was sketching the muqarnas and comparing them to the white marble mosque that seats 40,000", "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"{They} was checking visa requirements for Jordan",                                                     "amman",     CARMEN_CLUE_POSITIVE},
                      {"The merchants say business has been slow -- no new faces", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Khan el-Khalili",         "market",   c, 3); }
    { ClueData c[] = {{"The suspect was exchanging Egyptian pounds for Jordanian dinars",                           "amman",      CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned heading west to Morocco before flying home",                                    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendants say no tourists have been around today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Sultan Hassan Mosque",    "mosque",   c, 3); }

    /* Beirut  (connections: amman, istanbul)
       Mohammad Al-Amin→amman+istanbul  Gemmayzeh→istanbul+amman
       National Museum→amman+istanbul                          */
    { ClueData c[] = {{"{They} was reading about Nabataean ruins and a Roman amphitheater in a Levantine capital",     "amman",    CARMEN_CLUE_POSITIVE},
                      {"{They} said the street art is good but the scene in Turkey's largest city is better",          "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The mosque guard hasn't noticed any outsiders this afternoon", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Mohammad Al-Amin Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"{They} mentioned heading to the city where Hagia Sophia stands",                               "istanbul", CARMEN_CLUE_POSITIVE},
                      {"{They} was converting Lebanese lira to Jordanian dinar at the exchange",                       "amman",    CARMEN_CLUE_POSITIVE},
                      {"The gallery owner shook his head -- no one asked about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Gemmayzeh Street Art",    "landmark", c, 3); }
    { ClueData c[] = {{"{They} was comparing Phoenician artifacts to ones from a city perched over a desert Citadel", "amman",    CARMEN_CLUE_POSITIVE},
                      {"{They} had a Turkish Airlines boarding pass stub in {their} jacket",                                "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The front desk has no record of anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "National Museum",         "museum",   c, 3); }

    /* Marrakech  (connections: casablanca, fez)
       Jemaa→fez+casablanca  MACAAL→casablanca+fez
       Le Jardin→fez+casablanca                                */
    { ClueData c[] = {{"The suspect asked a storyteller about the oldest university in the world",                 "fez",        CARMEN_CLUE_POSITIVE},
                      {"{They} was searching for flights to the Moroccan city named in a classic Bogart film",         "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The snake charmer says he sees thousands of faces -- none stood out", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Jemaa el-Fnaa",      "landmark", c, 3); }
    { ClueData c[] = {{"{They} said {they}'s catching a train to Morocco's economic capital tonight",                       "casablanca", CARMEN_CLUE_POSITIVE},
                      {"{They} said the city with the world's oldest university has finer art",                        "fez",        CARMEN_CLUE_POSITIVE},
                      {"The gallery guard hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "MACAAL",              "museum",   c, 3); }
    { ClueData c[] = {{"{They} had a train ticket to Morocco's oldest imperial city in {their} back pocket",               "fez",        CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned visiting the Art Deco district in Morocco's coastal business hub",            "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The riad keeper hasn't had any unusual guests this month", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Le Jardin Secret",   "park",     c, 3); }

    /* Lahore  (connections: dhaka, tashkent, dubai, hyderabad)
       Badshahi→dhaka+tashkent  Fort→tashkent+dubai
       Food Street→dubai+hyderabad  Anarkali→hyderabad+dhaka   */
    { ClueData c[] = {{"{They} was comparing this mosque to one in a Bengali river capital surrounded by waterways",   "dhaka",     CARMEN_CLUE_POSITIVE},
                      {"{They} was reading about Timurid architecture in a city famous for its Soviet-era metro",      "tashkent",  CARMEN_CLUE_POSITIVE},
                      {"The prayer hall attendant says no foreigners have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Badshahi Mosque",      "mosque",   c, 3); }
    { ClueData c[] = {{"{They} mentioned comparing these Mughal walls to Silk Road forts in Central Asia's largest city", "tashkent", CARMEN_CLUE_POSITIVE},
                      {"{They} was reading restaurant reviews from the Gulf city with the world's tallest building",      "dubai",    CARMEN_CLUE_POSITIVE},
                      {"The fort guard has not seen any suspicious visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Lahore Fort",          "landmark", c, 3); }
    { ClueData c[] = {{"{They} said the Pakistani restaurants in the Gulf's glitziest city can't compare to the real thing", "dubai",     CARMEN_CLUE_POSITIVE},
                      {"{They} was comparing the biryani here to the famous Hyderabadi dum style across the border",        "hyderabad", CARMEN_CLUE_POSITIVE},
                      {"The waiter doesn't recall anyone unusual this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Food Street",          "market",   c, 3); }
    { ClueData c[] = {{"{They} mentioned shipping a box of silk fabric to a friend near the Charminar",                "hyderabad", CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned visiting relatives in Bangladesh next",                                       "dhaka",     CARMEN_CLUE_POSITIVE},
                      {"The bazaar shopkeepers haven't seen anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Anarkali Bazaar",      "market",   c, 3); }

    /* Konya  (connections: istanbul, isfahan)
       Mevlana→isfahan+istanbul  Alaeddin→istanbul+isfahan
       Sille→istanbul+isfahan                                  */
    { ClueData c[] = {{"{They} was reading Rumi and said {they}'d follow the poet's path to Persia's city of bridges",    "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"{They} asked the guide about bus connections to Turkey's largest city",                        "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The museum docent hasn't seen any visitors asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Mevlana Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"{They} said the great Sinan mosque on the Golden Horn is more impressive but this one has soul", "istanbul", CARMEN_CLUE_POSITIVE},
                      {"{They} had an Iran visa application form half-filled",                                           "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"The worshippers don't remember seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Alaeddin Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect said {they}'d rather be where the call to prayer echoes across two continents",    "istanbul", CARMEN_CLUE_POSITIVE},
                      {"{They} was comparing the Sufi lodges here to the mystic gardens in a Persian oasis city",       "isfahan",  CARMEN_CLUE_POSITIVE},
                      {"The villagers say no tourists have come through lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Sille Village",     "landmark", c, 3); }

    /* Abu Dhabi  (connections: dubai, doha, muscat, cairo)
       Zayed→muscat+doha  Louvre→doha+cairo
       Mangrove→cairo+dubai  Qasr→dubai+muscat                */
    { ClueData c[] = {{"{They} said the Sultan's Grand Mosque on the Arabian Sea coast has comparable grandeur",       "muscat", CARMEN_CLUE_POSITIVE},
                      {"{They} said {they} prefers the I.M. Pei-designed museum on the peninsula for Islamic art",        "doha",   CARMEN_CLUE_POSITIVE},
                      {"The mosque attendant has not noticed anyone unfamiliar", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Sheikh Zayed Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"{They} said a nearby Gulf capital's collection of Islamic art is even deeper",                  "doha",   CARMEN_CLUE_POSITIVE},
                      {"{They} was reading about Saladin's Citadel on the Nile and comparing palaces",                 "cairo",  CARMEN_CLUE_POSITIVE},
                      {"The ticket desk has no record of anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Louvre Abu Dhabi",    "museum",   c, 3); }
    { ClueData c[] = {{"{They} was exchanging dirhams for Egyptian pounds at the park gift shop",                      "cairo",  CARMEN_CLUE_POSITIVE},
                      {"{They} said the Museum of the Future in the Gulf's glitziest city is next on {their} list",          "dubai",  CARMEN_CLUE_POSITIVE},
                      {"The park ranger says it's been quiet all day", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Mangrove Park",       "park",     c, 3); }
    { ClueData c[] = {{"{They} was checking Emirates flights to the mega-mall across the Gulf",                         "dubai",  CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned kayaking near a port city where frankincense ships once sailed",               "muscat", CARMEN_CLUE_POSITIVE},
                      {"The palace staff say no visitors have asked about flights today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Qasr Al Watan",       "landmark", c, 3); }

    /* Muscat  (connections: dubai, abu_dhabi, dhaka, zanzibar)
       Sultan Qaboos→zanzibar+dubai  Bimmah→dhaka+zanzibar
       Royal Opera→dubai+dhaka                                 */
    { ClueData c[] = {{"{They} was reading about the old Omani stone houses on a spice island off East Africa",        "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"{They} said a desert city's cultural scene rivals this one -- its opera season is next month",  "dubai",    CARMEN_CLUE_POSITIVE},
                      {"The mosque guide hasn't seen anyone asking for directions lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Sultan Qaboos Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"{They} mentioned visiting the world's most densely populated city where rickshaws outnumber cars", "dhaka",    CARMEN_CLUE_POSITIVE},
                      {"{They} had a documentary about Swahili trading ports bookmarked on {their} tablet",                      "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"The swimmers here haven't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Bimmah Sinkhole",        "landmark", c, 3); }
    { ClueData c[] = {{"{They} was checking Emirates airline schedules heading west across the Gulf",                   "dubai",    CARMEN_CLUE_POSITIVE},
                      {"{They} said {they}'s heading east to a river delta capital in South Asia",                          "dhaka",    CARMEN_CLUE_POSITIVE},
                      {"The box office has no record of tickets sold to anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Royal Opera House",      "landmark", c, 3); }

    /* Dakar  (connections: casablanca, cairo, zanzibar)
       Grande Mosquee→casablanca+cairo  Goree→cairo+zanzibar
       African Renaissance→zanzibar+casablanca                 */
    { ClueData c[] = {{"{They} said the Art Deco of a Moroccan coastal city reminded {them} of home",                    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"{They} was researching how the slave trade connected West Africa to the land of the Pharaohs",  "cairo",      CARMEN_CLUE_POSITIVE},
                      {"The mosque caretaker has not seen anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Grande Mosquee",              "mosque",   c, 3); }
    { ClueData c[] = {{"{They} said the history museums along the Nile cover similar stories of trade and movement",   "cairo",      CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned booking a dhow trip along the East African coast to a spice island",           "zanzibar",   CARMEN_CLUE_POSITIVE},
                      {"The ferry operators haven't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Goree Island",                "landmark", c, 3); }
    { ClueData c[] = {{"{They} was reading about old Omani stone houses in a Swahili port off the East African coast",  "zanzibar",   CARMEN_CLUE_POSITIVE},
                      {"{They} was booking a Royal Air Maroc flight heading north to Morocco",                          "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The monument guards say no tourists have asked about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "African Renaissance Monument", "landmark", c, 3); }

    /* Bandung  (connections: kuala_lumpur, jakarta)
       Braga→kuala_lumpur+jakarta  Masjid Raya→jakarta+kuala_lumpur
       Tangkuban→kuala_lumpur+jakarta                          */
    { ClueData c[] = {{"{They} was comparing Braga Street to the famous hawker lane near the twin towers",             "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"{They} said the world's largest mosque in Indonesia's capital makes this city look tiny",       "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"The barista hasn't served anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Braga Street",          "cafe",     c, 3); }
    { ClueData c[] = {{"{They} had a train ticket back to the Javanese megacity on the coast",                         "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"{They} said the coffee here is good but Malaysia's capital has better variety",                 "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"No one at the mosque recalls a visitor asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Masjid Raya Bandung",   "mosque",   c, 3); }
    { ClueData c[] = {{"{They} mentioned catching the evening train back to Indonesia's capital",                       "jakarta",      CARMEN_CLUE_POSITIVE},
                      {"{They} was comparing the tea plantations here to the Cameron Highlands near the twin towers",  "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The volcano guides haven't seen any tourists asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Tangkuban Perahu",      "landmark", c, 3); }

    /* Isfahan  (connections: dubai, konya, tashkent)
       Naqsh-e Jahan→dubai+konya  Vank→konya+tashkent
       Si-o-se-pol→tashkent+dubai                              */
    { ClueData c[] = {{"{They} said this square is half the world, but a glittering Gulf port has the other half",     "dubai",    CARMEN_CLUE_POSITIVE},
                      {"{They} visited the Armenian quarter and said {their} next stop is a Turkish city famous for Sufis", "konya",   CARMEN_CLUE_POSITIVE},
                      {"The carpet sellers say no one has asked about travel today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Naqsh-e Jahan Square",  "landmark", c, 3); }
    { ClueData c[] = {{"{They} said the Mevlana Museum in the city where Rumi rests is {their} next spiritual stop",       "konya",    CARMEN_CLUE_POSITIVE},
                      {"{They} was sipping tea and reading about Soviet metro mosaics in Central Asia's largest city",  "tashkent", CARMEN_CLUE_POSITIVE},
                      {"The priests at the cathedral have not seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Vank Cathedral",         "landmark", c, 3); }
    { ClueData c[] = {{"{They} said {they}'s following the Silk Road east to Central Asia's largest city",                  "tashkent", CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned the desert city skyline with the world's tallest tower as {their} next stop",      "dubai",    CARMEN_CLUE_POSITIVE},
                      {"The tea house regulars don't remember anyone new this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Si-o-se-pol Tea Houses", "cafe",     c, 3); }

    /* Dhaka  (connections: lahore, kuala_lumpur, muscat, hyderabad)
       Baitul Mukarram→kuala_lumpur+lahore  Star Mosque→lahore+muscat
       Lalbagh→muscat+hyderabad  Sadarghat→hyderabad+kuala_lumpur */
    { ClueData c[] = {{"{They} said the world's tallest twin towers are calling {their} name",                            "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"{They} photographed the star-shaped tiles and said a Pakistani city's are even finer",          "lahore",       CARMEN_CLUE_POSITIVE},
                      {"The mosque guard says no strangers have attended prayers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Baitul Mukarram Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"{They} said the Wazir Khan Mosque in the Mughal heartland has even more intricate tiles",     "lahore",       CARMEN_CLUE_POSITIVE},
                      {"{They} was haggling for a rickshaw and talking about frankincense merchants across the ocean",   "muscat",       CARMEN_CLUE_POSITIVE},
                      {"The caretaker says only locals have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Star Mosque",             "mosque",   c, 3); }
    { ClueData c[] = {{"{They} mentioned sailing to a port city on the Arabian Sea along an old trade route",           "muscat",       CARMEN_CLUE_POSITIVE},
                      {"{They} was buying bangles and said the Charminar city has them even cheaper",                   "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"The rickshaw drivers haven't taken any foreigners to the fort", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Lalbagh Fort",            "landmark", c, 3); }
    { ClueData c[] = {{"{They} was comparing the river chaos to nothing -- India's Nizam city is calmer",              "hyderabad",    CARMEN_CLUE_POSITIVE},
                      {"{They} was checking Malaysia Airlines prices on {their} phone",                                     "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The ferry ticket sellers haven't seen any foreigners today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Sadarghat Terminal",      "landmark", c, 3); }

    /* Fez  (connections: casablanca, marrakech, sarajevo)
       Al-Qarawiyyin→sarajevo+casablanca  Medina→marrakech+sarajevo
       Borj Nord→casablanca+marrakech                          */
    { ClueData c[] = {{"{They} bragged about the world's oldest university and said a Balkan city with minarets and church bells is next", "sarajevo",   CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned a friend waiting for {them} in Morocco's economic capital",                                          "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The university librarian hasn't seen any visitors matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Al-Qarawiyyin",       "mosque",   c, 3); }
    { ClueData c[] = {{"{They} said the Red City at the foot of the Atlas Mountains is easier to navigate",            "marrakech",  CARMEN_CLUE_POSITIVE},
                      {"{They} had a printout of cheap flights to Bosnia",                                             "sarajevo",   CARMEN_CLUE_POSITIVE},
                      {"The leather tannery workers don't recall any foreigners asking questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Fez Medina",          "market",   c, 3); }
    { ClueData c[] = {{"The suspect posted a panoramic photo with a hashtag about the Moroccan city from the classic Bogart film", "casablanca", CARMEN_CLUE_POSITIVE},
                      {"{They} said the city at the foot of the Atlas Mountains is more fun but this one has more soul",                 "marrakech",  CARMEN_CLUE_POSITIVE},
                      {"The ticket booth operator hasn't sold any tickets to outsiders", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Borj Nord Viewpoint", "landmark", c, 3); }

    /* Zanzibar  (connections: cairo, muscat, dakar)
       Stone Town→muscat+dakar  Hamamni→dakar+cairo
       Malindi→cairo+muscat  Forodhani→muscat+cairo            */
    { ClueData c[] = {{"{They} mentioned the Omani sultans who once ruled this island",                               "muscat",   CARMEN_CLUE_POSITIVE},
                      {"{They} had a printout of flights to a West African city where Sufi brotherhoods lead prayers",  "dakar",    CARMEN_CLUE_POSITIVE},
                      {"The shopkeeper in the narrow lanes hasn't seen anyone unusual", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Stone Town",            "landmark", c, 3); }
    { ClueData c[] = {{"{They} mentioned wanting to visit Senegal's Atlantic coast next",                             "dakar",    CARMEN_CLUE_POSITIVE},
                      {"{They} said the great Mamluk mosques by the pyramids put this island to shame",                 "cairo",    CARMEN_CLUE_POSITIVE},
                      {"The old bath-house guide has not noticed any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Hamamni Persian Baths", "landmark", c, 3); }
    { ClueData c[] = {{"{They} was reading about parks along the Nile on {their} phone",                                  "cairo",    CARMEN_CLUE_POSITIVE},
                      {"{They} was reading about Swahili coast trade routes to a frankincense port on the Arabian Sea",  "muscat",  CARMEN_CLUE_POSITIVE},
                      {"The imam has not seen any foreigners at prayers this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Malindi Mosque",        "mosque",   c, 3); }
    { ClueData c[] = {{"{They} was comparing the old Omani fort here to the Sultan's palace across the sea",           "muscat",   CARMEN_CLUE_POSITIVE},
                      {"{They} was eating Zanzibar pizza and said the street food by the pyramids is even better",      "cairo",   CARMEN_CLUE_POSITIVE},
                      {"The food vendors say no tourists have been asking questions tonight", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Forodhani Gardens",     "market",   c, 3); }

    /* Hyderabad  (connections: dubai, lahore, dhaka, kuala_lumpur)
       Charminar→lahore+dubai  Mecca Masjid→dubai+kuala_lumpur
       Salar Jung→kuala_lumpur+dhaka  Laad Bazaar→dhaka+lahore */
    { ClueData c[] = {{"{They} was comparing the four minarets to the great Badshahi Mosque in Pakistan's Mughal city", "lahore",       CARMEN_CLUE_POSITIVE},
                      {"{They} was checking Emirates flights to the Gulf on {their} phone",                                 "dubai",        CARMEN_CLUE_POSITIVE},
                      {"The monument guard hasn't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Charminar",         "landmark", c, 3); }
    { ClueData c[] = {{"{They} said this mosque is beautiful but the one in the Gulf city with the tallest tower is more famous", "dubai",        CARMEN_CLUE_POSITIVE},
                      {"{They} was researching street food near the twin towers across the Indian Ocean",                        "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendant has not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Mecca Masjid",      "mosque",   c, 3); }
    { ClueData c[] = {{"{They} said Malaysia's capital has even more impressive Islamic art collections",              "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned shipping a box of lac bangles to a friend in Bangladesh",                      "dhaka",        CARMEN_CLUE_POSITIVE},
                      {"The security staff have not logged any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Salar Jung Museum", "museum",   c, 3); }
    { ClueData c[] = {{"{They} was buying bangles and said the river capital of Bengal has them even cheaper",         "dhaka",        CARMEN_CLUE_POSITIVE},
                      {"{They} mentioned flying to Pakistan next to see the Badshahi Mosque",                           "lahore",       CARMEN_CLUE_POSITIVE},
                      {"The bazaar merchants haven't noticed anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Laad Bazaar",       "market",   c, 3); }

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
