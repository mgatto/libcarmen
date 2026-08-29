# Cross-Sphere Connectivity Analysis

**Status: realized at runtime.** Sphere tags in `presets/islamic.jsonc` still name the seven cultural groups below. The connection graph itself is no longer this 46-edge static list: `carmen_world_generate_connections()` produces a connected 3-regular 33-edge flight graph per case (in-sphere seed plus cross-sphere stub matching). The edge lists, degree distribution, and per-city audits below are a **reference sketch** of a fully cross-connected static network and are not compiled into the library.

Canonical cultural sub-sphere taxonomy and a fully cross-connected reference network for the Islamic-world city set. Every city belongs to one sub-sphere and connects to at least two *other* sub-spheres, so no city is culturally isolated. This is the taxonomy the runtime connection generator uses to seed in-sphere edges and prefer cross-sphere stubs. **Does not modify the plan.**

Recompute note: this revision drops Doha and Amman from the roster (they were removed from the Arab East group and not reassigned), adds Kano (Nigeria) to Sub-Saharan Africa, moves Sarajevo into Turkey/Balkans, and renames Maghreb/Balkans to Maghreb. Six edges were introduced to keep every city in the 3-5 degree range and reaching at least two external spheres after the removals; they are marked below. Result: 22 cities, 46 edges.

## Cultural sub-spheres (7 groups)

| Group | Cities |
|-------|--------|
| **A - Maghreb** | Casablanca, Fez, Tunis |
| **B - Turkey/Balkans** | Istanbul, Konya, Sarajevo |
| **C - Arab East (Mashreq + Gulf)** | Cairo, Damascus, Dubai, Muscat |
| **D - Sub-Saharan Africa** | Dakar, Zanzibar, Kano |
| **E - Iran/Central Asia** | Isfahan, Samarkand, Kabul |
| **F - South Asia** | Lahore, Dhaka, Hyderabad |
| **G - Southeast Asia** | Kuala Lumpur, Jakarta, Yogyakarta |

## Changes from the previous revision

- **Removed (9 edges)** with Doha and Amman: `istanbul--amman`, `cairo--amman`, `dubai--amman`, `muscat--amman`, `amman--samarkand`, `damascus--doha`, `muscat--doha`, `yogyakarta--doha`, `sarajevo--doha`.
- **Added (6 edges)** to restore degree and cross-sphere coverage: `dakar--kano`, `fez--kano`, `cairo--kano` (Kano into the network), `muscat--dubai` (Muscat back to degree 3), `damascus--sarajevo` (Sarajevo's second external sphere after losing Doha), `samarkand--damascus` (Samarkand's C link, replacing the lost `amman--samarkand`).

## Edge list (22 cities, 46 edges)

### Intra-sphere edges (13)

istanbul--sarajevo, tunis--fez, muscat--dubai, zanzibar--dakar, dakar--kano, isfahan--samarkand, kabul--samarkand, kabul--isfahan, lahore--dhaka, hyderabad--lahore, hyderabad--dhaka, kuala_lumpur--jakarta, yogyakarta--kuala_lumpur

### Cross-sphere edges (33)

istanbul--cairo, istanbul--casablanca, istanbul--kabul, cairo--casablanca, cairo--zanzibar, cairo--dakar, cairo--kano, casablanca--dakar, fez--sarajevo, fez--kuala_lumpur, fez--kano, fez--damascus, dubai--lahore, dubai--isfahan, dubai--yogyakarta, isfahan--konya, samarkand--lahore, samarkand--damascus, dhaka--kuala_lumpur, hyderabad--kuala_lumpur, tunis--sarajevo, tunis--konya, tunis--dhaka, damascus--isfahan, damascus--konya, damascus--sarajevo, kabul--lahore, kabul--hyderabad, jakarta--dhaka, jakarta--hyderabad, jakarta--muscat, muscat--zanzibar, zanzibar--yogyakarta

