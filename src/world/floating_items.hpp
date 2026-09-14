#pragma once
#include "../game.hpp"

constexpr int item_float_reach = 16;
constexpr int item_float_beat = 12;
bool floating_item(const Entity& item);
bool float_water(const Tile& tile);
bool float_cell_free(const Game& game, Cell cell, int cargo_slot);
bool start_item_float(Game& game, int slot, Cell direction);
void stop_item_float(Game& game, Entity& item);
void step_floating_item(Game& game, int slot);
