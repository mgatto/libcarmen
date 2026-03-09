# Translation Roadmap

This file maps each city in the Islamic seed data to its country and primary
language(s), and lists the target locale files for the project.

## Per-City Language Map

| City ID        | City Name     | Country      | Primary Language(s)   | ISO 639-1 |
|----------------|---------------|--------------|-----------------------|-----------|
| istanbul       | Istanbul      | Turkey       | Turkish               | tr        |
| konya          | Konya         | Turkey       | Turkish               | tr        |
| sarajevo       | Sarajevo      | Bosnia       | Bosnian               | bs        |
| cairo          | Cairo         | Egypt        | Arabic                | ar        |
| casablanca     | Casablanca    | Morocco      | Arabic, French        | ar        |
| marrakech      | Marrakech     | Morocco      | Arabic, French        | ar        |
| fez            | Fez           | Morocco      | Arabic                | ar        |
| dakar          | Dakar         | Senegal      | Wolof, French         | wo        |
| zanzibar       | Zanzibar      | Tanzania     | Swahili               | sw        |
| dubai          | Dubai         | UAE          | Arabic                | ar        |
| doha           | Doha          | Qatar        | Arabic                | ar        |
| abu_dhabi      | Abu Dhabi     | UAE          | Arabic                | ar        |
| muscat         | Muscat        | Oman         | Arabic                | ar        |
| amman          | Amman         | Jordan       | Arabic                | ar        |
| beirut         | Beirut        | Lebanon      | Arabic                | ar        |
| isfahan        | Isfahan       | Iran         | Persian (Farsi)       | fa        |
| tashkent       | Tashkent      | Uzbekistan   | Uzbek                 | uz        |
| lahore         | Lahore        | Pakistan     | Urdu, Punjabi         | ur        |
| hyderabad      | Hyderabad     | India        | Urdu, Telugu          | ur        |
| dhaka          | Dhaka         | Bangladesh   | Bangla                | bn        |
| kuala_lumpur   | Kuala Lumpur  | Malaysia     | Malay                 | ms        |
| jakarta        | Jakarta       | Indonesia    | Indonesian            | id        |
| bandung        | Bandung       | Indonesia    | Indonesian, Sundanese | id        |

## Target Locale Files

Each locale file lives at `locales/{code}.json` and follows the flat key-value
format described in the plan. English (`en`) is the reference locale and ships
with the library.

| Code | Language   | Cities Covered                                            | Priority |
|------|------------|-----------------------------------------------------------|----------|
| ar   | Arabic     | Cairo, Casablanca, Marrakech, Fez, Dubai, Doha, Abu Dhabi, Muscat, Amman, Beirut | High |
| tr   | Turkish    | Istanbul, Konya                                           | High     |
| bs   | Bosnian    | Sarajevo                                                  | Medium   |
| fa   | Persian    | Isfahan                                                   | Medium   |
| ur   | Urdu       | Lahore, Hyderabad                                         | Medium   |
| bn   | Bangla     | Dhaka                                                     | Medium   |
| ms   | Malay      | Kuala Lumpur                                               | Medium   |
| id   | Indonesian | Jakarta, Bandung                                          | Medium   |
| sw   | Swahili    | Zanzibar                                                  | Low      |
| wo   | Wolof      | Dakar                                                     | Low      |
| uz   | Uzbek      | Tashkent                                                  | Low      |

Arabic covers the most cities (10) and is the highest-priority translation
target after English. Turkish is second due to Istanbul and Konya being major
stops in the game world.

## Notes for Translators

- Every key in `locales/en.json` must have a corresponding entry in your
  locale file. Missing keys fall back to the raw key string at runtime.
- Clue text contains pronoun template tokens (`{They}`, `{their}`, `{them}`,
  `{they}'s`, `{they}'d`) that are expanded at runtime. Preserve these tokens
  in the translated text and adapt the surrounding grammar as needed.
- City and site names may remain in English or be transliterated/translated
  according to local convention.
- The `_locale` key at the top of each JSON file identifies the locale and is
  not used for display.
