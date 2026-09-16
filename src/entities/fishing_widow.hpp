#pragma once
#include "../game.hpp"

enum WidowPhase { WidowHunt, WidowWindup, WidowLine, WidowUntangle, WidowWorkWindup, WidowWorkLine, WidowWorkWait };
void init_fishing_widow(Entity& widow);
void step_fishing_widow(Game& game, int slot);
void interrupt_fishing_widow(Entity& widow);
