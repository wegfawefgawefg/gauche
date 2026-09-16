#pragma once
#include "route.hpp"

// Two reserved room footprints form one habitat, with ordinary route sockets.
void plan_forest_den(Game& game,FloorPlan& plan);
struct GenerationTrace;
void carve_forest_den(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_forest_den(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
