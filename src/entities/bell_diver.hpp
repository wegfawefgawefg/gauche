#pragma once
#include "../game.hpp"

// PHASES: Shared label_a; a submerged body is represented by its bubble sprite.
enum DiverPhase { DiverSwim, DiverBell, DiverRise, DiverHunt, DiverSwing, DiverRecover, DiverSink };
bool diver_submerged(const Entity& diver);
void init_bell_diver(Entity& diver);
void step_bell_diver(Game& game, int slot);
