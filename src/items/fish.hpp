#pragma once
#include "catalog.hpp"

const RegionalItem* fish_item(ItemKind kind);
constexpr int kelp_health_cost = 3;
void apply_kelp_meal(Game& game, int slot);
bool eat_held_kelp(Game& game, int slot);
