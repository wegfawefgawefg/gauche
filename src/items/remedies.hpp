#pragma once

#include "catalog.hpp"

constexpr int fungal_bread_sleep_ticks=10*60;

const RegionalItem* forest_remedy(ItemKind kind);
bool use_remedy(Game& game, int slot);
void use_disabled_remedy(Game& game, int slot, const Input& input);
