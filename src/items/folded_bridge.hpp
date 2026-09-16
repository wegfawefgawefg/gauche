#pragma once
#include "catalog.hpp"

const RegionalItem* folded_bridge_item(ItemKind kind);
bool place_folded_bridge(Game& game, int slot);
bool bridge_bank(const Tile& tile);
TileKind bridge_underlay(const Prop& plank);
bool valid_bridge_tile(const Tile& tile);
void collapse_bridge_plank(Game& game, Cell cell, Cell source);
void step_bridge_support(Game& game, Cell cell);
