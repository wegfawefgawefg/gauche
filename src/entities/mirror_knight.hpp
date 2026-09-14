#pragma once
#include "../game.hpp"

enum KnightPhase { KnightAdvance, KnightRaise, KnightGuard, KnightSwing, KnightRecover };
void init_mirror_knight(Entity& knight);
void step_mirror_knight(Game& game, int slot);
void interrupt_mirror_knight(Entity& knight);
bool knight_reflects(const Entity& knight, Cell incoming);
