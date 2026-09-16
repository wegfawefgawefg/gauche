#pragma once
#include "../game.hpp"

enum { ZombieReady, ZombieWindup, ZombieRecover };
bool step_zombie_swipe(Game& game,int slot);
bool begin_zombie_swipe(Game& game,int slot,bool hunt_animals);
void interrupt_zombie_swipe(Entity& actor);
bool valid_zombie_swipe(const Entity& actor);
