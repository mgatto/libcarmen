# Customization -- Guide for Custom Seed Data

This note is for people who build their own world (custom `CarmenWorld` seed data) instead of using the built-in `carmen_seed_build_islamic_world()`. It documents an invariant you must uphold so that case generation produces coherent narratives.

## Every city must have a matching artifact

`carmen_case_generate()` is artifact-driven: it picks the stolen artifact first, then seeds the trail at that artifact's `origin_city_id`, so the crime scene the player starts in (`case->origin_id == case->trail[0]`) always equals `case->artifact.origin_city_id`. This is what keeps the case briefing ("Someone stole X from Y") consistent with where gameplay actually begins.

For this to work cleanly in a custom world, **every city in your seed data must be the origin city of at least one artifact in the catalog** (`CARMEN_ARTIFACTS[]` in `src/artifact.c`). Concretely: for every city ID you pass to `carmen_world_add_city()`, there must be an artifact whose `origin_city_id` equals that city ID.

If you add a city that no artifact points to, that city can still appear as an intermediate stop or hideout on a trail, but it can never be the trail origin -- so no case will ever be framed as a theft from that city. That is a legitimate design choice, but if you intend every city to be a possible crime scene, you must supply a corresponding artifact.

## What happens if the invariant is violated

If a custom world contains **none** of the catalog's artifact origin cities, `carmen_case_generate()` falls back to choosing a random start city and a random artifact. In that fallback the narrative may not match gameplay (the artifact's origin city and the player's start city can differ). The fallback exists only so the library can still run on arbitrary worlds (for example, minimal test worlds); it is not intended for shipping content. Provide an artifact per city to avoid it entirely.

## Checklist for custom seed data

- Add each city with `carmen_world_add_city()`.
- Add at least one artifact per city to `CARMEN_ARTIFACTS[]` whose `origin_city_id` matches the city ID (and bump `CARMEN_ARTIFACT_COUNT` in `include/carmen/artifact.h`).
- Provide the i18n keys (`name`, `description`) referenced by each artifact in your locale files.
- Ensure the graph is connected enough that any artifact origin city can reach a full-length trail for your hardest difficulty / configured `trail_length`.
