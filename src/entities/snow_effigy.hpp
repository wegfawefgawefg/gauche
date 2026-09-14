#pragma once
#include "../game.hpp"

enum EffigyPhase { EffigyIdle, EffigyStrike, EffigyRecover };
void init_snow_effigy(Entity& effigy);
void step_snow_effigy(Game& game, int slot);
void interrupt_snow_effigy(Entity& effigy);
void thaw_snow_effigy(Game& game, int slot);
bool observer_faces_cell(const Game& game, const Entity& observer, Cell cell);
