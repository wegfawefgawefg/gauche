#pragma once
#include "../game.hpp"
enum SlagSnailPhase { SlagCrawl, SlagTuck, SlagLunge, SlagRecover, SlagCooled };
void init_slag_snail(Entity& actor);
void step_slag_snail(Game& game,int slot);
void interrupt_slag_snail(Entity& actor);
bool cool_slag_snail(Entity& actor);
bool slag_snail_shelled(const Entity& actor);
bool valid_slag_snail(const Entity& actor);
