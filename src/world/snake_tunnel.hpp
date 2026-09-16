#pragma once
#include "route.hpp"
void plan_snake_tunnel(Game& game,FloorPlan& plan);
void carve_snake_tunnel(Game& game,FloorPlan& plan);
void populate_snake_tunnel(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
