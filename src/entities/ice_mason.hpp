#pragma once
#include "../game.hpp"

enum MasonPhase { MasonReady, MasonSeek, MasonCut, MasonBuild, MasonSwing, MasonRest };
void init_ice_mason(Entity& mason);
void step_ice_mason(Game& game, int slot);
void interrupt_ice_mason(Entity& mason);
