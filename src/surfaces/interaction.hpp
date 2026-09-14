#pragma once
#include "../game.hpp"

bool surface_wet(const Tile& tile);
bool pour_surface(Game& game, Cell cell, LiquidKind kind, int ticks);
bool ignite_surface(Game& game, Cell cell);
void contact_surface(Game& game, int slot);
void step_surfaces(Game& game);
int surface_step_delay(const Tile& tile);

bool smoke_hides(const Stage& stage, Cell from, Cell to);
