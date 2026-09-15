#pragma once
#include "projectile.hpp"
bool launch_tar_spit(Game& game,int owner,Cell target);
void step_tar_spit(Game& game,int slot);
Cell tar_spit_next(const Entity& shot);
bool valid_tar_spit(const Entity& shot);
