#pragma once

#include "../game.hpp"

constexpr int parry_ticks = 10;
constexpr int parry_wear = 4;
bool parry_active(const Entity& actor);
bool parry_ranged_hit(Game& game, int defender_slot, Cell incoming);
void reflect_projectile(Entity& shot, const Entity& defender, int defender_slot);
