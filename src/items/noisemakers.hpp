#pragma once

#include "catalog.hpp"

const RegionalItem* forest_noisemaker(ItemKind kind);
bool use_noisemaker(Game& game, int slot, Cell direction);
void step_firecracker(Game& game, int slot);
