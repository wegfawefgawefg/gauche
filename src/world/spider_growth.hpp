#pragma once
#include "route.hpp"
struct GenerationTrace;
void grow_spider_habitats(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_spider_growth(Game& game,FloorPlan& plan,PopulationReport* report=nullptr);
