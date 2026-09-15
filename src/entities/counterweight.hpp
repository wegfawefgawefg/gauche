#pragma once
#include "../game.hpp"
enum WeightPhase { WeightIdle, WeightTravel, WeightWarn, WeightDrop, WeightReel };
constexpr int weight_reach=5;
void init_counterweight(Entity& actor);
void step_counterweight(Game& game,int slot);
void interrupt_counterweight(Entity& actor);
void break_counterweight(Game& game,const Entity& actor);
bool valid_counterweight(const Entity& actor);
