#include "game.hpp"

#include <array>
#include <cstdlib>

namespace {

constexpr std::array<Cell, 4> neighbors{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

int nearest_player(const Game& game, Cell from, int radius) {
    int nearest = -1;
    int best = radius + 1;
    for (Handle handle : game.players) {
        const Entity* player = get_entity(game, handle);
        if (player == nullptr || player->health <= 0) continue;
        const int length = distance(from, player->cell);
        if (length < best) { nearest = handle.slot; best = length; }
    }
    return nearest;
}

void wander(Game& game, int slot) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0) return;
    const std::uint32_t choice = random_u32(game) % 5;
    if (choice < 4) move_entity(game, slot, entity.cell + neighbors[choice]);
}

void approach(Game& game, int slot, Cell target) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0) return;
    const Cell difference = target - entity.cell;
    const Cell first = std::abs(difference.x) >= std::abs(difference.y) ?
                       Cell{difference.x > 0 ? 1 : -1, 0} :
                       Cell{0, difference.y > 0 ? 1 : -1};
    const Cell second = first.x != 0 ?
                        Cell{0, difference.y > 0 ? 1 : -1} :
                        Cell{difference.x > 0 ? 1 : -1, 0};
    if (!move_entity(game, slot, entity.cell + first) &&
        !move_entity(game, slot, entity.cell + second)) wander(game, slot);
}

void bite(Game& game, int slot, int damage, int range = 1) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    if (enemy.attack_wait > 0) return;
    const int target_slot = nearest_player(game, enemy.cell, range);
    if (target_slot < 0) return;
    Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    const Cell delta = target.cell - enemy.cell;
    enemy.facing = std::abs(delta.x) > std::abs(delta.y) ?
                   Cell{delta.x > 0 ? 1 : -1, 0} :
                   Cell{0, delta.y > 0 ? 1 : -1};
    damage_entity(game, target_slot, damage, enemy.cell);
    enemy.attack_wait = enemy.attack_interval;
    emit_sound(game, SoundId::ZombieScratch1, enemy.cell);
}

void step_zombie(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    wander(game, slot);
    zombie.sprite = Sprite::Zombie;
    bite(game, slot, 5);
    if (zombie.attack_wait == zombie.attack_interval)
        zombie.sprite = Sprite::ZombieScratch1;
}

void step_hunter(Game& game, int slot) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    const int target_slot = nearest_player(game, enemy.cell, 9);
    if (target_slot < 0) wander(game, slot);
    else {
        const Cell target = game.entities[static_cast<std::size_t>(target_slot)].cell;
        if (distance(enemy.cell, target) > 1) approach(game, slot, target);
    }
    const int damage = enemy.kind == EntityKind::Bear ? 24 :
                       (enemy.kind == EntityKind::Wolf ? 11 : 7);
    bite(game, slot, damage);
}

void step_ember(Game& game, int slot) {
    Entity& ember = game.entities[static_cast<std::size_t>(slot)];
    const int target_slot = nearest_player(game, ember.cell, 8);
    if (target_slot < 0) { wander(game, slot); return; }
    const Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    if (ember.attack_wait == 0) {
        const Cell difference = target.cell - ember.cell;
        if (difference.x == 0 || difference.y == 0) {
            ember.facing = difference.x == 0 ?
                           Cell{0, difference.y > 0 ? 1 : -1} :
                           Cell{difference.x > 0 ? 1 : -1, 0};
            use_held_item(game, slot, target.cell);
            ember.attack_wait = ember.attack_interval;
        }
    }
    if (distance(ember.cell, target.cell) > 4) approach(game, slot, target.cell);
}

void step_spawner(Game& game, int slot) {
    Entity& spawner = game.entities[static_cast<std::size_t>(slot)];
    if (spawner.spawn_wait > 0) { --spawner.spawn_wait; return; }
    const EntityKind spawn = game.run.floor <= 4 ? EntityKind::Zombie :
                             (game.run.floor <= 8 ? EntityKind::Ember : EntityKind::FrostBat);
    int nearby = 0;
    for (const Entity& entity : game.entities)
        if (entity.kind == spawn && distance(entity.cell, spawner.cell) < 8) ++nearby;
    if (nearby < 4) {
        for (Cell direction : neighbors) {
            const Cell cell = spawner.cell + direction;
            const Tile* tile = game.stage.at(cell);
            if (tile != nullptr && walkable(tile->kind) && entity_at(game, cell, true) < 0) {
                spawn_entity(game, spawn, cell);
                break;
            }
        }
    }
    spawner.spawn_wait = 120;
}

} // namespace

void step_enemy(Game& game, int slot) {
    switch (game.entities[static_cast<std::size_t>(slot)].kind) {
    case EntityKind::Zombie: step_zombie(game, slot); break;
    case EntityKind::Chicken: case EntityKind::Bunny: wander(game, slot); break;
    case EntityKind::Bat: case EntityKind::Wolf: case EntityKind::Bear:
    case EntityKind::FrostBat: step_hunter(game, slot); break;
    case EntityKind::Ember: step_ember(game, slot); break;
    case EntityKind::Spawner: step_spawner(game, slot); break;
    default: break;
    }
}
