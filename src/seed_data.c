#include "carmen/seed_data.h"
#include "seed_helpers.h"

/* ---- world builder ---- */

void carmen_seed_build_world(CarmenWorld *w)
{
    if (!w) return;
    carmen_world_init(w);

    /* ------------------------------------------------------------ cities */
    carmen_world_add_city(w, "paris",        "Paris",          NULL, "France",            "Europe",         48.86,    2.35);
    carmen_world_add_city(w, "london",       "London",         NULL, "England",           "Europe",         51.51,   -0.13);
    carmen_world_add_city(w, "rome",         "Rome",           NULL, "Italy",             "Europe",         41.90,   12.50);
    carmen_world_add_city(w, "moscow",       "Moscow",         NULL, "Russia",            "Europe",         55.76,   37.62);
    carmen_world_add_city(w, "oslo",         "Oslo",           NULL, "Norway",            "Europe",         59.91,   10.75);
    carmen_world_add_city(w, "cairo",        "Cairo",          NULL, "Egypt",             "Africa",         30.04,   31.24);
    carmen_world_add_city(w, "nairobi",      "Nairobi",        NULL, "Kenya",             "Africa",         -1.29,   36.82);
    carmen_world_add_city(w, "moroni",       "Moroni",         NULL, "Comoros",           "Africa",        -11.70,   43.26);
    carmen_world_add_city(w, "new_york",     "New York",       NULL, "USA",               "North America",  40.71,  -74.01);
    carmen_world_add_city(w, "mexico_city",  "Mexico City",    NULL, "Mexico",            "North America",  19.43,  -99.13);
    carmen_world_add_city(w, "montreal",     "Montreal",       NULL, "Canada",            "North America",  45.50,  -73.57);
    carmen_world_add_city(w, "lima",         "Lima",           NULL, "Peru",              "South America", -12.05,  -77.04);
    carmen_world_add_city(w, "rio",          "Rio de Janeiro", NULL, "Brazil",            "South America", -22.91,  -43.17);
    carmen_world_add_city(w, "buenos_aires", "Buenos Aires",   NULL, "Argentina",         "South America", -34.60,  -58.38);
    carmen_world_add_city(w, "tokyo",        "Tokyo",          NULL, "Japan",             "Asia",           35.68,  139.69);
    carmen_world_add_city(w, "bangkok",      "Bangkok",        NULL, "Thailand",          "Asia",           13.76,  100.50);
    carmen_world_add_city(w, "kathmandu",    "Kathmandu",      NULL, "Nepal",             "Asia",           27.72,   85.32);
    carmen_world_add_city(w, "new_delhi",    "New Delhi",      NULL, "India",             "Asia",           28.61,   77.21);
    carmen_world_add_city(w, "beijing",      "Beijing",        NULL, "China",             "Asia",           39.90,  116.40);
    carmen_world_add_city(w, "baghdad",      "Baghdad",        NULL, "Iraq",              "Asia",           33.31,   44.37);
    carmen_world_add_city(w, "sydney",       "Sydney",         NULL, "Australia",         "Oceania",       -33.87,  151.21);
    carmen_world_add_city(w, "port_moresby", "Port Moresby",   NULL, "Papua New Guinea",  "Oceania",        -6.21,  147.00);

    /* -------------------------------------------------------- sites & clues */

    /* Paris */
    { ClueData c[] = {{"The suspect asked about the Nile delta",       "cairo",  CARMEN_CLUE_POSITIVE},
                      {"She was carrying a guide to Egyptian pyramids", "cairo",  CARMEN_CLUE_POSITIVE}};
      add_site(w, "paris", "Louvre Museum",     "museum",   c, 2); }
    { ClueData c[] = {{"He mentioned wanting to see Big Ben", "london", CARMEN_CLUE_POSITIVE}};
      add_site(w, "paris", "Eiffel Tower",      "landmark", c, 1); }
    { ClueData c[] = {{"The suspect booked a flight heading southeast", "rome", CARMEN_CLUE_POSITIVE}};
      add_site(w, "paris", "Charles de Gaulle", "airport",  c, 1); }
    { ClueData c[] = {{"Someone matching the description bought a ticket to London", "london", CARMEN_CLUE_POSITIVE}};
      add_site(w, "paris", "Gare du Nord",      "station",  c, 1); }

    /* London */
    { ClueData c[] = {{"The suspect was studying Norse mythology", "oslo",   CARMEN_CLUE_POSITIVE},
                      {"She asked about Viking longships",        "oslo",   CARMEN_CLUE_POSITIVE}};
      add_site(w, "london", "British Museum",   "museum",   c, 2); }
    { ClueData c[] = {{"A one-way ticket was purchased to Moscow", "moscow", CARMEN_CLUE_POSITIVE}};
      add_site(w, "london", "Heathrow Airport", "airport",  c, 1); }
    { ClueData c[] = {{"The suspect mentioned cold winters ahead", "moscow", CARMEN_CLUE_POSITIVE}};
      add_site(w, "london", "Scotland Yard",    "landmark", c, 1); }

    /* Rome */
    { ClueData c[] = {{"The thief was reading a French phrasebook", "paris", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rome", "Colosseum",          "landmark", c, 1); }
    { ClueData c[] = {{"She asked directions to the nearest airport to Paris", "paris", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rome", "Vatican Museum",     "museum",   c, 1); }
    { ClueData c[] = {{"A ticket to Cairo was found in the wastebasket", "cairo", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rome", "Fiumicino Airport",  "airport",  c, 1); }

    /* Moscow */
    { ClueData c[] = {{"The suspect mentioned the Land of the Rising Sun", "tokyo", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moscow", "Kremlin",              "landmark", c, 1); }
    { ClueData c[] = {{"He was studying a map of East Asia", "beijing", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moscow", "Red Square",           "landmark", c, 1); }
    { ClueData c[] = {{"A boarding pass to Beijing was left behind", "beijing", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moscow", "Sheremetyevo Airport", "airport",  c, 1); }

    /* Oslo */
    { ClueData c[] = {{"The suspect wanted to see the Northern Lights",  "oslo",   CARMEN_CLUE_POSITIVE},
                      {"She was exchanging currency for British pounds",  "london", CARMEN_CLUE_POSITIVE}};
      add_site(w, "oslo", "Viking Ship Museum", "museum",   c, 2); }
    { ClueData c[] = {{"He asked about ferries to England", "london", CARMEN_CLUE_POSITIVE}};
      add_site(w, "oslo", "Oslo Opera House",   "landmark", c, 1); }
    { ClueData c[] = {{"The manifest shows a passenger bound for London", "london", CARMEN_CLUE_POSITIVE}};
      add_site(w, "oslo", "Gardermoen Airport", "airport",  c, 1); }

    /* Cairo */
    { ClueData c[] = {{"The suspect was pricing flights to South America", "rio",     CARMEN_CLUE_POSITIVE},
                      {"He mentioned Carnival in Rio",                     "rio",     CARMEN_CLUE_POSITIVE}};
      add_site(w, "cairo", "Great Pyramids",  "landmark", c, 2); }
    { ClueData c[] = {{"She asked about the exchange rate for Kenyan shillings", "nairobi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "cairo", "Egyptian Museum", "museum",   c, 1); }
    { ClueData c[] = {{"A vendor sold him a Portuguese phrasebook", "rio", CARMEN_CLUE_POSITIVE}};
      add_site(w, "cairo", "Khan el-Khalili", "market",   c, 1); }
    { ClueData c[] = {{"The suspect flew south along the African coast", "nairobi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "cairo", "Cairo Airport",   "airport",  c, 1); }

    /* Nairobi */
    { ClueData c[] = {{"The suspect talked about seeing Table Mountain", "moroni", CARMEN_CLUE_POSITIVE}};
      add_site(w, "nairobi", "Nairobi National Park",  "landmark", c, 1); }
    { ClueData c[] = {{"She was reading about the Indian Ocean islands", "moroni", CARMEN_CLUE_POSITIVE}};
      add_site(w, "nairobi", "National Museum",        "museum",   c, 1); }
    { ClueData c[] = {{"He purchased a ticket toward the Comoros archipelago", "moroni", CARMEN_CLUE_POSITIVE}};
      add_site(w, "nairobi", "Jomo Kenyatta Airport",  "airport",  c, 1); }

    /* Moroni */
    { ClueData c[] = {{"The suspect mentioned wanting to see kangaroos", "sydney", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moroni", "Medina of Moroni",  "landmark", c, 1); }
    { ClueData c[] = {{"She had a guidebook to the Australian Outback", "sydney", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moroni", "Old Friday Mosque", "museum",   c, 1); }
    { ClueData c[] = {{"Booking records show a flight toward Oceania", "sydney", CARMEN_CLUE_POSITIVE}};
      add_site(w, "moroni", "Moroni Airport",    "airport",  c, 1); }

    /* New York */
    { ClueData c[] = {{"The thief was studying Spanish colonial architecture", "mexico_city", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_york", "Statue of Liberty",  "landmark", c, 1); }
    { ClueData c[] = {{"He asked about Aztec ruins near a capital city", "mexico_city", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_york", "Metropolitan Museum", "museum",  c, 1); }
    { ClueData c[] = {{"A ticket stub to Mexico City was on the floor", "mexico_city", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_york", "JFK Airport",         "airport", c, 1); }
    { ClueData c[] = {{"She boarded a train heading north to Canada", "montreal", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_york", "Grand Central",       "station", c, 1); }

    /* Mexico City */
    { ClueData c[] = {{"The suspect was reading about Machu Picchu", "lima", CARMEN_CLUE_POSITIVE}};
      add_site(w, "mexico_city", "Teotihuacan",           "landmark", c, 1); }
    { ClueData c[] = {{"He had a phrase card with Portuguese greetings", "rio", CARMEN_CLUE_POSITIVE}};
      add_site(w, "mexico_city", "National Palace",       "museum",   c, 1); }
    { ClueData c[] = {{"She bought a ticket to Lima", "lima", CARMEN_CLUE_POSITIVE}};
      add_site(w, "mexico_city", "Benito Juarez Airport", "airport",  c, 1); }

    /* Montreal */
    { ClueData c[] = {{"The suspect practiced French phrases", "paris", CARMEN_CLUE_POSITIVE}};
      add_site(w, "montreal", "Notre-Dame Basilica", "landmark", c, 1); }
    { ClueData c[] = {{"She asked about the best time to visit Paris", "paris", CARMEN_CLUE_POSITIVE}};
      add_site(w, "montreal", "Montreal Museum",     "museum",   c, 1); }
    { ClueData c[] = {{"A passport stamp shows intent to fly to Europe", "paris", CARMEN_CLUE_POSITIVE}};
      add_site(w, "montreal", "Trudeau Airport",     "airport",  c, 1); }

    /* Lima */
    { ClueData c[] = {{"The suspect mentioned tango dancing in Argentina", "buenos_aires", CARMEN_CLUE_POSITIVE}};
      add_site(w, "lima", "Machu Picchu",          "landmark", c, 1); }
    { ClueData c[] = {{"He was pricing flights to Brazil", "rio", CARMEN_CLUE_POSITIVE}};
      add_site(w, "lima", "Larco Museum",          "museum",   c, 1); }
    { ClueData c[] = {{"A manifest lists the next stop as Buenos Aires", "buenos_aires", CARMEN_CLUE_POSITIVE}};
      add_site(w, "lima", "Jorge Chavez Airport",  "airport",  c, 1); }

    /* Rio de Janeiro */
    { ClueData c[] = {{"The thief spoke of sushi and cherry blossoms", "tokyo", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rio", "Christ the Redeemer", "landmark", c, 1); }
    { ClueData c[] = {{"She mentioned wanting to visit the Great Wall", "beijing", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rio", "Sugarloaf Mountain",  "landmark", c, 1); }
    { ClueData c[] = {{"The suspect headed west across the Pacific", "tokyo", CARMEN_CLUE_POSITIVE}};
      add_site(w, "rio", "Galeao Airport",      "airport",  c, 1); }

    /* Buenos Aires */
    { ClueData c[] = {{"The suspect converted money to US dollars", "new_york", CARMEN_CLUE_POSITIVE}};
      add_site(w, "buenos_aires", "La Boca",        "landmark", c, 1); }
    { ClueData c[] = {{"He was planning a trip to North America", "new_york", CARMEN_CLUE_POSITIVE}};
      add_site(w, "buenos_aires", "Teatro Colon",   "landmark", c, 1); }
    { ClueData c[] = {{"A flight plan to New York was filed", "new_york", CARMEN_CLUE_POSITIVE}};
      add_site(w, "buenos_aires", "Ezeiza Airport", "airport",  c, 1); }

    /* Tokyo */
    { ClueData c[] = {{"The suspect asked about Muay Thai boxing", "bangkok", CARMEN_CLUE_POSITIVE}};
      add_site(w, "tokyo", "Senso-ji Temple",       "landmark", c, 1); }
    { ClueData c[] = {{"She had brochures for Bangkok nightlife", "bangkok", CARMEN_CLUE_POSITIVE}};
      add_site(w, "tokyo", "Tokyo National Museum", "museum",   c, 1); }
    { ClueData c[] = {{"He flew toward Southeast Asia", "bangkok", CARMEN_CLUE_POSITIVE}};
      add_site(w, "tokyo", "Narita Airport",        "airport",  c, 1); }

    /* Bangkok */
    { ClueData c[] = {{"The suspect was researching Himalayan treks", "kathmandu", CARMEN_CLUE_POSITIVE}};
      add_site(w, "bangkok", "Grand Palace",         "landmark", c, 1); }
    { ClueData c[] = {{"She bought a map of Nepal", "kathmandu", CARMEN_CLUE_POSITIVE}};
      add_site(w, "bangkok", "Chatuchak Market",     "market",   c, 1); }
    { ClueData c[] = {{"He booked a flight to Kathmandu", "kathmandu", CARMEN_CLUE_POSITIVE}};
      add_site(w, "bangkok", "Suvarnabhumi Airport", "airport",  c, 1); }

    /* Kathmandu */
    { ClueData c[] = {{"The suspect mentioned the Taj Mahal", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "kathmandu", "Boudhanath Stupa",  "landmark", c, 1); }
    { ClueData c[] = {{"She was exchanging currency for Indian rupees", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "kathmandu", "Durbar Square",     "landmark", c, 1); }
    { ClueData c[] = {{"The flight log shows a departure to New Delhi", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "kathmandu", "Tribhuvan Airport", "airport",  c, 1); }

    /* New Delhi */
    { ClueData c[] = {{"The suspect mentioned ancient Mesopotamia", "baghdad", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_delhi", "Taj Mahal",              "landmark", c, 1); }
    { ClueData c[] = {{"He was reading about the Tigris and Euphrates", "baghdad", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_delhi", "Red Fort",               "landmark", c, 1); }
    { ClueData c[] = {{"She flew west toward the Middle East", "baghdad", CARMEN_CLUE_POSITIVE}};
      add_site(w, "new_delhi", "Indira Gandhi Airport",  "airport",  c, 1); }

    /* Beijing */
    { ClueData c[] = {{"The suspect wanted to ride the Trans-Siberian Railway", "moscow", CARMEN_CLUE_POSITIVE}};
      add_site(w, "beijing", "Great Wall",              "landmark", c, 1); }
    { ClueData c[] = {{"He had a Russian dictionary in his bag", "moscow", CARMEN_CLUE_POSITIVE}};
      add_site(w, "beijing", "Forbidden City",          "museum",   c, 1); }
    { ClueData c[] = {{"A ticket to Moscow was purchased at the counter", "moscow", CARMEN_CLUE_POSITIVE}};
      add_site(w, "beijing", "Beijing Capital Airport", "airport",  c, 1); }

    /* Baghdad */
    { ClueData c[] = {{"The suspect talked about monsoon season in India", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "baghdad", "Al-Mutanabbi Street", "landmark", c, 1); }
    { ClueData c[] = {{"She had a guidebook with a tiger on the cover", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "baghdad", "Iraq Museum",         "museum",   c, 1); }
    { ClueData c[] = {{"He booked passage east toward the subcontinent", "new_delhi", CARMEN_CLUE_POSITIVE}};
      add_site(w, "baghdad", "Baghdad Airport",     "airport",  c, 1); }

    /* Sydney */
    { ClueData c[] = {{"The suspect was reading about Melanesian cultures", "port_moresby", CARMEN_CLUE_POSITIVE}};
      add_site(w, "sydney", "Sydney Opera House",       "landmark", c, 1); }
    { ClueData c[] = {{"He mentioned wanting to dive in coral reefs", "port_moresby", CARMEN_CLUE_POSITIVE}};
      add_site(w, "sydney", "Harbour Bridge",           "landmark", c, 1); }
    { ClueData c[] = {{"She flew north toward Papua New Guinea", "port_moresby", CARMEN_CLUE_POSITIVE}};
      add_site(w, "sydney", "Kingsford Smith Airport",  "airport",  c, 1); }

    /* Port Moresby */
    { ClueData c[] = {{"The suspect was planning a trip to Rome", "rome", CARMEN_CLUE_POSITIVE}};
      add_site(w, "port_moresby", "National Museum",        "museum",   c, 1); }
    { ClueData c[] = {{"She converted currency to euros", "rome", CARMEN_CLUE_POSITIVE}};
      add_site(w, "port_moresby", "Varirata National Park", "landmark", c, 1); }
    { ClueData c[] = {{"A flight to Europe was booked under a false name", "rome", CARMEN_CLUE_POSITIVE}};
      add_site(w, "port_moresby", "Jacksons Airport",       "airport",  c, 1); }

    /* ---------------------------------------------------------- connections */
    /* Europe */
    add_route(w, "paris",       "london",       340);
    add_route(w, "paris",       "rome",         1100);
    add_route(w, "london",      "oslo",         1150);
    add_route(w, "london",      "moscow",       2500);
    add_route(w, "rome",        "cairo",        2130);
    add_route(w, "moscow",      "beijing",      5800);
    add_route(w, "oslo",        "london",       1150);
    /* Africa */
    add_route(w, "cairo",       "nairobi",      3500);
    add_route(w, "cairo",       "rio",          9500);
    add_route(w, "nairobi",     "moroni",       1600);
    /* North America */
    add_route(w, "new_york",    "london",       5570);
    add_route(w, "new_york",    "mexico_city",  3360);
    add_route(w, "new_york",    "montreal",     530);
    add_route(w, "montreal",    "paris",        5500);
    /* South America */
    add_route(w, "mexico_city", "lima",         4240);
    add_route(w, "lima",        "buenos_aires", 3140);
    add_route(w, "lima",        "rio",          3770);
    add_route(w, "buenos_aires","new_york",     8500);
    /* Asia */
    add_route(w, "tokyo",       "bangkok",      4600);
    add_route(w, "bangkok",     "kathmandu",    2900);
    add_route(w, "kathmandu",   "new_delhi",    800);
    add_route(w, "new_delhi",   "baghdad",      3400);
    add_route(w, "beijing",     "moscow",       5800);
    add_route(w, "beijing",     "tokyo",        2100);
    /* Oceania */
    add_route(w, "sydney",      "port_moresby", 4600);
    add_route(w, "moroni",      "sydney",       10500);
    /* Cross-continental */
    add_route(w, "rio",         "tokyo",        18500);
    add_route(w, "rome",        "paris",        1100);
    add_route(w, "baghdad",     "new_delhi",    3400);
    add_route(w, "port_moresby","rome",         14000);
}
