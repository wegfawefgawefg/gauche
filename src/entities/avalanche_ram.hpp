#pragma once
#include "../game.hpp"

enum RamPhase { RamRoam, RamPaw, RamLunge, RamRecover, RamStagger };
void init_avalanche_ram(Entity& ram);
void step_avalanche_ram(Game& game, int slot);
void interrupt_avalanche_ram(Entity& ram);
