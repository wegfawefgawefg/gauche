#pragma once
#include "catalog.hpp"

constexpr int siphon_portion = 300, siphon_capacity = 1800;
const RegionalItem* heat_siphon_item(ItemKind kind);
bool draw_siphon_heat(Game& game, int slot);
bool discharge_siphon(Game& game, int slot);
bool step_siphon_action(Game& game, int slot, const Input& input);
int extract_heat(Game& game, Cell cell, int limit);
