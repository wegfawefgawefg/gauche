#pragma once
#include "../game.hpp"

enum BoilerPhase { BoilerIdle, BoilerTell, BoilerVent };
constexpr int boiler_fuel_limit = 7200;
void init_boiler_tank(Entity& tank);
void step_boiler_tank(Game& game, int slot);
bool arm_boiler(Game& game, Entity& tank);
void damage_boiler(Game& game, int slot);
void rupture_boiler(Game& game, int slot);
bool feed_boiler(Game& game, Cell cell);
std::vector<Cell> boiler_splash_cells(const Game& game, Cell source, Cell direction, bool rupture);
void boiler_splash(Game& game, int slot, bool rupture);
int boiler_at(const Game& game, Cell cell);
bool valid_boiler_state(const Entity& actor);
