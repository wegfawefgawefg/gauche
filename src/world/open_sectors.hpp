#pragma once
#include "route.hpp"
struct GenerationTrace;
void carve_open_sectors(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_open_sectors(Game& game,FloorPlan& plan,PopulationReport* report=nullptr);
