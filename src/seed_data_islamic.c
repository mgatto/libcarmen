#include "carmen/seed_data_islamic.h"
#include "seed_helpers.h"

/* ---- world builder: Modern Muslim Traveler ---- */

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
    /* Clue tiers: first positive = Cat 2 (cultural), second = Cat 1 (geographic) */

    /* Istanbul */
    { ClueData c[] = {{"The suspect was googling Bosnian coffee recipes",                                          "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"She mentioned wanting to visit the place where East meets West... in the Balkans",          "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"The muezzin shook his head -- no strangers at today's prayer", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Suleymaniye Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"He kept staring at a Nile River postcard pinned to the gallery wall",                      "cairo", CARMEN_CLUE_POSITIVE},
                      {"She was exchanging lira for Egyptian pounds at the bureau de change",                       "cairo", CARMEN_CLUE_POSITIVE},
                      {"The gallery staff haven't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Istanbul Modern",     "museum",   c, 3); }
    { ClueData c[] = {{"The suspect ordered a ferry ticket and asked about the Whirling Dervishes",                "konya", CARMEN_CLUE_POSITIVE},
                      {"She had a bus timetable for an Anatolian city famous for its Sufi poets",                   "konya", CARMEN_CLUE_POSITIVE},
                      {"The fish vendor hasn't seen anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Kadikoy Food Market", "market",   c, 3); }
    { ClueData c[] = {{"She mentioned a city where you can ski in the morning and swim the Mediterranean by noon", "beirut", CARMEN_CLUE_POSITIVE},
                      {"He was converting lira to Lebanese pounds at the exchange counter",                         "beirut", CARMEN_CLUE_POSITIVE},
                      {"The imam has not seen any unfamiliar faces at today's prayers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Camlica Mosque",      "mosque",   c, 3); }

    /* Dubai */
    { ClueData c[] = {{"He said the world's best collection of Islamic calligraphy is just a short flight south",  "doha", CARMEN_CLUE_POSITIVE},
                      {"She was checking flight times to a nearby Gulf peninsula capital",                          "doha", CARMEN_CLUE_POSITIVE},
                      {"The receptionist hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Museum of the Future",      "museum",   c, 3); }
    { ClueData c[] = {{"He mentioned pearl diving traditions across the Gulf",                                     "muscat", CARMEN_CLUE_POSITIVE},
                      {"She was reading about Oman's coast and old dhow routes",                                   "muscat", CARMEN_CLUE_POSITIVE},
                      {"The guide said no foreigners have visited this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Al Fahidi Historical Dist", "landmark", c, 3); }
    { ClueData c[] = {{"He was dreaming aloud about Mughal gardens and Wazir Khan's tilework",                     "lahore", CARMEN_CLUE_POSITIVE},
                      {"She had a PIA boarding pass tucked in her passport",                                        "lahore", CARMEN_CLUE_POSITIVE},
                      {"The concierge hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Dubai Mall",                "market",   c, 3); }
    { ClueData c[] = {{"She asked the imam about Persian tile patterns",                                           "isfahan", CARMEN_CLUE_POSITIVE},
                      {"He was studying Farsi on his phone and comparing dome geometry",                            "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The mosque attendant has not noticed any unfamiliar visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Jumeirah Mosque",           "mosque",   c, 3); }

    /* Kuala Lumpur */
    { ClueData c[] = {{"She was raving about volcanic highland coffee from Java's most artistic city",             "bandung", CARMEN_CLUE_POSITIVE},
                      {"He asked which Indonesian city has the best cafe scene",                                    "bandung", CARMEN_CLUE_POSITIVE},
                      {"The observation deck staff haven't noticed anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Petronas Towers",       "landmark", c, 3); }
    { ClueData c[] = {{"She wanted to see the largest mosque in the world's most populous Muslim-majority country", "jakarta", CARMEN_CLUE_POSITIVE},
                      {"She was planning a trip to Indonesia's capital next",                                        "jakarta", CARMEN_CLUE_POSITIVE},
                      {"The curator said no one has asked about travel lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Islamic Arts Museum",   "museum",   c, 3); }
    { ClueData c[] = {{"The suspect was craving kacchi biryani from Bengal",                                       "dhaka", CARMEN_CLUE_POSITIVE},
                      {"He mentioned flying to Bangladesh for a friend's wedding",                                  "dhaka", CARMEN_CLUE_POSITIVE},
                      {"The hawker stall owner shrugged -- too busy to notice anyone", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Jalan Alor",            "market",   c, 3); }
    { ClueData c[] = {{"He said the mihrab here can't match the one inside Istiqlal across the strait",            "jakarta", CARMEN_CLUE_POSITIVE},
                      {"She compared the architecture to the largest mosque in Southeast Asia",                      "jakarta", CARMEN_CLUE_POSITIVE},
                      {"The worshippers here don't recall seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Masjid Negara",         "mosque",   c, 3); }

    /* Doha */
    { ClueData c[] = {{"He mentioned wanting to see the white marble mosque that seats 40,000 worshippers",        "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The suspect kept comparing this collection to a Louvre branch just across the Gulf",         "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The security guard says no tourists have been asking unusual questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Museum of Islamic Art",     "museum",   c, 3); }
    { ClueData c[] = {{"She was haggling for saffron and muttering about bazaars in the city that's half the world", "isfahan", CARMEN_CLUE_POSITIVE},
                      {"He said the best saffron comes from Iran and he's heading there next",                       "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The spice merchant has not seen anyone suspicious this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Souq Waqif",               "market",   c, 3); }
    { ClueData c[] = {{"He had a documentary about Swahili trading ports bookmarked on his tablet",                "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"She mentioned booking a dhow trip along the East African coast",                            "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"The cultural center staff have not seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Katara Cultural Village",  "landmark", c, 3); }
    { ClueData c[] = {{"She had a campus visitor badge from a university in the Emirates' capital",                "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The suspect mentioned studying at a Gulf university where East meets West academically",    "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"Nobody at the campus remembers seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Education City",            "landmark", c, 3); }

    /* Amman */
    { ClueData c[] = {{"The suspect was reading about Pharaonic architecture",                                     "cairo", CARMEN_CLUE_POSITIVE},
                      {"She was exchanging dinars for Egyptian pounds at the currency desk",                         "cairo", CARMEN_CLUE_POSITIVE},
                      {"The site guards have not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "The Citadel",              "landmark", c, 3); }
    { ClueData c[] = {{"He mentioned a city where you can party till dawn and hear church bells with the adhan",   "beirut", CARMEN_CLUE_POSITIVE},
                      {"She was converting dinars to Lebanese pounds and checking nightlife reviews",                "beirut", CARMEN_CLUE_POSITIVE},
                      {"The barista doesn't recall anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "Rainbow Street Cafes",     "cafe",     c, 3); }
    { ClueData c[] = {{"The suspect asked about a city where they built the world's tallest tower in the desert",  "dubai", CARMEN_CLUE_POSITIVE},
                      {"She was checking Emirates airline schedules to a Gulf megacity",                             "dubai", CARMEN_CLUE_POSITIVE},
                      {"The imam said no visitors have asked about travel recently", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "King Abdullah Mosque",     "mosque",   c, 3); }

    /* Casablanca */
    { ClueData c[] = {{"The suspect mentioned the Atlas Mountains are just a train ride away",                     "marrakech", CARMEN_CLUE_POSITIVE},
                      {"She had a riad reservation in the Red City",                                                "marrakech", CARMEN_CLUE_POSITIVE},
                      {"The mosque caretaker has not seen any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Hassan II Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"He was studying a map of West Africa with a city on the Atlantic coast circled",           "dakar", CARMEN_CLUE_POSITIVE},
                      {"She was asking about flights to Senegal at a travel agency",                                "dakar", CARMEN_CLUE_POSITIVE},
                      {"The shopkeeper hasn't seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Habous Quarter",    "market",   c, 3); }
    { ClueData c[] = {{"He had a guidebook for the Egyptian pyramids in his shopping bag",                         "cairo", CARMEN_CLUE_POSITIVE},
                      {"She was exchanging dirhams for Egyptian currency at the bureau de change",                   "cairo", CARMEN_CLUE_POSITIVE},
                      {"The mall security hasn't flagged anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Morocco Mall",      "market",   c, 3); }
    { ClueData c[] = {{"She said the city with the world's oldest university has finer examples of this style",    "fez", CARMEN_CLUE_POSITIVE},
                      {"He said the medina in Morocco's oldest imperial city makes this look modern",               "fez", CARMEN_CLUE_POSITIVE},
                      {"No one in the cafe recalls a traveler asking for directions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Art Deco Downtown", "landmark", c, 3); }

    /* Jakarta */
    { ClueData c[] = {{"He said even the world's largest mosque can't match the Islamic arts collection across the strait", "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"She booked a flight north to Malaysia",                                                              "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The security guard has not noticed anyone suspicious", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Istiqlal Mosque",    "mosque",   c, 3); }
    { ClueData c[] = {{"He was sketching Art Deco facades and said a Javanese highland city has even better ones", "bandung", CARMEN_CLUE_POSITIVE},
                      {"She said the volcanic highland city three hours by train is worth the trip",                 "bandung", CARMEN_CLUE_POSITIVE},
                      {"The museum attendant doesn't recall any visitors asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "National Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"She mentioned visiting the Paris of Java for its colonial architecture and hot springs",   "bandung", CARMEN_CLUE_POSITIVE},
                      {"He was checking train schedules to a volcanic highland city in West Java",                   "bandung", CARMEN_CLUE_POSITIVE},
                      {"The old Dutch building's caretaker saw nothing out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Kota Tua",           "landmark", c, 3); }
    { ClueData c[] = {{"He ordered es kopi susu and researched street food near the twin towers up north",         "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"She said the nasi lemak across the strait in Malaysia is better than anything here",         "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The cafe owner hasn't seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Menteng Cafes",      "cafe",     c, 3); }

    /* Sarajevo */
    { ClueData c[] = {{"He was reading about the city where Sinan built his masterpiece mosques",                  "istanbul", CARMEN_CLUE_POSITIVE},
                      {"She had a Turkish Airlines boarding pass stub in her coat pocket",                           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The coppersmiths haven't noticed any outsiders this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Bascarsija",             "market",   c, 3); }
    { ClueData c[] = {{"She mentioned wanting to study at the world's oldest university",                          "fez", CARMEN_CLUE_POSITIVE},
                      {"He asked about flights to Morocco's oldest imperial city",                                  "fez", CARMEN_CLUE_POSITIVE},
                      {"The imam says no visitors have asked about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Gazi Husrev-beg Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect muttered something about the Bosphorus at sunset",                             "istanbul", CARMEN_CLUE_POSITIVE},
                      {"She was pricing flights to the city that straddles two continents",                          "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The museum staff don't remember any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "War Tunnel Museum",      "museum",   c, 3); }

    /* Tashkent */
    { ClueData c[] = {{"He was comparing the Silk Road tilework here to the ones in the city that's half the world", "isfahan", CARMEN_CLUE_POSITIVE},
                      {"She asked about direct flights to Iran",                                                      "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The librarian guarding the Quran manuscript hasn't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Khast Imam Complex", "mosque",   c, 3); }
    { ClueData c[] = {{"She said the food scene in Pakistan's cultural capital would blow this bazaar away",        "lahore", CARMEN_CLUE_POSITIVE},
                      {"He was buying dried fruits to bring to friends near the Badshahi Mosque",                    "lahore", CARMEN_CLUE_POSITIVE},
                      {"The bazaar vendors have not seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Chorsu Bazaar",      "market",   c, 3); }
    { ClueData c[] = {{"She photographed Soviet mosaics and said a Mughal city's tilework puts them to shame",     "lahore", CARMEN_CLUE_POSITIVE},
                      {"He was telling a local that the Mughal fort in Pakistan's second city makes this look small", "lahore", CARMEN_CLUE_POSITIVE},
                      {"The metro staff haven't noticed anyone unusual riding the trains", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Tashkent Metro",     "landmark", c, 3); }
    { ClueData c[] = {{"A hotel booking in the city with Naqsh-e Jahan Square was on his phone",                   "isfahan", CARMEN_CLUE_POSITIVE},
                      {"She said the tilework here is beautiful but a certain Persian city's is unmatchable",        "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendant hasn't seen any foreigners this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Minor Mosque",       "mosque",   c, 3); }

    /* Cairo */
    { ClueData c[] = {{"He said this park's view rivals the ancient Citadel perched over a Levantine capital",     "amman", CARMEN_CLUE_POSITIVE},
                      {"She was checking visa requirements for Jordan",                                              "amman", CARMEN_CLUE_POSITIVE},
                      {"The park gardener hasn't noticed anyone out of place", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Al-Azhar Park",          "park",     c, 3); }
    { ClueData c[] = {{"He mentioned connecting through the Moroccan city that shares its name with a famous film", "casablanca", CARMEN_CLUE_POSITIVE},
                      {"She was checking Royal Air Maroc flights on her laptop",                                     "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The street vendors haven't noticed anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Tahrir Square",           "landmark", c, 3); }
    { ClueData c[] = {{"The suspect bought a Moroccan tea set as a gift",                                          "casablanca", CARMEN_CLUE_POSITIVE},
                      {"He mentioned heading west to Morocco before flying home",                                    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The merchants say business has been slow -- no new faces", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Khan el-Khalili",         "market",   c, 3); }
    { ClueData c[] = {{"She was sketching the muqarnas and comparing them to the white marble mosque that seats 40,000", "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"He was comparing the massive scale here to a grand mosque in the Emirates' capital",               "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendants say no tourists have been around today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Sultan Hassan Mosque",    "mosque",   c, 3); }

    /* Beirut */
    { ClueData c[] = {{"He was reading about Nabataean ruins and a Roman amphitheater in a Levantine capital",     "amman", CARMEN_CLUE_POSITIVE},
                      {"She was converting Lebanese lira to Jordanian dinar at the exchange",                        "amman", CARMEN_CLUE_POSITIVE},
                      {"The mosque guard hasn't noticed any outsiders this afternoon", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Mohammad Al-Amin Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"He mentioned heading to the city where Hagia Sophia stands",                               "istanbul", CARMEN_CLUE_POSITIVE},
                      {"She said the street art is good but the scene in Turkey's largest city is better",           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The gallery owner shook his head -- no one asked about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Gemmayzeh Street Art",    "landmark", c, 3); }
    { ClueData c[] = {{"She was comparing Phoenician artifacts to ones from the land along the Nile",              "cairo", CARMEN_CLUE_POSITIVE},
                      {"A brochure for the Egyptian Museum was tucked in his jacket",                                "cairo", CARMEN_CLUE_POSITIVE},
                      {"The front desk has no record of anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "National Museum",         "museum",   c, 3); }

    /* Marrakech */
    { ClueData c[] = {{"The suspect asked a storyteller about the oldest university in the world",                 "fez", CARMEN_CLUE_POSITIVE},
                      {"She had a train ticket to Morocco's oldest imperial city in her back pocket",               "fez", CARMEN_CLUE_POSITIVE},
                      {"The snake charmer says he sees thousands of faces -- none stood out", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Jemaa el-Fnaa",      "landmark", c, 3); }
    { ClueData c[] = {{"He was comparing this gallery to the vibrant art scene in West Africa's Atlantic port",    "dakar", CARMEN_CLUE_POSITIVE},
                      {"She mentioned the African contemporary art scene in Senegal",                               "dakar", CARMEN_CLUE_POSITIVE},
                      {"The gallery guard hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "MACAAL",              "museum",   c, 3); }
    { ClueData c[] = {{"She was searching for flights to the Moroccan city named in a classic Bogart film",        "casablanca", CARMEN_CLUE_POSITIVE},
                      {"He said he's catching a train to Morocco's economic capital tonight",                        "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The riad keeper hasn't had any unusual guests this month", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Le Jardin Secret",   "park",     c, 3); }

    /* Lahore */
    { ClueData c[] = {{"He was comparing this mosque to one in a Bengali river capital surrounded by waterways",   "dhaka", CARMEN_CLUE_POSITIVE},
                      {"She mentioned visiting relatives in Bangladesh next",                                        "dhaka", CARMEN_CLUE_POSITIVE},
                      {"The prayer hall attendant says no foreigners have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Badshahi Mosque",      "mosque",   c, 3); }
    { ClueData c[] = {{"She was reading about Timurid architecture in a city famous for its Soviet-era metro",     "tashkent", CARMEN_CLUE_POSITIVE},
                      {"He mentioned comparing these Mughal walls to Silk Road forts in Central Asia's largest city", "tashkent", CARMEN_CLUE_POSITIVE},
                      {"The fort guard has not seen any suspicious visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Lahore Fort",          "landmark", c, 3); }
    { ClueData c[] = {{"He was reading restaurant reviews from the Gulf city with the world's tallest building",   "dubai", CARMEN_CLUE_POSITIVE},
                      {"She said the Pakistani restaurants in the Gulf's glitziest city can't compare to the real thing", "dubai", CARMEN_CLUE_POSITIVE},
                      {"The waiter doesn't recall anyone unusual this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Food Street",          "market",   c, 3); }
    { ClueData c[] = {{"He bought silk fabric and said bazaars along the old Silk Road have better ikat",          "tashkent", CARMEN_CLUE_POSITIVE},
                      {"She was asking a silk merchant about supply routes through Uzbekistan",                      "tashkent", CARMEN_CLUE_POSITIVE},
                      {"The bazaar shopkeepers haven't seen anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Anarkali Bazaar",      "market",   c, 3); }

    /* Konya */
    { ClueData c[] = {{"He was reading Rumi and said he'd follow the poet's path to Persia's city of bridges",    "isfahan", CARMEN_CLUE_POSITIVE},
                      {"She had an Iran visa application form half-filled",                                          "isfahan", CARMEN_CLUE_POSITIVE},
                      {"The museum docent hasn't seen any visitors asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Mevlana Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"He said the great Sinan mosque on the Golden Horn is more impressive but this one has soul", "istanbul", CARMEN_CLUE_POSITIVE},
                      {"She asked the guide about bus connections to Turkey's largest city",                           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The worshippers don't remember seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Alaeddin Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect said he'd rather be where the call to prayer echoes across two continents",    "istanbul", CARMEN_CLUE_POSITIVE},
                      {"She asked a villager about the fastest bus to the city on the Bosphorus",                    "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The villagers say no tourists have come through lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Sille Village",     "landmark", c, 3); }

    /* Abu Dhabi */
    { ClueData c[] = {{"He said the Sultan's Grand Mosque on the Arabian Sea coast has comparable grandeur",       "muscat", CARMEN_CLUE_POSITIVE},
                      {"She was checking Oman Air flights on her phone",                                             "muscat", CARMEN_CLUE_POSITIVE},
                      {"The mosque attendant has not noticed anyone unfamiliar", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Sheikh Zayed Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"She said she prefers the I.M. Pei-designed museum on the peninsula for Islamic art",       "doha", CARMEN_CLUE_POSITIVE},
                      {"He said a nearby Gulf capital's collection of Islamic art is even deeper",                   "doha", CARMEN_CLUE_POSITIVE},
                      {"The ticket desk has no record of anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Louvre Abu Dhabi",    "museum",   c, 3); }
    { ClueData c[] = {{"The suspect mentioned kayaking near a port city where frankincense ships once sailed",     "muscat", CARMEN_CLUE_POSITIVE},
                      {"She said the Omani coastline is more dramatic for kayaking",                                 "muscat", CARMEN_CLUE_POSITIVE},
                      {"The park ranger says it's been quiet all day", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Mangrove Park",       "park",     c, 3); }
    { ClueData c[] = {{"He was reading about Saladin's Citadel on the Nile and comparing palaces",                 "cairo", CARMEN_CLUE_POSITIVE},
                      {"She was exchanging dirhams for Egyptian pounds at the palace gift shop",                     "cairo", CARMEN_CLUE_POSITIVE},
                      {"The palace staff say no visitors have asked about flights today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Qasr Al Watan",       "landmark", c, 3); }

    /* Muscat */
    { ClueData c[] = {{"He was reading about the old Omani stone houses on a spice island off East Africa",        "zanzibar", CARMEN_CLUE_POSITIVE},
                      {"He was comparing this mosque to the white marble one that seats 40,000",                     "abu_dhabi", CARMEN_CLUE_POSITIVE},
                      {"The mosque guide hasn't seen anyone asking for directions lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Sultan Qaboos Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"She mentioned visiting the world's most densely populated city where rickshaws outnumber cars", "dhaka", CARMEN_CLUE_POSITIVE},
                      {"He said he's heading east to a river delta capital in South Asia",                               "dhaka", CARMEN_CLUE_POSITIVE},
                      {"The swimmers here haven't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Bimmah Sinkhole",        "landmark", c, 3); }
    { ClueData c[] = {{"She said a desert city's cultural scene rivals this one -- its opera season is next month", "dubai", CARMEN_CLUE_POSITIVE},
                      {"He was checking Emirates airline schedules heading west across the Gulf",                    "dubai", CARMEN_CLUE_POSITIVE},
                      {"The box office has no record of tickets sold to anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Royal Opera House",      "landmark", c, 3); }

    /* Dakar */
    { ClueData c[] = {{"She said the Art Deco of a Moroccan coastal city reminded her of home",                    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The suspect was booking a Royal Air Maroc flight heading north",                             "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The mosque caretaker has not seen anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Grande Mosquee",              "mosque",   c, 3); }
    { ClueData c[] = {{"He was researching how the slave trade connected West Africa to the land of the Pharaohs", "cairo", CARMEN_CLUE_POSITIVE},
                      {"She said the history museums along the Nile cover similar stories of trade and movement",   "cairo", CARMEN_CLUE_POSITIVE},
                      {"The ferry operators haven't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Goree Island",                "landmark", c, 3); }
    { ClueData c[] = {{"She said the Red City's leather markets put this place to shame",                          "marrakech", CARMEN_CLUE_POSITIVE},
                      {"He mentioned flying to a Moroccan city at the foot of the Atlas Mountains for a festival",  "marrakech", CARMEN_CLUE_POSITIVE},
                      {"The monument guards say no tourists have asked about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "African Renaissance Monument", "landmark", c, 3); }

    /* Bandung */
    { ClueData c[] = {{"He was comparing Braga Street to the famous hawker lane near the twin towers",             "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"She said the coffee here is good but Malaysia's capital has better variety",                 "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The barista hasn't served anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Braga Street",          "cafe",     c, 3); }
    { ClueData c[] = {{"She said the world's largest mosque in Indonesia's capital makes this one look tiny",       "jakarta", CARMEN_CLUE_POSITIVE},
                      {"He had a train ticket back to the Javanese megacity on the coast",                          "jakarta", CARMEN_CLUE_POSITIVE},
                      {"No one at the mosque recalls a visitor asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Masjid Raya Bandung",   "mosque",   c, 3); }
    { ClueData c[] = {{"She was searching for flights to the city where Kota Tua's Dutch colonial buildings stand", "jakarta", CARMEN_CLUE_POSITIVE},
                      {"He mentioned catching the evening train back to Indonesia's capital",                        "jakarta", CARMEN_CLUE_POSITIVE},
                      {"The volcano guides haven't seen any tourists asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Tangkuban Perahu",      "landmark", c, 3); }

    /* Isfahan */
    { ClueData c[] = {{"He said this square is half the world, but a glittering Gulf port has the other half",     "dubai", CARMEN_CLUE_POSITIVE},
                      {"She was comparing Persian miniatures to Ottoman ones across the border in Turkey",           "istanbul", CARMEN_CLUE_POSITIVE},
                      {"The carpet sellers say no one has asked about travel today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Naqsh-e Jahan Square",  "landmark", c, 3); }
    { ClueData c[] = {{"He said the Mevlana Museum in the city where Rumi rests is his next spiritual stop",       "konya", CARMEN_CLUE_POSITIVE},
                      {"She visited the Armenian quarter and said her next stop is a Turkish city famous for Sufis", "konya", CARMEN_CLUE_POSITIVE},
                      {"The priests at the cathedral have not seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Vank Cathedral",         "landmark", c, 3); }
    { ClueData c[] = {{"He was sipping tea and reading about a city where Soviet metro stations hide mosaics underground", "tashkent", CARMEN_CLUE_POSITIVE},
                      {"She said she's following the Silk Road east to Central Asia's largest city",                        "tashkent", CARMEN_CLUE_POSITIVE},
                      {"The tea house regulars don't remember anyone new this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Si-o-se-pol Tea Houses", "cafe",     c, 3); }

    /* Dhaka */
    { ClueData c[] = {{"She said the world's tallest twin towers are calling her name",                            "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"He was checking Malaysia Airlines prices on his phone",                                      "kuala_lumpur", CARMEN_CLUE_POSITIVE},
                      {"The mosque guard says no strangers have attended prayers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Baitul Mukarram Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"She said the Wazir Khan Mosque in the Mughal heartland has even more intricate tiles",     "lahore", CARMEN_CLUE_POSITIVE},
                      {"He photographed the star-shaped tiles and said a Pakistani city's are even finer",          "lahore", CARMEN_CLUE_POSITIVE},
                      {"The caretaker says only locals have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Star Mosque",             "mosque",   c, 3); }
    { ClueData c[] = {{"She was haggling for a rickshaw and talking about frankincense merchants across the ocean", "muscat", CARMEN_CLUE_POSITIVE},
                      {"He mentioned sailing to a port city on the Arabian Sea along an old trade route",            "muscat", CARMEN_CLUE_POSITIVE},
                      {"The rickshaw drivers haven't taken any foreigners to the fort", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Lalbagh Fort",            "landmark", c, 3); }
    { ClueData c[] = {{"She watched the ferries and said the city of Mughal gardens has nothing like this chaos",  "lahore", CARMEN_CLUE_POSITIVE},
                      {"He was comparing the river chaos to nothing -- Pakistan's cultural capital is landlocked",   "lahore", CARMEN_CLUE_POSITIVE},
                      {"The ferry ticket sellers haven't seen any foreigners today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Sadarghat Terminal",      "landmark", c, 3); }

    /* Fez */
    { ClueData c[] = {{"He bragged about the world's oldest university and said a Balkan city with minarets and church bells is next", "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"She had a printout of cheap flights to Bosnia",                                                                 "sarajevo", CARMEN_CLUE_POSITIVE},
                      {"The university librarian hasn't seen any visitors matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Al-Qarawiyyin",       "mosque",   c, 3); }
    { ClueData c[] = {{"He said the city at the foot of the Atlas Mountains is more fun but this one has more soul", "marrakech", CARMEN_CLUE_POSITIVE},
                      {"She was lost in the medina and said the Red City's is easier to navigate",                     "marrakech", CARMEN_CLUE_POSITIVE},
                      {"The leather tannery workers don't recall any foreigners asking questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Fez Medina",          "market",   c, 3); }
    { ClueData c[] = {{"The suspect posted a panoramic photo with a hashtag about the Moroccan city from the classic Bogart film", "casablanca", CARMEN_CLUE_POSITIVE},
                      {"She mentioned a friend waiting for her in Morocco's economic capital",                                       "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The ticket booth operator hasn't sold any tickets to outsiders", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Borj Nord Viewpoint", "landmark", c, 3); }

    /* Zanzibar */
    { ClueData c[] = {{"She mentioned the Omani sultans who once ruled this island",                               "muscat", CARMEN_CLUE_POSITIVE},
                      {"He was reading about Swahili coast trade routes to a frankincense port on the Arabian Sea",  "muscat", CARMEN_CLUE_POSITIVE},
                      {"The shopkeeper in the narrow lanes hasn't seen anyone unusual", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Stone Town",            "landmark", c, 3); }
    { ClueData c[] = {{"He had a printout of flights to a West African city where Sufi brotherhoods lead prayers",  "dakar", CARMEN_CLUE_POSITIVE},
                      {"She mentioned wanting to visit Senegal's Atlantic coast next",                               "dakar", CARMEN_CLUE_POSITIVE},
                      {"The old bath-house guide has not noticed any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Hamamni Persian Baths", "landmark", c, 3); }
    { ClueData c[] = {{"The suspect said this mosque is modest next to the great Mamluk ones by the pyramids",     "cairo", CARMEN_CLUE_POSITIVE},
                      {"She was reading about parks along the Nile on her phone",                                    "cairo", CARMEN_CLUE_POSITIVE},
                      {"The imam has not seen any foreigners at prayers this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Malindi Mosque",        "mosque",   c, 3); }
    { ClueData c[] = {{"She was eating Zanzibar pizza and comparing it to Atlantic coast Moroccan street food",    "casablanca", CARMEN_CLUE_POSITIVE},
                      {"He mentioned catching a flight to Morocco from Dar es Salaam",                               "casablanca", CARMEN_CLUE_POSITIVE},
                      {"The food vendors say no tourists have been asking questions tonight", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Forodhani Gardens",     "market",   c, 3); }

    /* Hyderabad */
    { ClueData c[] = {{"He was comparing the four minarets to the great Badshahi Mosque in Pakistan's Mughal city", "lahore", CARMEN_CLUE_POSITIVE},
                      {"She mentioned flying to Pakistan next",                                                       "lahore", CARMEN_CLUE_POSITIVE},
                      {"The monument guard hasn't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Charminar",         "landmark", c, 3); }
    { ClueData c[] = {{"He said this mosque is beautiful but the one in the Gulf city with the tallest tower is more famous", "dubai", CARMEN_CLUE_POSITIVE},
                      {"She was checking Emirates flights to the Gulf on her phone",                                          "dubai", CARMEN_CLUE_POSITIVE},
                      {"The prayer attendant has not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Mecca Masjid",      "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect spent hours in the jade collection and mentioned the I.M. Pei museum on the Gulf", "doha", CARMEN_CLUE_POSITIVE},
                      {"She said a Gulf peninsula capital has an even finer collection of Islamic art",                  "doha", CARMEN_CLUE_POSITIVE},
                      {"The security staff have not logged any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Salar Jung Museum", "museum",   c, 3); }
    { ClueData c[] = {{"She was buying bangles and said the river capital of Bengal has them even cheaper",         "dhaka", CARMEN_CLUE_POSITIVE},
                      {"He mentioned shipping a box of lac bangles to a friend in Bangladesh",                       "dhaka", CARMEN_CLUE_POSITIVE},
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
