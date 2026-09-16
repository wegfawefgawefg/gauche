#pragma once
#include "../game.hpp"

bool entity_has_flame(const Entity& actor);
bool flame_cell(const Game& game,Cell cell);
bool hot_cell(const Game& game, Cell cell);
bool warm_cell(const Game& game, Cell cell);
bool warm_surface(Game& game, Cell cell, int ticks);
bool freeze_water(Game& game, Cell cell, int ticks);
bool thaw_water(Game& game, Cell cell);
void quench_cell(Game& game, Cell cell, SoundId sound = SoundId::ColdQuench);
void step_temperature(Game& game);
