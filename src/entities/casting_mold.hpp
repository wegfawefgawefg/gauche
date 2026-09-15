#pragma once
#include "../game.hpp"
void init_casting_mold(Entity& mold);
bool empty_casting_mold(const Entity& mold);
bool seal_casting_mold(Game& game,Entity& mold,Item& cargo);
Item recoverable_mold_item(const Game& game,Cell cell);
int release_mold_item(Game& game,Cell source,Cell destination);
bool finish_mold_death(Game& game,int slot);
bool valid_casting_mold(const Entity& mold);
