#pragma once
#include "../game.hpp"

enum class ProjectileKind { Arrow, Bomb, Flask, Rocket, Hook, Drill, Swap };
constexpr int bomb_fuse_ticks = 150;
void init_projectile(Entity& entity);
void step_projectile(Game& game, int slot);
bool launch_projectile(Game& game, int owner_slot, const Item& item, Cell direction, int reach);
int projectile_step_ticks(const Entity& entity);

bool projectile_blocked(const Game& game, Cell cell);
Cell bomb_landing(const Game& game, Cell origin, Cell facing, int reach);
