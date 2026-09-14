#include "behavior.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace {

constexpr std::array<Cell, 4> neighbors{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

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

void bite(Game& game, int slot, int damage, int range) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    if (enemy.attack_wait > 0) return;
    const int target_slot = nearest_player(game, enemy.cell, range);
    if (target_slot < 0) return;
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
            target.stun_ticks = std::max(target.stun_ticks, 20);
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
