#pragma once
#include "../game.hpp"
enum MoldThiefPhase { MoldSeek, MoldGrab, MoldReturn, MoldSeal, MoldRest, MoldFlee };
void init_mold_thief(Entity& actor);
void step_mold_thief(Game& game,int slot);
void interrupt_mold_thief(Entity& actor);
void hurt_mold_thief(Game& game,Entity& actor,Cell source);
bool valid_mold_thief(const Entity& actor);
