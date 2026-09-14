#pragma once

#include "../game.hpp"
#include <span>

bool scarecrow_shy(const Entity& actor);
bool scarecrow_covers(const Game& game, Cell ward, Cell cell, int radius);
int scarecrow_pressure(const Game& game, Cell cell);
bool scarecrow_allows_step(const Game& game, const Entity& actor, Cell destination);
bool step_scarecrow_fear(Game& game, int slot);
void mark_scarecrow_wards(const Game& game, const Entity& actor, std::span<std::uint8_t> blocked);
