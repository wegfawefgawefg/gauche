#pragma once

#include "../game.hpp"

void init_entity(Game& game, Entity& entity);
void step_entity(Game& game, int slot);
void step_player(Game& game, int slot, const Input& input);
void drop_player_item(Game& game, Entity& player);
void step_entity_timers(Game& game, int slot);

void init_player(Entity& entity);
void init_zombie(Game& game, Entity& entity);
void init_chicken(Game& game, Entity& entity);
void init_bat(Entity& entity);
void init_wolf(Entity& entity);
void init_dog(Entity& entity);
void init_bear(Entity& entity);
void init_bunny(Entity& entity);
void init_ember(Entity& entity);
void init_spawner(Entity& entity);
void init_den(Entity& entity);
void init_rail(Entity& entity);
void init_train(Entity& entity);
void init_fixture(Entity& entity);

void step_zombie(Game& game, int slot);
void step_chicken(Game& game, int slot);
void step_bat(Game& game, int slot);
void step_wolf(Game& game, int slot);
void step_dog(Game& game, int slot);
void step_bear(Game& game, int slot);
void step_bunny(Game& game, int slot);
void step_ember(Game& game, int slot);
void step_spawner(Game& game, int slot);
void step_den(Game& game, int slot);
void step_rail(Game& game, int slot);
void step_train(Game& game, int slot);

void init_zombie_stack(Game& game, Entity& entity);
void step_zombie_stack(Game& game, int slot);
bool topple_zombie_stack(Game& game, int slot);

void spawn_chicken_family(Game& game, Cell cell);

void init_boar(Entity& entity);
void step_boar(Game& game, int slot);
void init_thorn_snail(Entity& entity);
void step_thorn_snail(Game& game, int slot);
void init_lantern_moth(Entity& entity);
void step_lantern_moth(Game& game, int slot);
void init_spore_toad(Entity& entity);
void step_spore_toad(Game& game, int slot);
void init_crate_mimic(Entity& entity);
void step_crate_mimic(Game& game, int slot);

void init_root_turret(Entity& entity);
void step_root_turret(Game& game, int slot);
void init_bramble_guard(Entity& entity);
void step_bramble_guard(Game& game, int slot);

void init_mosquito(Entity& entity);
void step_mosquito(Game& game, int slot);

void init_owl(Entity& entity);
void step_owl(Game& game, int slot);

void init_woodpecker(Entity& entity);
void step_woodpecker(Game& game, int slot);

void init_wasp_nest(Entity& entity);
void step_wasp_nest(Game& game, int slot);
void init_wasp(Entity& entity);
void step_wasp(Game& game, int slot);
void init_forager_goblin(Game& game, Entity& entity);
void step_forager_goblin(Game& game, int slot);
void init_carrion_crow(Entity& entity);
void step_carrion_crow(Game& game, int slot);
