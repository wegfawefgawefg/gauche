#pragma once
#include "../game.hpp"

bool eats_meat(EntityKind kind);
bool step_foraging(Game& game, int slot, bool committed_attack = false);
