#pragma once
#include "../game.hpp"
enum AshPhase { AshDormant, AshRising, AshRoam, AshSwipe, AshRecover, AshSettling };
void init_ash_sleeper(Entity& actor);
void step_ash_sleeper(Game& game,int slot);
void rouse_ash_sleeper(Game& game,Entity& actor,Cell noise);
void hurt_ash_sleeper(Game& game,Entity& actor,Cell attacker);
void hear_ash_sleepers(Game& game,Cell origin,int radius);
bool damp_ash_sleeper(Entity& actor);
void interrupt_ash_sleeper(Entity& actor);
bool valid_ash_sleeper(const Entity& actor);
