#pragma once

#include "../game.hpp"

void init_pocket_door(Entity& door);
bool pocket_passage_allowed(const Game& game, Cell from, Cell to);
const Entity* pocket_partner(const Game& game, const Entity& door);
void enter_pocket_door(Game& game, int slot);
