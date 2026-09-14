#pragma once
#include "catalog.hpp"

const RegionalItem* quarry_item(ItemKind kind);
constexpr int brick_throw_hold_ticks = 18;
bool step_ice_brick(Game& game, int slot, const Input& input);
