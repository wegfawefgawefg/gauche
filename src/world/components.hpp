#pragma once
#include "generation_report.hpp"
#include <span>

struct WeightedComponent { int value; const char* name; unsigned weight; };
struct ComponentRoll { int value=-1,record=-1; };
ComponentRoll roll_component(Game& game,GenerationReport* report,GenerationFeature feature,
    int parent,const char* slot,Cell anchor,std::span<const WeightedComponent> choices);
void component_result(GenerationReport* report,ComponentRoll roll,const char* result,std::span<const Cell> cells={});
