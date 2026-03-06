#include "carmen/seed_data_islamic.h"
#include "seed_helpers.h"

#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

/* ---- world builder: Modern Muslim Traveler ---- */

void carmen_seed_build_islamic_world(CarmenWorld *w)
{
    if (!w) return;
    carmen_world_init(w);

    /* ------------------------------------------------------------ cities */
    carmen_world_add_city(w, "istanbul",     "Istanbul",      "Turkey",      "Europe",  41.01,   28.98);
    carmen_world_add_city(w, "sarajevo",     "Sarajevo",      "Bosnia",      "Europe",  43.86,   18.41);
    carmen_world_add_city(w, "cairo",        "Cairo",         "Egypt",       "Africa",  30.04,   31.24);
    carmen_world_add_city(w, "casablanca",   "Casablanca",    "Morocco",     "Africa",  33.57,   -7.59);
    carmen_world_add_city(w, "marrakech",    "Marrakech",     "Morocco",     "Africa",  31.63,   -8.00);
    carmen_world_add_city(w, "fez",          "Fez",           "Morocco",     "Africa",  34.03,   -5.00);
    carmen_world_add_city(w, "dakar",        "Dakar",         "Senegal",     "Africa",  14.72,  -17.47);
    carmen_world_add_city(w, "dubai",        "Dubai",         "UAE",         "Asia",    25.20,   55.27);
    carmen_world_add_city(w, "doha",         "Doha",          "Qatar",       "Asia",    25.29,   51.53);
    carmen_world_add_city(w, "abu_dhabi",    "Abu Dhabi",     "UAE",         "Asia",    24.45,   54.65);
    carmen_world_add_city(w, "zanzibar",     "Zanzibar",      "Tanzania",    "Africa",  -6.16,   39.19);
    carmen_world_add_city(w, "hyderabad",    "Hyderabad",     "India",       "Asia",    17.38,   78.49);
    carmen_world_add_city(w, "muscat",       "Muscat",        "Oman",        "Asia",    23.59,   58.55);
    carmen_world_add_city(w, "amman",        "Amman",         "Jordan",      "Asia",    31.95,   35.93);
    carmen_world_add_city(w, "beirut",       "Beirut",        "Lebanon",     "Asia",    33.89,   35.50);
    carmen_world_add_city(w, "isfahan",      "Isfahan",       "Iran",        "Asia",    32.65,   51.68);
    carmen_world_add_city(w, "konya",        "Konya",         "Turkey",      "Asia",    37.87,   32.48);
    carmen_world_add_city(w, "tashkent",     "Tashkent",      "Uzbekistan",  "Asia",    41.30,   69.28);
    carmen_world_add_city(w, "lahore",       "Lahore",        "Pakistan",    "Asia",    31.55,   74.35);
    carmen_world_add_city(w, "dhaka",        "Dhaka",         "Bangladesh",  "Asia",    23.81,   90.41);
    carmen_world_add_city(w, "kuala_lumpur", "Kuala Lumpur",  "Malaysia",    "Asia",     3.14,  101.69);
    carmen_world_add_city(w, "jakarta",      "Jakarta",       "Indonesia",   "Asia",    -6.21,  106.85);
    carmen_world_add_city(w, "bandung",      "Bandung",       "Indonesia",   "Asia",    -6.91,  107.61);

    /* -------------------------------------------------------- sites & clues */

    /* Istanbul */
    { ClueData c[] = {{"The suspect was googling Bosnian coffee recipes",         "sarajevo"},
                      {"She mentioned wanting to visit the place where East meets West... in the Balkans", "sarajevo"},
                      {"The muezzin shook his head -- no strangers at today's prayer", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Suleymaniye Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"He was browsing gallery listings in a city on the Bosphorus... no, farther south", "cairo"},
                      {"She asked the guard about exhibitions in Cairo's modern art scene",                "cairo"},
                      {"The gallery staff haven't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Istanbul Modern",     "museum",   c, 3); }
    { ClueData c[] = {{"The suspect ordered a ferry ticket and asked about the Whirling Dervishes", "konya"},
                      {"She had a Konya bus timetable folded in her back pocket",                   "konya"},
                      {"The fish vendor hasn't seen anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Kadikoy Food Market", "market",   c, 3); }
    { ClueData c[] = {{"A booking for Beirut showed up on her phone",                               "beirut"},
                      {"He was asking other worshippers about cheap flights to Lebanon",             "beirut"},
                      {"The imam has not seen any unfamiliar faces at today's prayers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "istanbul", "Camlica Mosque",      "mosque",   c, 3); }

    /* Dubai */
    { ClueData c[] = {{"The suspect asked the concierge about flights to Doha",   "doha"},
                      {"She was comparing the Museum of Islamic Art to this one",  "doha"},
                      {"The receptionist hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Museum of the Future",      "museum",   c, 3); }
    { ClueData c[] = {{"He mentioned pearl diving traditions across the Gulf",     "muscat"},
                      {"She was reading about Oman's coast and old dhow routes",   "muscat"},
                      {"The guide said no foreigners have visited this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Al Fahidi Historical Dist", "landmark", c, 3); }
    { ClueData c[] = {{"The suspect was pricing business class to Lahore",                  "lahore"},
                      {"She had a Pakistan International Airlines itinerary on her phone",  "lahore"},
                      {"The concierge hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Dubai Mall",                "market",   c, 3); }
    { ClueData c[] = {{"She asked the imam about Persian tile patterns",                       "isfahan"},
                      {"He was sketching the dome and said Isfahan's are far more intricate",   "isfahan"},
                      {"The mosque attendant has not noticed any unfamiliar visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dubai", "Jumeirah Mosque",           "mosque",   c, 3); }

    /* Kuala Lumpur */
    { ClueData c[] = {{"The suspect was on a video call discussing Bandung coffee", "bandung"},
                      {"He asked which Indonesian city has the best cafe scene",     "bandung"},
                      {"The observation deck staff haven't noticed anyone unusual today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Petronas Towers",       "landmark", c, 3); }
    { ClueData c[] = {{"She had a Jakarta transit guide folded in her bag",              "jakarta"},
                      {"She was planning a trip to Indonesia's capital next",             "jakarta"},
                      {"The curator said no one has asked about travel lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Islamic Arts Museum",   "museum",   c, 3); }
    { ClueData c[] = {{"The suspect was craving kacchi biryani from Bengal",             "dhaka"},
                      {"He mentioned flying to Bangladesh for a friend's wedding",       "dhaka"},
                      {"The hawker stall owner shrugged -- too busy to notice anyone", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Jalan Alor",            "market",   c, 3); }
    { ClueData c[] = {{"He photographed the mihrab and mentioned one grander in Jakarta", "jakarta"},
                      {"She compared the architecture to Jakarta's Istiqlal",             "jakarta"},
                      {"The worshippers here don't recall seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "kuala_lumpur", "Masjid Negara",         "mosque",   c, 3); }

    /* Doha */
    { ClueData c[] = {{"The suspect kept comparing this collection to the Louvre branch nearby", "abu_dhabi"},
                      {"He mentioned wanting to see the Sheikh Zayed Mosque next",               "abu_dhabi"},
                      {"The security guard says no tourists have been asking unusual questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Museum of Islamic Art",     "museum",   c, 3); }
    { ClueData c[] = {{"She was haggling for saffron and muttering about Isfahan's bazaars",       "isfahan"},
                      {"He said the best saffron comes from Iran and he's heading there next",      "isfahan"},
                      {"The spice merchant has not seen anyone suspicious this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Souq Waqif",               "market",   c, 3); }
    { ClueData c[] = {{"He had a documentary about Swahili trading ports bookmarked on his tablet", "zanzibar"},
                      {"She mentioned booking a dhow trip along the East African coast",            "zanzibar"},
                      {"The cultural center staff have not seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Katara Cultural Village",  "landmark", c, 3); }
    { ClueData c[] = {{"The suspect mentioned studying at a university in Abu Dhabi",  "abu_dhabi"},
                      {"She had an Abu Dhabi campus visitor badge in her purse",        "abu_dhabi"},
                      {"Nobody at the campus remembers seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "doha", "Education City",            "landmark", c, 3); }

    /* Amman */
    { ClueData c[] = {{"The suspect was reading about Pharaonic architecture", "cairo"},
                      {"He had a layover booking for Cairo on his tablet",      "cairo"},
                      {"The site guards have not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "The Citadel",              "landmark", c, 3); }
    { ClueData c[] = {{"She was scrolling Instagram photos of Beirut nightlife",             "beirut"},
                      {"He said Gemmayzeh Street in Beirut has better nightlife",             "beirut"},
                      {"The barista doesn't recall anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "Rainbow Street Cafes",     "cafe",     c, 3); }
    { ClueData c[] = {{"The suspect asked about Friday prayer times in Dubai",          "dubai"},
                      {"She had a Dubai hotel confirmation email open on her tablet",    "dubai"},
                      {"The imam said no visitors have asked about travel recently", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "amman", "King Abdullah Mosque",     "mosque",   c, 3); }

    /* Casablanca */
    { ClueData c[] = {{"The suspect mentioned the Atlas Mountains are just a train ride away", "marrakech"},
                      {"She had a riad reservation in the Red City",                           "marrakech"},
                      {"The mosque caretaker has not seen any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Hassan II Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"He was studying a map of West Africa with Dakar circled",       "dakar"},
                      {"She was asking about flights to Senegal at a travel agency",     "dakar"},
                      {"The shopkeeper hasn't seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Habous Quarter",    "market",   c, 3); }
    { ClueData c[] = {{"A one-way ticket to Cairo was in the trash",                         "cairo"},
                      {"He had a guidebook for Egyptian pyramids in his shopping bag",        "cairo"},
                      {"The mall security hasn't flagged anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Morocco Mall",      "market",   c, 3); }
    { ClueData c[] = {{"The suspect photographed the facade and said Fez has better examples", "fez"},
                      {"He mentioned catching the next train to Fez to see the medina",        "fez"},
                      {"No one in the cafe recalls a traveler asking for directions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "casablanca", "Art Deco Downtown", "landmark", c, 3); }

    /* Jakarta */
    { ClueData c[] = {{"The suspect said the largest mosque in SE Asia still can't match KL's museum", "kuala_lumpur"},
                      {"She booked a flight north to Malaysia",                                        "kuala_lumpur"},
                      {"The security guard has not noticed anyone suspicious", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Istiqlal Mosque",    "mosque",   c, 3); }
    { ClueData c[] = {{"He mentioned wanting to see volcanoes near Bandung",                                  "bandung"},
                      {"She said the volcanic landscape outside Bandung is worth the train ride",              "bandung"},
                      {"The museum attendant doesn't recall any visitors asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "National Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"A taxi receipt to the train station for Bandung was in her wallet",  "bandung"},
                      {"He was checking train schedules to Bandung on his phone",             "bandung"},
                      {"The old Dutch building's caretaker saw nothing out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Kota Tua",           "landmark", c, 3); }
    { ClueData c[] = {{"The suspect ordered es kopi susu and researched KL street food",      "kuala_lumpur"},
                      {"She said the nasi lemak in KL is better than anything here",           "kuala_lumpur"},
                      {"The cafe owner hasn't seen anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "jakarta", "Menteng Cafes",      "cafe",     c, 3); }

    /* Sarajevo */
    { ClueData c[] = {{"The suspect was reading about Ottoman architecture in Istanbul", "istanbul"},
                      {"She had a Turkish Airlines boarding pass stub",                  "istanbul"},
                      {"The coppersmiths haven't noticed any outsiders this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Bascarsija",             "market",   c, 3); }
    { ClueData c[] = {{"He asked about flights to Morocco's oldest imperial city",                 "fez"},
                      {"She mentioned wanting to study at the world's oldest university",          "fez"},
                      {"The imam says no visitors have asked about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "Gazi Husrev-beg Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect muttered something about the Bosphorus at sunset",              "istanbul"},
                      {"She had an Istanbul guidebook with dog-eared pages on Sultanahmet",         "istanbul"},
                      {"The museum staff don't remember any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "sarajevo", "War Tunnel Museum",      "museum",   c, 3); }

    /* Tashkent */
    { ClueData c[] = {{"The suspect was comparing Silk Road tilework to Isfahan's", "isfahan"},
                      {"She asked about direct flights to Iran",                    "isfahan"},
                      {"The librarian guarding the Quran manuscript hasn't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Khast Imam Complex", "mosque",   c, 3); }
    { ClueData c[] = {{"He was buying dried fruits to bring to friends in Lahore",            "lahore"},
                      {"She said Lahore's food scene would blow this bazaar away",             "lahore"},
                      {"The bazaar vendors have not seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Chorsu Bazaar",      "market",   c, 3); }
    { ClueData c[] = {{"The suspect photographed Soviet mosaics and mentioned Lahore's Mughal ones",       "lahore"},
                      {"He was telling a local that Lahore Fort makes every other monument look small",     "lahore"},
                      {"The metro staff haven't noticed anyone unusual riding the trains", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Tashkent Metro",     "landmark", c, 3); }
    { ClueData c[] = {{"A hotel confirmation in Isfahan was on his phone",                          "isfahan"},
                      {"She said the tilework here is beautiful but Isfahan's is unmatchable",      "isfahan"},
                      {"The prayer attendant hasn't seen any foreigners this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "tashkent", "Minor Mosque",       "mosque",   c, 3); }

    /* Cairo */
    { ClueData c[] = {{"The suspect said this park's view rivals the Citadel in Amman", "amman"},
                      {"She was checking visa requirements for Jordan",                 "amman"},
                      {"The park gardener hasn't noticed anyone out of place", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Al-Azhar Park",          "park",     c, 3); }
    { ClueData c[] = {{"He mentioned flying to Casablanca for a connecting flight",   "casablanca"},
                      {"She was checking Royal Air Maroc flights on her laptop",       "casablanca"},
                      {"The street vendors haven't noticed anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Tahrir Square",           "landmark", c, 3); }
    { ClueData c[] = {{"The suspect bought a Moroccan tea set as a gift",                   "casablanca"},
                      {"He mentioned heading west to Morocco before flying home",            "casablanca"},
                      {"The merchants say business has been slow -- no new faces", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Khan el-Khalili",         "market",   c, 3); }
    { ClueData c[] = {{"She was sketching the muqarnas and comparing them to Abu Dhabi's",            "abu_dhabi"},
                      {"He was comparing the massive scale here to Sheikh Zayed's in the Gulf",        "abu_dhabi"},
                      {"The prayer attendants say no tourists have been around today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "cairo", "Sultan Hassan Mosque",    "mosque",   c, 3); }

    /* Beirut */
    { ClueData c[] = {{"The suspect mentioned crossing into Jordan tomorrow",  "amman"},
                      {"He was converting Lebanese lira to Jordanian dinar",   "amman"},
                      {"The mosque guard hasn't noticed any outsiders this afternoon", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Mohammad Al-Amin Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"She photographed street art and said Istanbul's scene is better",   "istanbul"},
                      {"He said he's heading to Istanbul to see its street art next",        "istanbul"},
                      {"The gallery owner shook his head -- no one asked about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "Gemmayzeh Street Art",    "landmark", c, 3); }
    { ClueData c[] = {{"A brochure for the Egyptian Museum was in his jacket",                      "cairo"},
                      {"She was comparing Phoenician artifacts to Egyptian ones in Cairo",           "cairo"},
                      {"The front desk has no record of anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "beirut", "National Museum",         "museum",   c, 3); }

    /* Marrakech */
    { ClueData c[] = {{"The suspect asked a storyteller about the oldest university in the world", "fez"},
                      {"She had a train ticket to Fez in her back pocket",                        "fez"},
                      {"The snake charmer says he sees thousands of faces -- none stood out", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Jemaa el-Fnaa",      "landmark", c, 3); }
    { ClueData c[] = {{"He was comparing this gallery to Dakar's art scene",                    "dakar"},
                      {"She mentioned the African contemporary art scene in Senegal",           "dakar"},
                      {"The gallery guard hasn't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "MACAAL",              "museum",   c, 3); }
    { ClueData c[] = {{"The suspect searched for Casablanca flights on her phone",     "casablanca"},
                      {"He said he's catching a train to Casablanca tonight",           "casablanca"},
                      {"The riad keeper hasn't had any unusual guests this month", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "marrakech", "Le Jardin Secret",   "park",     c, 3); }

    /* Lahore */
    { ClueData c[] = {{"The suspect was comparing this mosque to the Jama Masjid in Delhi... no, to Dhaka's", "dhaka"},
                      {"He mentioned visiting relatives in Bangladesh next",                                   "dhaka"},
                      {"The prayer hall attendant says no foreigners have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Badshahi Mosque",      "mosque",   c, 3); }
    { ClueData c[] = {{"She was reading about Mughal architecture in Tashkent",                          "tashkent"},
                      {"He mentioned comparing these Mughal walls to Silk Road forts in Tashkent",       "tashkent"},
                      {"The fort guard has not seen any suspicious visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Lahore Fort",          "landmark", c, 3); }
    { ClueData c[] = {{"The suspect was searching Dubai restaurant reviews while eating nihari",             "dubai"},
                      {"She said Dubai's Pakistani restaurants can't compare to the real thing here",         "dubai"},
                      {"The waiter doesn't recall anyone unusual this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Food Street",          "market",   c, 3); }
    { ClueData c[] = {{"He bought silk fabric and said Tashkent's bazaars have better ikat",            "tashkent"},
                      {"She was asking a silk merchant about supply routes through Uzbekistan",          "tashkent"},
                      {"The bazaar shopkeepers haven't seen anyone out of the ordinary", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "lahore", "Anarkali Bazaar",      "market",   c, 3); }

    /* Konya */
    { ClueData c[] = {{"The suspect was reading Rumi and said she'd follow his path to Iran", "isfahan"},
                      {"He had an Iran visa application form half-filled",                    "isfahan"},
                      {"The museum docent hasn't seen any visitors asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Mevlana Museum",    "museum",   c, 3); }
    { ClueData c[] = {{"She asked the guide about bus connections to Istanbul",                                     "istanbul"},
                      {"He said Istanbul's Suleymaniye is more impressive but this one has more soul",              "istanbul"},
                      {"The worshippers don't remember seeing a stranger", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Alaeddin Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect said he'd rather be in Istanbul for the weekend",    "istanbul"},
                      {"She asked a villager about the fastest bus to Istanbul",          "istanbul"},
                      {"The villagers say no tourists have come through lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "konya", "Sille Village",     "landmark", c, 3); }

    /* Abu Dhabi */
    { ClueData c[] = {{"The suspect said this mosque rivals anything in Muscat", "muscat"},
                      {"She was checking Oman Air flights on her phone",         "muscat"},
                      {"The mosque attendant has not noticed anyone unfamiliar", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Sheikh Zayed Mosque", "mosque",   c, 3); }
    { ClueData c[] = {{"He spent an hour in the Islamic wing and said Doha's collection is deeper",        "doha"},
                      {"She said she prefers Doha's Museum of Islamic Art for pure Islamic collections",   "doha"},
                      {"The ticket desk has no record of anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Louvre Abu Dhabi",    "museum",   c, 3); }
    { ClueData c[] = {{"The suspect mentioned kayaking in Muscat next weekend",       "muscat"},
                      {"She said Muscat's coastline is more dramatic for kayaking",    "muscat"},
                      {"The park ranger says it's been quiet all day", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Mangrove Park",       "park",     c, 3); }
    { ClueData c[] = {{"A Cairo boarding pass fell out of her notebook",                   "cairo"},
                      {"He was reading about Cairo's Citadel and comparing palaces",       "cairo"},
                      {"The palace staff say no visitors have asked about flights today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "abu_dhabi", "Qasr Al Watan",       "landmark", c, 3); }

    /* Muscat */
    { ClueData c[] = {{"The suspect mentioned Zanzibar's old Omani stone houses", "zanzibar"},
                      {"He was comparing this mosque to Sheikh Zayed's",          "abu_dhabi"},
                      {"The mosque guide hasn't seen anyone asking for directions lately", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Sultan Qaboos Mosque",   "mosque",   c, 3); }
    { ClueData c[] = {{"She mentioned swimming in sinkholes and then flying to Dhaka", "dhaka"},
                      {"He said he's heading east to Dhaka after this",                "dhaka"},
                      {"The swimmers here haven't seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Bimmah Sinkhole",        "landmark", c, 3); }
    { ClueData c[] = {{"The suspect bought opera tickets and mentioned Dubai's next season",   "dubai"},
                      {"She said the Dubai Opera's season next month is worth the trip",        "dubai"},
                      {"The box office has no record of tickets sold to anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "muscat", "Royal Opera House",      "landmark", c, 3); }

    /* Dakar */
    { ClueData c[] = {{"The suspect was booking a Royal Air Maroc flight heading north", "casablanca"},
                      {"She mentioned Casablanca's Art Deco reminded her of home",       "casablanca"},
                      {"The mosque caretaker has not seen anyone asking about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Grande Mosquee",              "mosque",   c, 3); }
    { ClueData c[] = {{"He was researching the slave trade and mentioned Cairo's history museums",          "cairo"},
                      {"She said Cairo's historical museums cover similar stories of trade and movement",   "cairo"},
                      {"The ferry operators haven't seen anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "Goree Island",                "landmark", c, 3); }
    { ClueData c[] = {{"The suspect said Marrakech's markets are better for leather",  "marrakech"},
                      {"He mentioned flying to Marrakech for a music festival",         "marrakech"},
                      {"The monument guards say no tourists have asked about flights", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dakar", "African Renaissance Monument", "landmark", c, 3); }

    /* Bandung */
    { ClueData c[] = {{"The suspect was comparing Braga Street to Jalan Alor in KL", "kuala_lumpur"},
                      {"She said the coffee here is good but KL has better variety",  "kuala_lumpur"},
                      {"The barista hasn't served anyone fitting that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Braga Street",          "cafe",     c, 3); }
    { ClueData c[] = {{"He had a train ticket back to Jakarta in his pocket",                   "jakarta"},
                      {"She said Jakarta's Istiqlal makes this one look tiny",                   "jakarta"},
                      {"No one at the mosque recalls a visitor asking about travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Masjid Raya Bandung",   "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect was searching for Jakarta flights on her phone",    "jakarta"},
                      {"He mentioned catching the evening train back to Jakarta",        "jakarta"},
                      {"The volcano guides haven't seen any tourists asking about onward travel", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "bandung", "Tangkuban Perahu",      "landmark", c, 3); }

    /* Isfahan */
    { ClueData c[] = {{"The suspect said this square is half the world, but Dubai has the other half", "dubai"},
                      {"He was comparing Persian miniatures to Ottoman ones in Istanbul",              "istanbul"},
                      {"The carpet sellers say no one has asked about travel today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Naqsh-e Jahan Square",  "landmark", c, 3); }
    { ClueData c[] = {{"She visited the Armenian quarter and mentioned Konya next",         "konya"},
                      {"He said Konya's Mevlana Museum is his next spiritual stop",         "konya"},
                      {"The priests at the cathedral have not seen any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Vank Cathedral",         "landmark", c, 3); }
    { ClueData c[] = {{"The suspect was sipping tea and reading about Tashkent's Silk Road history", "tashkent"},
                      {"She said she's following the Silk Road east to Tashkent",                    "tashkent"},
                      {"The tea house regulars don't remember anyone new this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "isfahan", "Si-o-se-pol Tea Houses", "cafe",     c, 3); }

    /* Dhaka */
    { ClueData c[] = {{"The suspect was checking Malaysia Airlines prices on his phone", "kuala_lumpur"},
                      {"She mentioned the Petronas Towers as her next stop",             "kuala_lumpur"},
                      {"The mosque guard says no strangers have attended prayers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Baitul Mukarram Mosque",  "mosque",   c, 3); }
    { ClueData c[] = {{"He photographed the star-shaped tiles and compared them to Lahore's",       "lahore"},
                      {"She said Lahore's Wazir Khan Mosque has even more intricate tiles",          "lahore"},
                      {"The caretaker says only locals have visited today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Star Mosque",             "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect was haggling for a rickshaw to the river and mentioned Muscat's port", "muscat"},
                      {"He mentioned sailing to Muscat on an old trade route",                             "muscat"},
                      {"The rickshaw drivers haven't taken any foreigners to the fort", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Lalbagh Fort",            "landmark", c, 3); }
    { ClueData c[] = {{"She watched the ferries and said Lahore has nothing like this chaos",                  "lahore"},
                      {"He was comparing the river chaos to nothing -- Lahore is landlocked, he laughed",      "lahore"},
                      {"The ferry ticket sellers haven't seen any foreigners today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "dhaka", "Sadarghat Terminal",      "landmark", c, 3); }

    /* Fez */
    { ClueData c[] = {{"The suspect bragged about visiting the world's oldest university then heading to Sarajevo", "sarajevo"},
                      {"He had a printout of cheap flights to Bosnia",                                              "sarajevo"},
                      {"The university librarian hasn't seen any visitors matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Al-Qarawiyyin",       "mosque",   c, 3); }
    { ClueData c[] = {{"She was lost in the medina and said Marrakech's is easier to navigate",    "marrakech"},
                      {"He said Marrakech is more fun but Fez has more soul",                       "marrakech"},
                      {"The leather tannery workers don't recall any foreigners asking questions", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Fez Medina",          "market",   c, 3); }
    { ClueData c[] = {{"The suspect took a panoramic photo and posted it with #NextStopCasablanca",  "casablanca"},
                      {"She mentioned a friend waiting for her in Casablanca",                        "casablanca"},
                      {"The ticket booth operator hasn't sold any tickets to outsiders", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "fez", "Borj Nord Viewpoint", "landmark", c, 3); }

    /* Zanzibar */
    { ClueData c[] = {{"The suspect was reading about Swahili coast trade routes to Muscat", "muscat"},
                      {"She mentioned the Omani sultans who once ruled this island",         "muscat"},
                      {"The shopkeeper in the narrow lanes hasn't seen anyone unusual", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Stone Town",            "landmark", c, 3); }
    { ClueData c[] = {{"He had a printout of Dakar flights from Dar es Salaam",             "dakar"},
                      {"She mentioned wanting to visit West Africa's biggest city next",     "dakar"},
                      {"The old bath-house guide has not noticed any strangers", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Hamamni Persian Baths", "landmark", c, 3); }
    { ClueData c[] = {{"The suspect said this mosque is modest next to Cairo's Sultan Hassan", "cairo"},
                      {"She was reading about Al-Azhar Park on her phone",                     "cairo"},
                      {"The imam has not seen any foreigners at prayers this week", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Malindi Mosque",        "mosque",   c, 3); }
    { ClueData c[] = {{"She was eating Zanzibar pizza and comparing it to Casablanca street food",  "casablanca"},
                      {"He mentioned catching a flight to Morocco from Dar es Salaam",               "casablanca"},
                      {"The food vendors say no tourists have been asking questions tonight", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "zanzibar", "Forodhani Gardens",     "market",   c, 3); }

    /* Hyderabad */
    { ClueData c[] = {{"The suspect was comparing the four minarets to Lahore's Badshahi Mosque", "lahore"},
                      {"She mentioned flying to Pakistan next",                                   "lahore"},
                      {"The monument guard hasn't noticed anyone matching that description", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Charminar",         "landmark", c, 3); }
    { ClueData c[] = {{"He said this mosque is beautiful but Dubai's Jumeirah is more famous",    "dubai"},
                      {"She was checking Emirates flights to the Gulf on her phone",               "dubai"},
                      {"The prayer attendant has not seen any strangers today", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Mecca Masjid",      "mosque",   c, 3); }
    { ClueData c[] = {{"The suspect spent hours in the jade collection and mentioned Doha's museum",  "doha"},
                      {"She said Doha's Museum of Islamic Art has an even finer collection",           "doha"},
                      {"The security staff have not logged any unusual visitors", NULL, CARMEN_CLUE_NEGATIVE}};
      add_site(w, "hyderabad", "Salar Jung Museum", "museum",   c, 3); }
    { ClueData c[] = {{"She was buying bangles and said Dhaka's markets are cheaper",                  "dhaka"},
                      {"He mentioned shipping a box of lac bangles to a friend in Bangladesh",          "dhaka"},
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
