#pragma once
#include "route.hpp"

void plan_ice_thaw(Game& game,FloorPlan& plan);
TileKind ice_thaw_floor(const RoomPlan& room,int x,int y);
void carve_ice_thaw(Game& game,FloorPlan& plan);
void populate_ice_thaw(Game& game,const FloorPlan& plan);
