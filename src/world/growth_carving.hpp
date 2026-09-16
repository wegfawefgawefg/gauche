#pragma once
#include "route.hpp"

// Before objective entities exist, test the planned key/switch route with its door shut.
bool generation_lock_intact(const Game& game,const FloorPlan& plan);
// Thin dry paths through the current geometry, not rectangular room reservations.
// Preserve reachable room anchors plus the objective/exit when adding solid terrain.
std::vector<std::uint8_t> generation_walking_routes(const Game& game,const FloorPlan& plan);
bool generation_exit_reachable(const Game& game,const FloorPlan& plan);
