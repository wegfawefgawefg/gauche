#pragma once
#include "catalog.hpp"

const RegionalItem* industrial_melee_item(ItemKind kind);
// A rubber tap releases pressure through an existing valve; it does not repair
// a leak, replace fuel, or damage the vessel. Returns true for a live boiler.
bool tap_boiler(Game& game, int slot);
