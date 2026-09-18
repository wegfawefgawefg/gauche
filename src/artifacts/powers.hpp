#pragma once
#include "../game.hpp"

bool artifact_stackable(ArtifactKind kind);
bool artifact_eligible(const Entity& player, ArtifactKind kind);
void grant_artifact(Entity& player, ArtifactKind kind);
int power_damage(const Entity& player, int base);
int power_healing(const Entity& player, int base);
int power_gold(const Entity& player, int base);
int power_move_interval(Entity& player);
void step_player_powers(Game& game, Entity& player);
bool power_attack_item(ItemKind kind);
void improve_pickup(const Entity& player, Item& item);
bool preserve_throw(Game& game, const Entity& player, ItemKind kind);
int defend_with_powers(Game& game, Entity& target, int damage, bool direct);
int roll_power_damage(Game& game, const Entity& attacker, int base);
