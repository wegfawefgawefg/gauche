#pragma once
#include "route.hpp"

// Two reserved room footprints form one habitat, with ordinary route sockets.
void plan_forest_den(Game& game,FloorPlan& plan);
void carve_forest_den(Game& game,FloorPlan& plan);
void populate_forest_den(Game& game,const FloorPlan& plan);
