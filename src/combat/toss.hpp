#pragma once
#include "../game.hpp"

constexpr int toss_duration = 24;
constexpr int toss_beat = 6;
bool toss_actor(Game& game,int slot,Cell direction,Cell source,Handle instigator = {});
void step_actor_tosses(Game& game);
bool valid_actor_toss(const Entity& actor);
float actor_toss_height(const Entity& actor);
