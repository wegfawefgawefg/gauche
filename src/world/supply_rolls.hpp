#pragma once
#include "components.hpp"
#include "../items/supply.hpp"

// Native/shared supplies, with usefulness and item choices recorded separately.
ComponentRoll roll_native_supply(Game& game,GenerationReport* report,GenerationFeature feature,
    int parent,Cell cell,LootSource source,SupplyNeed need=SupplyNeed::Any);
