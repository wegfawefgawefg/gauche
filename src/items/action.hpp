#pragma once
#include "../game.hpp"

int item_windup(const Item& item);
void cancel_item_action(Entity& user);
bool step_melee_action(Game& game, int slot, const Input& input);
