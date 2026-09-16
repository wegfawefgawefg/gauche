#pragma once
#include "route.hpp"
struct GenerationTrace;
void carve_forest_river(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_forest_river(Game& game,FloorPlan& plan,PopulationReport* report=nullptr);
