#pragma once
#include "../game.hpp"

enum EelPhase { EelSwim, EelCharge, EelRest };
constexpr int eel_shock_reach = 4;
void init_glass_eel(Entity& eel);
void step_glass_eel(Game& game, int slot);
void interrupt_glass_eel(Entity& eel);
