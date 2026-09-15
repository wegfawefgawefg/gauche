#pragma once
#include "catalog.hpp"

const RegionalItem* circuit_item(ItemKind kind);
bool circuit_space(const Game& game, Cell cell);
bool place_circuit_item(Game& game, Cell cell, const Item& item);
