#pragma once
#include "../game.hpp"

enum YetiPhase { YetiIdle, YetiGrab, YetiRecover };
void init_yeti(Entity& actor);
void step_yeti(Game& game,int slot);
void interrupt_yeti(Entity& actor);
bool valid_yeti(const Entity& actor);
