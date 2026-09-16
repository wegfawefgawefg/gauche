#pragma once
#include "../game.hpp"
enum OldGrowthPhase { GrowthRoam, GrowthMaul, GrowthPaw, GrowthRush, GrowthRecover, GrowthStagger };
void init_old_growth_bear(Entity& bear);
void step_old_growth_bear(Game& game,int slot);
void old_growth_timers(Entity& bear);
void interrupt_old_growth(Entity& bear,int damage);
