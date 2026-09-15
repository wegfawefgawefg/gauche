#pragma once
#include "../game.hpp"
enum GunnerPhase { GunnerReady, GunnerBrace, GunnerBurst, GunnerReload };
void init_rivet_gunner(Entity& actor);
void step_rivet_gunner(Game& game,int slot);
void interrupt_rivet_gunner(Entity& actor);
bool valid_rivet_gunner(const Entity& actor);