## Degree distribution (all within 3-5)

- **Degree 5 (9 cities)**: Cairo, Damascus, Fez, Isfahan, Kabul, Lahore, Dhaka, Hyderabad, Kuala Lumpur
- **Degree 4 (8 cities)**: Istanbul, Sarajevo, Dubai, Tunis, Dakar, Zanzibar, Samarkand, Jakarta
- **Degree 3 (5 cities)**: Konya, Muscat, Casablanca, Kano, Yogyakarta

## Cross-sphere audit

Every city connects to **at least 2** different external groups. Neighbors are annotated with their group; the final column lists the distinct external spheres reached.

| City | Group | Connections by group | External spheres |
|------|-------|---------------------|------------------|
| Casablanca | A | istanbul(B), cairo(C), dakar(D) | B, C, D |
| Fez | A | tunis(A), sarajevo(B), damascus(C), kuala_lumpur(G), kano(D) | B, C, D, G |
| Tunis | A | fez(A), sarajevo(B), konya(B), dhaka(F) | B, F |
| Istanbul | B | sarajevo(B), cairo(C), casablanca(A), kabul(E) | A, C, E |
| Konya | B | isfahan(E), tunis(A), damascus(C) | A, C, E |
| Sarajevo | B | istanbul(B), fez(A), tunis(A), damascus(C) | A, C |
| Cairo | C | istanbul(B), casablanca(A), zanzibar(D), dakar(D), kano(D) | A, B, D |
| Damascus | C | isfahan(E), fez(A), konya(B), sarajevo(B), samarkand(E) | A, B, E |
| Dubai | C | muscat(C), lahore(F), isfahan(E), yogyakarta(G) | E, F, G |
| Muscat | C | dubai(C), zanzibar(D), jakarta(G) | D, G |
| Dakar | D | zanzibar(D), kano(D), cairo(C), casablanca(A) | A, C |
| Zanzibar | D | dakar(D), cairo(C), muscat(C), yogyakarta(G) | C, G |
| Kano | D | dakar(D), fez(A), cairo(C) | A, C |
| Isfahan | E | samarkand(E), kabul(E), dubai(C), konya(B), damascus(C) | B, C |
| Samarkand | E | isfahan(E), kabul(E), lahore(F), damascus(C) | C, F |
| Kabul | E | samarkand(E), isfahan(E), lahore(F), hyderabad(F), istanbul(B) | B, F |
| Lahore | F | dhaka(F), hyderabad(F), dubai(C), samarkand(E), kabul(E) | C, E |
| Dhaka | F | lahore(F), hyderabad(F), kuala_lumpur(G), jakarta(G), tunis(A) | A, G |
| Hyderabad | F | lahore(F), dhaka(F), kuala_lumpur(G), jakarta(G), kabul(E) | E, G |
| Kuala Lumpur | G | jakarta(G), yogyakarta(G), dhaka(F), hyderabad(F), fez(A) | A, F |
| Jakarta | G | kuala_lumpur(G), dhaka(F), hyderabad(F), muscat(C) | C, F |
| Yogyakarta | G | kuala_lumpur(G), dubai(C), zanzibar(D) | C, D |

## Group reachability

| Group | Reaches groups | Count |
|-------|---------------|-------|
| A (Maghreb) | B, C, D, F, G | 5 of 6 |
| B (Turkey/Balkans) | A, C, E | 3 of 6 |
| C (Arab East) | A, B, D, E, F, G | **all 6** |
| D (Sub-Saharan Africa) | A, C, G | 3 of 6 |
| E (Iran/Central Asia) | B, C, F | 3 of 6 |
| F (South Asia) | A, C, E, G | 4 of 6 |
| G (Southeast Asia) | A, C, D, F | 4 of 6 |

Group C (Arab East) is the universal connector, reaching all six other spheres. Group A (Maghreb) reaches five (all but E). The remaining groups each reach three or four, and every city individually reaches at least two external spheres.
