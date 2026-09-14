#pragma once

#include "../item_pattern.hpp"

struct ThunderChain {
    std::array<Handle, 4> targets{};
    std::array<Cell, 4> cells{};
    int count = 0;
};
bool thunder_arc_clear(const Game& game, Cell from, Cell to);
ThunderChain thunder_chain(const Game& game, int first, ItemPattern pattern);
bool launch_thunder_acorn(Game& game, int owner, const Item& item, Cell direction);
void step_thunder_acorn(Game& game, int slot);
