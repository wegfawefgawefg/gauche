#pragma once
#include "route.hpp"
void plan_giant_tree(Game& game,FloorPlan& plan);
void carve_giant_tree(Game& game,FloorPlan& plan);
void populate_giant_tree(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
