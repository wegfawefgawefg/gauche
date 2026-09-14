#pragma once
#include "../game.hpp"

bool ice_cover_space(const Game& game, Cell cell);
bool place_ice_cover(Game& game, Cell cell);
bool melt_ice_cover(Game& game, Cell cell);
void age_ice_cover(Game& game, Cell cell);
