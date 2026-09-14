#pragma once

#include "../game.hpp"

void place_coins(Game& game, Cell cell, int amount);
void collect_coins(Game& game, Entity& player);
void drop_enemy_loot(Game& game, const Entity& enemy);
