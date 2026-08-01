# Marrakech: City Data (parked candidate)

Marrakech was part of the original Islamic-world seed but was dropped when the roster was aligned to the 22-city cross-sphere network (see [cross-sphere-connectivity.md](cross-sphere-connectivity.md)). This note preserves its full data so it can be reinstated later, and captures the open question of whether it is a more attractive Islamicate city than Casablanca for the Maghreb (Group A) slot.

## Why consider it over Casablanca

Casablanca is Morocco's modern economic capital: its headline sights skew twentieth-century (Art Deco downtown, Morocco Mall, the 1993 Hassan II Mosque). Marrakech reads as far more classically "Islamicate": a Almoravid/Almohad imperial capital with an intact medina (a UNESCO World Heritage site), the Koutoubia minaret that served as the model for the Giralda and the Hassan Tower, the Ben Youssef Madrasa, Saadian Tombs, and the theatrical Jemaa el-Fnaa. For a game about a "Modern Muslim Traveler" it offers a richer, more legible sense of place. The trade-off: Casablanca is the bigger real-world air hub, so any swap should re-create Casablanca's external connectivity on Marrakech (see swap notes below). One data gap worth noting: Marrakech's original site set had no `type.mosque` entry, so if it is promoted, adding the Koutoubia Mosque is recommended both thematically and to match the one-mosque convention other cities follow.

## Seed record

Coordinates and identity as they appeared in [src/seed_data_islamic.c](../../src/seed_data_islamic.c) before removal:

```c
carmen_world_add_city(w, "marrakech", "city.marrakech.name", "city.marrakech.local_name", "city.marrakech.country", "city.marrakech.continent", 31.63, -8.00);
```

Locale keys (still present in [locales/en.json](../../locales/en.json)):

```json
"city.marrakech.name": "Marrakech",
"city.marrakech.local_name": "مراكش",
"city.marrakech.country": "Morocco",
"city.marrakech.continent": "Africa"
```

## Sites

Original three sites (note: no mosque among them):

```c
add_site(w, "marrakech", "jemaa",     "site.marrakech.jemaa",     "type.landmark");
add_site(w, "marrakech", "macaal",    "site.marrakech.macaal",    "type.museum");
add_site(w, "marrakech", "le_jardin", "site.marrakech.le_jardin", "type.park");
```

```json
"site.marrakech.jemaa": "Jemaa el-Fnaa",
"site.marrakech.macaal": "MACAAL",
"site.marrakech.le_jardin": "Le Jardin Secret"
```

Suggested fourth site if reinstated (adds the missing mosque and the city's signature landmark):

```c
add_site(w, "marrakech", "koutoubia", "site.marrakech.koutoubia", "type.mosque");
```

```json
"site.marrakech.koutoubia": "Koutoubia Mosque"
```

## Inbound clue pool

```c
{ const char *k[] = {"clue.marrakech.inbound.0", "clue.marrakech.inbound.1", "clue.marrakech.inbound.2", "clue.marrakech.inbound.3"};
  add_inbound_clues(w, "marrakech", k, 4); }
```

```json
"clue.marrakech.inbound.0": "{They} had a riad reservation in the Red City at the foot of the Atlas",
"clue.marrakech.inbound.1": "{They} was headed for the ochre-walled city and its Jemaa el-Fnaa square",
"clue.marrakech.inbound.2": "{They} mentioned snake charmers and souks below snow-capped mountains",
"clue.marrakech.inbound.3": "{They} was craving the night food stalls of the Red City"
```

## Artifact

Marrakech had no artifact in the original catalog. If promoted it should get one to be eligible as a crime-scene origin; a Saadian-era or Almohad piece (for example a carved cedar minbar panel, or a zellige fountain basin) would fit the catalog's style. Note `zellige_mosaic_fez` already covers Marinid zellige from Fez, so pick a distinct object.

## Prior connections

In the original seed Marrakech sat on two short intra-Morocco train links only:

- `casablanca--marrakech` 240 km, train
- `marrakech--fez` 400 km, train

Both were dropped in the cross-sphere rework (Casablanca now links Istanbul, Cairo, and Dakar; the Maghreb intra-sphere edge is `tunis--fez`).

## Swap considerations (replace Casablanca with Marrakech)

If Marrakech takes Casablanca's Group A (Maghreb) slot, the cleanest approach is to inherit Casablanca's exact edges so the 46-edge topology and degree distribution are preserved:

- `istanbul--casablanca` -> `istanbul--marrakech` (~3200 km, flight)
- `cairo--casablanca` -> `cairo--marrakech` (~3400 km, flight)
- `casablanca--dakar` -> `marrakech--dakar` (~2650 km, flight)

That keeps Marrakech at degree 3 (matching Casablanca today) and leaves Fez and Tunis untouched. Alternatively, keep both cities and expand the roster beyond 22, which would require revisiting the network doc rather than a like-for-like swap.
