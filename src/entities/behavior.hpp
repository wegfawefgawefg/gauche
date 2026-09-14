#pragma once

#include "../game.hpp"
#include "targeting.hpp"
#include <optional>

int nearest_player(const Game& game, Cell from, int radius);
void wander(Game& game, int slot);
bool willing_step(Game& game, int slot, Cell destination);
void approach(Game& game, int slot, Cell target);
void bite(Game& game, int slot, int damage, int range = 1);
void maybe_growl(Game& game, int slot, SoundId sound);

void remember_attacker(Game& game, int slot, Cell from);
void flee(Game& game, int slot, Cell threat);

void pursue(Game& game, int slot, Cell target);

std::optional<Cell> next_route_cell(const Game& game, int slot, Cell target, int budget = 4096);
