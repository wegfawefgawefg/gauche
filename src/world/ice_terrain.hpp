#pragma once

#include "route.hpp"

// Regional selection is shared in biome.hpp through game.hpp.

TileKind ice_room_floor(const RoomPlan& room, int x, int y);

void place_ice_terrain(Game& game, const FloorPlan& plan);
