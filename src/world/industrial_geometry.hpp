#pragma once
#include "route.hpp"

void plan_industrial_geometry(Game& game,FloorPlan& plan);
void carve_industrial_geometry(Game& game,FloorPlan& plan);
void populate_industrial_links(Game& game,const FloorPlan& plan);
