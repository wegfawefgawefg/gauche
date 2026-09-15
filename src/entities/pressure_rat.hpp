#pragma once
#include "../game.hpp"
enum RatPhase { RatRoam, RatInflate, RatDash, RatCooled, RatSpent };
void init_pressure_rat(Entity& rat);
void step_pressure_rat(Game& game,int slot);
bool cool_pressure_rat(Entity& rat);
bool valid_pressure_rat(const Entity& rat);
