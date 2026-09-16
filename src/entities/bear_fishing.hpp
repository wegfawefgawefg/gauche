#pragma once
#include "../game.hpp"

enum BearWorkPhase { BearFishPaw=3, BearFishFollow=4 };
void start_bear_fishing(Entity& bear);
void interrupt_bear_fishing(Entity& bear);
bool step_bear_fishing(Game& game,int slot);
bool valid_bear(const Entity& bear);
