#pragma once
#include "route.hpp"

void place_ice_materials(Game& game,const FloorPlan& plan);
bool melt_ice_wall(Game& game,Cell cell,int damage,Cell source);
// Stored contents are released once the enclosing wall actually opens.
void release_wall_contents(Game& game,Cell cell);
