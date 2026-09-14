#pragma once

#include "../game.hpp"

bool place_prop(Stage& stage, Cell cell, PropKind kind, std::uint8_t variant = 0);
bool hit_prop(Game& game, Cell cell, int damage, Cell source);
void step_on_prop(Game& game, int actor_slot);
