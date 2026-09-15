#pragma once
#include "../game.hpp"
bool exposed_fuse(const Entity& actor);
bool quench_exposed_fuse(Entity& actor,bool cold);
void wet_landed_fuse(Game& game,int slot);
