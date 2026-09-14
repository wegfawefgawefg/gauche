#include "game.hpp"
#include "entities/dispatch.hpp"

#include <cstdint>

std::uint32_t random_u32(Game& game) {
    game.rng ^= game.rng >> 12;
    game.rng ^= game.rng << 25;
    game.rng ^= game.rng >> 27;
    return static_cast<std::uint32_t>((game.rng * 2685821657736338717ULL) >> 32);
}

Handle spawn_entity(Game& game, EntityKind kind, Cell cell) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None) continue;
        const std::uint32_t generation = entity.generation + 1;
        entity = {};
        entity.generation = generation;
        entity.kind = kind;
        entity.cell = cell;
        entity.birth_tick = game.tick;
        init_entity(game, entity);
        return {slot, generation};
    }
    return {};
}

Entity* get_entity(Game& game, Handle handle) {
    if (handle.slot < 0 || handle.slot >= max_entities) return nullptr;
    Entity& entity = game.entities[static_cast<std::size_t>(handle.slot)];
    if (entity.kind == EntityKind::None || entity.generation != handle.generation) return nullptr;
    return &entity;
}

const Entity* get_entity(const Game& game, Handle handle) {
    if (handle.slot < 0 || handle.slot >= max_entities) return nullptr;
    const Entity& entity = game.entities[static_cast<std::size_t>(handle.slot)];
    if (entity.kind == EntityKind::None || entity.generation != handle.generation) return nullptr;
    return &entity;
}

void remove_entity(Game& game, Handle handle) {
    Entity* entity = get_entity(game, handle);
    if (entity == nullptr) return;
    const std::uint32_t generation = entity->generation;
    *entity = {};
    entity->generation = generation;
}

int entity_at(const Game& game, Cell cell, bool impassable_only) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None && entity.cell == cell &&
            (!impassable_only || entity.impassable)) return slot;
    }
    return -1;
}

bool move_entity(Game& game, int slot, Cell destination) {
    if (slot < 0 || slot >= max_entities) return false;
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile = game.stage.at(destination);
    if (entity.kind == EntityKind::None) return false;
    const int occupant = entity_at(game, destination, true);
    if (tile == nullptr || !walkable(tile->kind) || (occupant >= 0 && occupant != slot)) {
        // A blocked step still takes its beat, as it did in the Rust arena.
        entity.move_wait = entity.move_interval;
        if (entity.kind == EntityKind::Player)
            emit_sound(game, SoundId::HitBlock1, entity.cell);
        return false;
    }
    entity.facing = destination - entity.cell;
    entity.cell = destination;
    enter_actor_cell(game, slot);
    entity.move_wait = entity.move_interval;
    if (tile->kind == TileKind::Ice) entity.move_wait += 5;
    if (entity.kind == EntityKind::Player || entity.kind == EntityKind::Zombie ||
        entity.kind == EntityKind::Chicken)
        emit_sound(game, ((destination.x + destination.y + slot) & 1) == 0 ?
                   SoundId::Step1 : SoundId::Step2,
                   destination);
    return true;
}
