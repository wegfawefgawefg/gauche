#pragma once
#include "route.hpp"
void plan_timber_grove(Game& game,FloorPlan& plan);
void carve_timber_grove(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void dress_timber_grove(Game& game,FloorPlan& plan,TimberGrove& grove,GenerationTrace* trace);
void populate_timber_grove(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
