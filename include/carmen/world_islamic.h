#ifndef CARMEN_WORLD_ISLAMIC_H
#define CARMEN_WORLD_ISLAMIC_H

#include "carmen_export.h"
#include "game_world.h"

/* Populate w with the built-in "islamic" world.  The implementation is
   generated at build time from presets/islamic.jsonc by tools/gen_world.c
   (validated then emitted as C); editing the preset regenerates it, and any
   schema/capacity/reference error fails the build.  Does nothing if w is
   NULL; otherwise re-initialises w before populating it. */
CARMEN_API void carmen_world_build_islamic(CarmenWorld *w);

#endif
