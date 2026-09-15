#pragma once
#include "../game.hpp"

enum class ProjectileKind { Arrow, Bomb, Flask, Rocket, Hook, Drill, Blink, Rock, Boomerang, Net, Firecracker, Mixture, ThunderAcorn, FrostPuff, IceBrick, Snowball, PrismBomb, FishingHook, WidowHook, Flare, EchoPebble, Harpoon, ThawCharge, CoalSpit, QuarryCharge, FoamCan, ThrownBolt, ChainHook, TarSpit };
constexpr int bomb_fuse_ticks = 150;
void init_projectile(Entity& entity);
void step_projectile(Game& game, int slot);
bool launch_projectile(Game& game, int owner_slot, const Item& item, Cell direction, int reach);
int projectile_step_ticks(const Entity& entity);

bool projectile_blocked(const Game& game, Cell cell,bool overhead=false,bool through_grates=false);
Cell bomb_landing(const Game& game, Cell origin, Cell facing, int reach);
