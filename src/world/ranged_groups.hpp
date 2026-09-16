#pragma once
#include "room_supplies.hpp"

// Spend ordinary threat on one spaced melee/ranged group, preferring a gunner
// already placed by a specialist room. No extra population allowance.
void place_ranged_group(Game& game,const FloorPlan& plan,
                        const std::vector<std::size_t>& rooms,RoomSupplies& budget);
