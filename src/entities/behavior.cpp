#include "../props/interaction.hpp"
#include "behavior.hpp"
#include "../props/scarecrow.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace {

constexpr std::array<Cell, 4> neighbors{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

bool open_neighbor(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile != nullptr && walkable(*tile) && entity_at(game, cell, true) < 0;
}

std::uint64_t sound_roll(std::uint64_t value) {
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

} // namespace

int nearest_player(const Game& game, Cell from, int radius) {
    int nearest = -1;
    int best = radius + 1;
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        if (!game.run.online[owner]) continue;
        const Handle handle = game.players[owner];
        const Entity* player = get_entity(game, handle);
        if (player == nullptr || player->health <= 0) continue;
        const int length = distance(from, player->cell);
        if (length < best && !smoke_hides(game.stage, from, player->cell)) { nearest = handle.slot; best = length; }
    }
    return nearest;
}

bool willing_step(Game& game, int slot, Cell destination) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (!scarecrow_allows_step(game, actor, destination)) {
        actor.move_wait = std::max(1, actor.move_interval);
        return false;
    }
    return move_entity(game, slot, destination);
}

void wander(Game& game, int slot) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0) return;
    // WANDER: Pick among free neighbors before spending the movement beat.
    std::array<Cell, 4> choices{};
    std::uint32_t count = 0;
    for (Cell side : neighbors)
        if (open_neighbor(game, entity.cell + side)) choices[count++] = entity.cell + side;
    if (count == 0) { entity.move_wait = std::max(1, entity.move_interval); return; }
    const std::uint32_t choice = random_u32(game) % (count + 1);
    if (choice == count) entity.move_wait = std::max(1, entity.move_interval / 2);
    else willing_step(game, slot, choices[choice]);
}

void approach(Game& game, int slot, Cell target) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.move_wait > 0 || entity.cell == target) return;
    const Cell difference = target - entity.cell;
    const Cell first = std::abs(difference.x) >= std::abs(difference.y) ?
                       Cell{difference.x > 0 ? 1 : -1, 0} :
                       Cell{0, difference.y > 0 ? 1 : -1};
    const Cell second = first.x != 0 ?
                        Cell{0, difference.y > 0 ? 1 : -1} :
                        Cell{difference.x > 0 ? 1 : -1, 0};
    // DETOUR: Failed move_entity calls set move_wait; never call one just to probe.
    const Cell choices[]{first, second, {-second.x, -second.y}, {-first.x, -first.y}};
    for (Cell side : choices) {
        if (!open_neighbor(game, entity.cell + side)) continue;
        willing_step(game, slot, entity.cell + side);
        return;
    }
    entity.move_wait = std::max(1, entity.move_interval);
}

void bite(Game& game, int slot, int damage, int range) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    if (enemy.attack_wait > 0) return;
    const auto chosen = enemy_target(game, enemy.cell, range);
    if (!chosen) return;
    if (chosen->actor.slot < 0) {
        enemy.facing = cardinal_toward(enemy.cell, chosen->cell, enemy.facing);
        hit_prop(game, chosen->cell, damage, enemy.cell);
        enemy.attack_wait = enemy.attack_interval;
        emit_sound(game, SoundId::ZombieScratch1, enemy.cell);
        return;
    }
    const int target_slot = chosen->actor.slot;
    Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    const Cell delta = target.cell - enemy.cell;
    enemy.facing = std::abs(delta.x) > std::abs(delta.y) ?
                   Cell{delta.x > 0 ? 1 : -1, 0} :
                   Cell{0, delta.y > 0 ? 1 : -1};
    const int prior_health = target.health;
    damage_entity(game, target_slot, damage, enemy.cell);
    if (target.health < prior_health && target.health > 0) {
        if (enemy.kind == EntityKind::FrostBat)
            target.freeze_ticks = std::max(target.freeze_ticks, 90);
        if (enemy.kind == EntityKind::Bear)
            apply_stun(target, 20);
    }
    enemy.attack_wait = enemy.attack_interval;
    emit_sound(game, SoundId::ZombieScratch1, enemy.cell);
}

void maybe_growl(Game& game, int slot, SoundId sound) {
    const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    // A local sound roll must not advance the gameplay RNG or change a replay.
    const std::uint64_t seed = game.tick ^
        (static_cast<std::uint64_t>(slot) << 32) ^
        (static_cast<std::uint64_t>(entity.generation) * 0x9e3779b97f4a7c15ULL);
    if (sound_roll(seed) % 10000U == 0)
        emit_sound(game, sound, entity.cell);
}
