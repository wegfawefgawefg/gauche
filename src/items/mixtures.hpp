#pragma once
#include "catalog.hpp"
#include <vector>

const RegionalItem* forest_mixture(ItemKind kind);
bool throw_mixture(Game& game, int slot, Cell direction);
void step_mixture(Game& game, int slot);
void splash_mixture(Game& game, const Item& item, Cell center, Cell source);
std::vector<Cell> mixture_cells(const Game& game, const Item& item, Cell center);
bool eat_rotten_fruit(Game& game, int slot);
