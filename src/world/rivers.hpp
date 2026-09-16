#pragma once
#include "route.hpp"
#include <span>
struct GenerationTrace;
struct ComponentRoll;
bool carve_forest_river_loop(Game& game,FloorPlan& plan,GenerationTrace* trace,
    std::span<const std::uint8_t> allowed,ComponentRoll style,int strength=1);
void carve_forest_river(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_forest_river(Game& game,FloorPlan& plan,PopulationReport* report=nullptr);
void populate_river_rafts(Game& game,FloorPlan& plan);
