#pragma once
#include "../game.hpp"
bool can_bleed(EntityKind kind);
bool apply_bleeding(Entity& actor,int ticks);
void step_bleeding(Game& game,int slot);
