#pragma once

#include "catalog.hpp"

const RegionalItem* forest_remedy(ItemKind kind);
bool use_remedy(Game& game, int slot);
void use_disabled_remedy(Game& game, int slot, const Input& input);
