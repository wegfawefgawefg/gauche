#pragma once

#include "../game.hpp"

int nearest_player(const Game& game, Cell from, int radius);
void wander(Game& game, int slot);
void approach(Game& game, int slot, Cell target);
void bite(Game& game, int slot, int damage, int range = 1);
void maybe_growl(Game& game, int slot, SoundId sound);
