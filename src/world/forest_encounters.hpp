#pragma once
#include "room_supplies.hpp"
#include <span>

void populate_forest_encounters(Game& game,const FloorPlan& plan,std::span<const std::size_t> rooms,
    PopulationReport* population,GenerationReport* decisions);
