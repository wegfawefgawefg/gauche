#pragma once

#include "route.hpp"

// ORDER: Keep the current fire/ice run order until all four zones are integrated.
inline bool ice_floor(int floor) { return floor >= 9 && floor <= 12; }

TileKind ice_room_floor(const RoomPlan& room, int x, int y);
