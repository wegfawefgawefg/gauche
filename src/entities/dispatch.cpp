#include "dispatch.hpp"
#include "pocket_door.hpp"
#include "rime_skater.hpp"
#include "../projectiles/projectile.hpp"
#include "../world/encounter.hpp"

void init_entity(Game& game, Entity& entity) {
    // Each kind owns its setup beside its step. Dispatch stays deliberately plain.
    switch (entity.kind) {
    case EntityKind::RimeSkater: init_rime_skater(entity); break;
    case EntityKind::PocketDoor: init_pocket_door(entity); break;
    case EntityKind::WaspNest: init_wasp_nest(entity); break;
    case EntityKind::Wasp: init_wasp(entity); break;
    case EntityKind::ForagerGoblin: init_forager_goblin(game, entity); break;
    case EntityKind::CarrionCrow: init_carrion_crow(entity); break;
    case EntityKind::BurrowWorm: init_burrow_worm(entity); break;
    case EntityKind::Projectile: init_projectile(entity); break;
    case EntityKind::Boar: init_boar(entity); break;
    case EntityKind::ThornSnail: init_thorn_snail(entity); break;
    case EntityKind::LanternMoth: init_lantern_moth(entity); break;
    case EntityKind::Mosquito: init_mosquito(entity); break;
    case EntityKind::Owl: init_owl(entity); break;
    case EntityKind::Woodpecker: init_woodpecker(entity); break;
    case EntityKind::RootTurret: init_root_turret(entity); break;
    case EntityKind::BrambleGuard: init_bramble_guard(entity); break;
    case EntityKind::SporeToad: init_spore_toad(entity); break;
    case EntityKind::CrateMimic: init_crate_mimic(entity); break;
    case EntityKind::Encounter: init_encounter(entity); break;
    case EntityKind::Player: init_player(entity); break;
    case EntityKind::ZombieStack: init_zombie_stack(game, entity); break;
    case EntityKind::Zombie: init_zombie(game, entity); break;
    case EntityKind::Chicken: init_chicken(game, entity); break;
    case EntityKind::Bat: case EntityKind::FrostBat: init_bat(entity); break;
    case EntityKind::Wolf: init_wolf(entity); break;
    case EntityKind::Dog: init_dog(entity); break;
    case EntityKind::Bear: init_bear(entity); break;
    case EntityKind::Bunny: init_bunny(entity); break;
    case EntityKind::Ember: init_ember(entity); break;
    case EntityKind::Spawner: init_spawner(entity); break;
    case EntityKind::Den: init_den(entity); break;
    case EntityKind::RailLayer: init_rail(entity); break;
    case EntityKind::Train: init_train(entity); break;
    case EntityKind::Key: case EntityKind::Door: case EntityKind::Exit:
    case EntityKind::Trap: case EntityKind::Switch: case EntityKind::Campfire:
    case EntityKind::Coins: case EntityKind::WaveVent: case EntityKind::EncounterGate:
    case EntityKind::Crusher: init_fixture(entity); break;
    case EntityKind::GroundItem: case EntityKind::None: case EntityKind::Count: break;
    }
}

void step_entity(Game& game, int slot) {
    switch (game.entities[static_cast<std::size_t>(slot)].kind) {
    case EntityKind::RimeSkater: step_rime_skater(game, slot); break;
    case EntityKind::WaspNest: step_wasp_nest(game, slot); break;
    case EntityKind::Wasp: step_wasp(game, slot); break;
    case EntityKind::ForagerGoblin: step_forager_goblin(game, slot); break;
    case EntityKind::CarrionCrow: step_carrion_crow(game, slot); break;
    case EntityKind::BurrowWorm: step_burrow_worm(game, slot); break;
    case EntityKind::Projectile: step_projectile(game, slot); break;
    case EntityKind::Boar: step_boar(game, slot); break;
    case EntityKind::ThornSnail: step_thorn_snail(game, slot); break;
    case EntityKind::LanternMoth: step_lantern_moth(game, slot); break;
    case EntityKind::Mosquito: step_mosquito(game, slot); break;
    case EntityKind::Owl: step_owl(game, slot); break;
    case EntityKind::Woodpecker: step_woodpecker(game, slot); break;
    case EntityKind::RootTurret: step_root_turret(game, slot); break;
    case EntityKind::BrambleGuard: step_bramble_guard(game, slot); break;
    case EntityKind::SporeToad: step_spore_toad(game, slot); break;
    case EntityKind::CrateMimic: step_crate_mimic(game, slot); break;
    case EntityKind::Encounter: step_encounter(game, slot); break;
    case EntityKind::ZombieStack: step_zombie_stack(game, slot); break;
    case EntityKind::Zombie: step_zombie(game, slot); break;
    case EntityKind::Chicken: step_chicken(game, slot); break;
    case EntityKind::Bat: case EntityKind::FrostBat: step_bat(game, slot); break;
    case EntityKind::Wolf: step_wolf(game, slot); break;
    case EntityKind::Dog: step_dog(game, slot); break;
    case EntityKind::Bear: step_bear(game, slot); break;
    case EntityKind::Bunny: step_bunny(game, slot); break;
    case EntityKind::Ember: step_ember(game, slot); break;
    case EntityKind::Spawner: step_spawner(game, slot); break;
    case EntityKind::Den: step_den(game, slot); break;
    case EntityKind::RailLayer: step_rail(game, slot); break;
    case EntityKind::Train: step_train(game, slot); break;
    default: break;
    }
}
