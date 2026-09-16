#pragma once
#include "route.hpp"
#include <span>
struct PopulationReport;
void populate_bear_beds(Game& game,std::span<const Cell> beds,bool family);
std::vector<std::size_t> populate_bear_clearings(Game& game,const FloorPlan& plan,PopulationReport* report);
