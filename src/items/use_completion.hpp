#pragma once
#include "../game.hpp"

// Successful one-shot uses only. Guns/continuous actions own their completion.
void finish_item_use(Game& game, Entity& user, Item& item, ItemKind used_kind, Cell target,
                     Cell direction, int cooldown);
