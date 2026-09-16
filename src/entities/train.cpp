#include "../world/ice_material.hpp"
#include "dispatch.hpp"
#include "../props/interaction.hpp"

void init_rail(Entity& entity) {
    entity.facing = {-1, 0};
    entity.move_interval = 1;
}

void init_train(Entity& entity) {
    entity.sprite = Sprite::TrainHead;
    entity.facing = {-1, 0};
    entity.health = entity.max_health = 10000000;
    entity.move_interval = 1;
    entity.impassable = true;
    entity.hard_blocker = true;
}

void step_rail(Game& game, int slot) {
    Entity& rail = game.entities[static_cast<std::size_t>(slot)];
    const Cell next = rail.cell + rail.facing;
    if (game.stage.in_bounds(next)) {
        hit_prop(game, next, 255, rail.cell);
        damage_tile(game.stage, next, 0, 0, TileImpact::Train);
        release_wall_contents(game,next);
        rail.cell = next;
        if (game.tick % 4 == 0) emit_sound(game, SoundId::RailPlace, next);
        return;
    }

    // TRACK FIRST: The layer finishes every rail before an engine can enter it.
    const Cell origin{game.stage.width - 1, rail.cell.y};
    for (Entity& entity : game.entities)
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::RailLayer &&
            entity.cell == origin)
            crush_entity(game, static_cast<int>(&entity - game.entities.data()), origin);
    const Handle train = spawn_entity(game, EntityKind::Train, origin);
    if (Entity* head = get_entity(game, train)) {
        head->train_origin = origin;
        head->train_cars_left = 5 + static_cast<int>(random_u32(game) % 16);
    }
    remove_entity(game, {slot, rail.generation});
}

void step_train(Game& game, int slot) {
    Entity& train = game.entities[static_cast<std::size_t>(slot)];
    const Cell next = train.cell + train.facing;
    const Tile* tile = game.stage.at(next);
    if (tile == nullptr || tile->kind != TileKind::Rail) {
        remove_entity(game, {slot, train.generation});
        return;
    }
    for (int other_slot = 0; other_slot < max_entities; ++other_slot) {
        if (other_slot == slot) continue;
        Entity& other = game.entities[static_cast<std::size_t>(other_slot)];
        if (other.kind == EntityKind::Train && other.cell == next) return;
        if (other.kind == EntityKind::None || other.cell != next) continue;
        if (other.kind == EntityKind::GroundItem)
            remove_entity(game, {other_slot, other.generation});
        else if (other.kind == EntityKind::Crusher)
            crush_entity(game, other_slot, train.cell);
        else damage_entity(game, other_slot, 1000, train.cell);
    }
    train.cell = next;
    if (game.tick % 8 == 0) emit_sound(game, SoundId::TrainPassing, train.cell);
    if (train.train_cars_left > 0) {
        const Handle car = spawn_entity(game, EntityKind::Train, train.train_origin);
        if (Entity* entity = get_entity(game, car)) {
            entity->sprite = train.train_cars_left == 1 ? Sprite::Caboose : Sprite::TrainCarA;
            entity->train_cars_left = 0;
        }
        --train.train_cars_left;
    }
}
