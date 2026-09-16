#pragma once
#include "../game.hpp"
#include <optional>

const IndustrialShift* worker_shift(const Game& game,const Entity& worker);
std::optional<Cell> shift_ore_target(const Game& game,const IndustrialShift& shift,int lane);
bool shift_has_ore(const Game& game,const IndustrialShift& shift);
bool step_shift_hauler(Game& game,int slot,const IndustrialShift& shift);
bool valid_industrial_shifts(const Game& game);
