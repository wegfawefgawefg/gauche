#include "items/sled.hpp"
#include "game.hpp"
#include "entities/icicle_spider.hpp"
#include "entities/echo_hound.hpp"
#include "world/water.hpp"
#include "surfaces/interaction.hpp"
#include "surfaces/slip.hpp"
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
    clear_sled_links(game,*entity);
    clear_spider_strand(game,*entity);
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

bool move_entity(Game& game, int slot, Cell destination, bool allow_slip) {
    if (slot < 0 || slot >= max_entities) return false;
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile = game.stage.at(destination);
    if (entity.kind == EntityKind::None || entity.vitals.rooted > 0) return false;
    const int occupant = entity_at(game, destination, true);
    if (tile == nullptr || !walkable(*tile) || (occupant >= 0 && occupant != slot)) {
        // A blocked step still takes its beat, as it did in the Rust arena.
        entity.move_wait = entity.move_interval;
        return false;
    }
    entity.vitals.slide_momentum = 0;
    const Cell direction = destination - entity.cell;
    entity.facing = direction;
    entity.cell = destination;
    enter_actor_cell(game, slot);
    if (entity.kind==EntityKind::Player && entity.cell==destination) board_sled(game,slot);
    if (allow_slip && entity.cell == destination && !ridden_sled(game,entity) && slip_on_surface(game, slot, direction)) return true;
    entity.move_wait = entity.move_interval;
    // LANDING: A spring can move us again during contact; effects use the final cell.
    if (entity.health <= 0) return true;
    tile = game.stage.at(entity.cell);
    if (tile == nullptr) return true;
    if (wading_actor(entity)) entity.move_wait += surface_step_delay(*tile);
    // FOOTFALLS: Snow muffles steps; water carries them. Hounds ignore their own pack.
    if (wading_actor(entity) && entity.kind != EntityKind::EchoHound)
        hear_echo_hounds(game, entity.cell, surface_wet(*tile) ? 6 : tile->kind == TileKind::Snow ? 2 : 4);
    if (tile->kind==TileKind::Bridge && wading_actor(entity))
        emit_sound(game,SoundId::BridgeStep,entity.cell);
    else if (surface_wet(*tile) && wading_actor(entity))
        emit_sound(game, ((entity.cell.x + entity.cell.y + slot) & 1) == 0 ?
            SoundId::WaterStep1 : SoundId::WaterStep2, entity.cell);
    else if (wading_actor(entity) && entity.kind != EntityKind::RimeSkater &&
             (tile->kind == TileKind::Snow || tile->kind == TileKind::Ice)) {
        const bool alternate = ((entity.cell.x + entity.cell.y + slot) & 1) != 0;
        emit_sound(game, tile->kind == TileKind::Snow ?
            (alternate ? SoundId::SnowStep2 : SoundId::SnowStep1) :
            (alternate ? SoundId::IceStep2 : SoundId::IceStep1), entity.cell);
    } else if (entity.kind == EntityKind::Player || entity.kind == EntityKind::Zombie ||
        entity.kind == EntityKind::Chicken || entity.kind == EntityKind::ZombieStack)
        emit_sound(game, ((entity.cell.x + entity.cell.y + slot) & 1) == 0 ?
                   SoundId::Step1 : SoundId::Step2,
                   entity.cell);
    return true;
}
