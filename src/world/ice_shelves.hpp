#pragma once
#include "route.hpp"

void plan_ice_shelves(Game& game,FloorPlan& plan);
TileKind ice_shelf_floor(const RoomPlan& room,int x,int y);
void connect_ice_shelves(Game& game,FloorPlan& plan);
void carve_shelf_reward(Game& game,FloorPlan& plan);
void populate_shelf_reward(Game& game,const FloorPlan& plan);
