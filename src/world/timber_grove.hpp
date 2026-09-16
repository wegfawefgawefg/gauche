#pragma once
#include "route.hpp"
void plan_timber_grove(Game& game,FloorPlan& plan);
void carve_timber_grove(Game& game,FloorPlan& plan);
void populate_timber_grove(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
