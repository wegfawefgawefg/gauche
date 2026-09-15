#pragma once
#include "catalog.hpp"

const RegionalItem* harpoon_item(ItemKind kind);
bool step_harpoon_action(Game& game, int slot, const Input& input);
void release_held_harpoon(Game& game, Entity& owner);
