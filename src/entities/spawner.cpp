#include "dispatch.hpp"

#include <array>

namespace {

constexpr std::array<Cell, 4> neighbors{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

bool release_into_neighbor(Game& game, Cell origin, EntityKind kind) {
    for (Cell direction : neighbors) {
        const Cell cell = origin + direction;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0)
            continue;
        spawn_entity(game, kind, cell);
        return true;
    }
    return false;
}

int nearby_kind(const Game& game, Cell origin, EntityKind kind, int radius) {
    int found = 0;
    for (const Entity& entity : game.entities)
        if (entity.kind == kind && distance(entity.cell, origin) <= radius) ++found;
    return found;
}

} // namespace

void init_spawner(Entity& entity) {
    entity.health = entity.max_health = 50;
    entity.impassable = true;
    entity.hard_blocker = true;
    entity.spawn_wait = 120;
    entity.sprite = Sprite::Spawner;
}

void init_den(Entity& entity) {
    entity.health = entity.max_health = 85;
    entity.impassable = true;
    entity.hard_blocker = true;
    entity.spawn_wait = 150;
    entity.sprite = Sprite::Den;
}

void step_spawner(Game& game, int slot) {
    Entity& spawner = game.entities[static_cast<std::size_t>(slot)];
    if (spawner.spawn_wait > 0) { --spawner.spawn_wait; return; }
    const EntityKind kind = (game.run.phase==RunPhase::Arena || forest_floor(game.run.floor)) ? EntityKind::Zombie :
                            (ice_floor(game.run.floor) ? EntityKind::FrostBat : EntityKind::Ember);
    if (nearby_kind(game, spawner.cell, kind, 7) < 4)
        release_into_neighbor(game, spawner.cell, kind);
    spawner.spawn_wait = 120;
}

void step_den(Game& game, int slot) {
    Entity& den = game.entities[static_cast<std::size_t>(slot)];
    if (den.spawn_wait > 0) { --den.spawn_wait; return; }
    if (nearby_kind(game, den.cell, EntityKind::Wolf, 8) < 3 &&
        release_into_neighbor(game, den.cell, EntityKind::Wolf))
        emit_sound(game, SoundId::ZombieGrowl1, den.cell);
    den.spawn_wait = 150;
}
