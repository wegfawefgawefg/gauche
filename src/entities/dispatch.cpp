#include "dispatch.hpp"

void init_entity(Game& game, Entity& entity) {
    // Each kind owns its setup beside its step. Dispatch stays deliberately plain.
    switch (entity.kind) {
    case EntityKind::Player: init_player(entity); break;
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
    case EntityKind::Crusher: init_fixture(entity); break;
    case EntityKind::GroundItem: case EntityKind::None: break;
    }
}

void step_entity(Game& game, int slot) {
    switch (game.entities[static_cast<std::size_t>(slot)].kind) {
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
