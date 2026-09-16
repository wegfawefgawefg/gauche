#pragma once
#include "projectile.hpp"

bool wooden_arrow(const Entity& shot);
bool burning_arrow(const Entity& shot);
void arrow_flame_contact(Game& game,Entity& shot,Cell cell);
bool douse_arrow(Entity& shot);
void ignite_arrow_impact(Game& game,const Entity& shot,Cell cell);
