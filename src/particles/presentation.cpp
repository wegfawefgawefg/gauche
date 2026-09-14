#include "system.hpp"
#include "templates.hpp"

#include <algorithm>

namespace {

std::uint32_t visual_bits(std::uint64_t value) {
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    return static_cast<std::uint32_t>(value ^ (value >> 31));
}

float hit_angle(const Game& game, Cell target, float fallback) {
    int closest = 9;
    Cell source{};
    for (int index = 0; index < game.sound_count; ++index) {
        const SoundEvent& sound = game.sounds[static_cast<std::size_t>(index)];
        const int length = distance(target, sound.cell);
        if (length == 0 || length >= closest) continue;
        if (sound.sound != SoundId::ZombieScratch1 && sound.sound != SoundId::Punch1 &&
            sound.sound != SoundId::SmallLaser && sound.sound != SoundId::Explosion &&
            sound.sound != SoundId::Explosion1) continue;
        closest = length;
        source = sound.cell;
    }
    if (closest == 9) return fallback;
    if (source.x < target.x) return 45.0F;
    if (source.x > target.x) return -45.0F;
    return source.y < target.y ? 180.0F : 0.0F;
}

bool bleeds(EntityKind kind) {
    switch (kind) {
    case EntityKind::Player: case EntityKind::Zombie: case EntityKind::Chicken:
    case EntityKind::Bat: case EntityKind::Wolf: case EntityKind::Dog:
    case EntityKind::Bear:
    case EntityKind::Bunny: case EntityKind::Ember: case EntityKind::FrostBat:
        return true;
    default: return false;
    }
}

float attack_angle(Cell facing) {
    if (facing.x > 0) return 45.0F;
    if (facing.x < 0) return -45.0F;
    return facing.y < 0 ? 180.0F : 0.0F;
}

void observe_entity(Cosmetics& cosmetics, const Game& game, int slot) {
    const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    EntityPose& pose = cosmetics.poses[static_cast<std::size_t>(slot)];
    const std::uint64_t seed = (game.tick << 16) ^ static_cast<std::uint64_t>(slot);
    const bool same = pose.seen && pose.generation == entity.generation &&
                      pose.kind == entity.kind;
    if (entity.kind == EntityKind::None) {
        // DEATH: The sweep removes an enemy in the same tick as its death sound.
        if (pose.seen && pose.health > 0 && bleeds(pose.kind))
            spawn_death(cosmetics, pose.cell, pose.kind,
                        hit_angle(game, pose.cell, pose.angle), seed);
        pose = {};
        return;
    }
    if (same && entity.cell != pose.cell) {
        ++pose.steps;
        if (entity.kind == EntityKind::Player || entity.kind == EntityKind::Zombie)
            spawn_footprint(cosmetics, entity.cell, entity.kind,
                            (pose.steps & 1U) != 0, seed);
        pose.angle = static_cast<float>(visual_bits(seed) % 31U) - 15.0F;
    }
    if (same && (entity.attack_wait > pose.attack_wait ||
                 entity.use_flash > pose.use_flash)) {
        pose.angle = attack_angle(entity.facing);
        if (entity.kind == EntityKind::Zombie && entity.attack_wait > pose.attack_wait)
            spawn_zombie_scratch(cosmetics, entity.cell, entity.facing, seed);
    }
    if (same && entity.health < pose.health && entity.health >= 0) {
        if (bleeds(entity.kind))
            spawn_hit(cosmetics, entity.cell, seed, pose.health - entity.health);
        else spawn_debris(cosmetics, entity.cell, seed);
        pose.shake = std::min(0.6F, pose.shake + 0.16F);
        pose.angle = hit_angle(game, entity.cell, pose.angle);
        if (entity.health == 0 && entity.kind == EntityKind::Player)
            spawn_death(cosmetics, entity.cell, EntityKind::None, pose.angle, seed);
    }
    if (!same) pose = {};
    pose.seen = true;
    pose.generation = entity.generation;
    pose.kind = entity.kind;
    pose.cell = entity.cell;
    pose.health = entity.health;
    pose.attack_wait = entity.attack_wait;
    pose.use_flash = entity.use_flash;
    if (entity.facing.x != 0) pose.horizontal_flip = entity.facing.x < 0;
    pose.shake = std::max(0.0F, pose.shake - 0.01F);
}

bool already_seen(Cosmetics& cosmetics, const SoundEvent& sound) {
    const std::uint64_t key = (sound.tick << 8) |
        (static_cast<std::uint64_t>(sound.sequence) + 1);
    if (std::find(cosmetics.seen_events.begin(), cosmetics.seen_events.end(), key) !=
        cosmetics.seen_events.end()) return true;
    cosmetics.seen_events[cosmetics.next_event++ % cosmetics.seen_events.size()] = key;
    return false;
}

void observe_sound(Cosmetics& cosmetics, const SoundEvent& sound, Cell focus) {
    if (already_seen(cosmetics, sound) || distance(sound.cell, focus) > 18) return;
    const std::uint64_t seed = (sound.tick << 8) | sound.sequence;
    spawn_sound_effect(cosmetics, sound, seed);
    if (sound.sound != SoundId::ZombieGrowl1 && sound.sound != SoundId::ZombieGrowl2 &&
        sound.sound != SoundId::Chick && sound.sound != SoundId::Hen &&
        sound.sound != SoundId::Rooster) return;
    for (EntityPose& pose : cosmetics.poses)
        if (pose.seen && pose.cell == sound.cell &&
            (pose.kind == EntityKind::Zombie || pose.kind == EntityKind::Chicken))
            pose.shake = std::max(pose.shake, 0.4F);
}

} // namespace

void update_cosmetics(Cosmetics& cosmetics, const Game& game, Cell focus, float zoom) {
    if (game.tick < cosmetics.last_tick || game.run.floor != cosmetics.last_floor ||
        (game.run.phase == RunPhase::Arena && cosmetics.last_phase != RunPhase::Arena))
        cosmetics = {};
    cosmetics.last_tick = game.tick;
    cosmetics.last_floor = game.run.floor;
    cosmetics.last_phase = game.run.phase;
    step_particles(cosmetics);
    for (int slot = 0; slot < max_entities; ++slot)
        observe_entity(cosmetics, game, slot);
    for (int index = 0; index < game.sound_count; ++index)
        observe_sound(cosmetics, game.sounds[static_cast<std::size_t>(index)], focus);
    if (game.tick % 12 == 0)
        for (std::size_t slot = 0; slot < game.entities.size(); ++slot) {
            const Entity& entity = game.entities[slot];
            if (entity.kind == EntityKind::Campfire && distance(entity.cell, focus) <= 12)
                spawn_campfire_smoke(cosmetics, entity.cell, game.tick + slot * 17U);
        }
    if (game.run.phase == RunPhase::Arena || game.run.floor <= 4)
        spawn_weather_cloud(cosmetics, focus, game.tick ^ 0x752ac012U, zoom);
}
