#include "system.hpp"
#include "gunfire.hpp"
#include "motion.hpp"
#include "../debris/sweep.hpp"
#include "templates.hpp"
#include "water.hpp"
#include "../surfaces/render.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/water.hpp"

#include <algorithm>
#include <cmath>

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
            sound.sound != SoundId::Explosion1 && sound.sound != SoundId::BearSlam &&
            sound.sound != SoundId::BoarHit && sound.sound != SoundId::WolfBite &&
            sound.sound != SoundId::BatBite && sound.sound != SoundId::MimicBite) continue;
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
    case EntityKind::Player: case EntityKind::ZombieStack: case EntityKind::Zombie: case EntityKind::Chicken:
    case EntityKind::BurrowWorm:
    case EntityKind::Wasp: case EntityKind::ForagerGoblin: case EntityKind::CarrionCrow:
    case EntityKind::Mosquito: case EntityKind::Owl: case EntityKind::Woodpecker:
    case EntityKind::Bat: case EntityKind::Wolf: case EntityKind::Dog:
    case EntityKind::Bear: case EntityKind::Boar: case EntityKind::SporeToad:
    case EntityKind::ThornSnail: case EntityKind::LanternMoth:
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
        {
            spawn_death(cosmetics, pose.cell, pose.kind,
                        hit_angle(game, pose.cell, pose.angle), seed);
            scatter_material(cosmetics.debris, pose.cell,
                pose.kind == EntityKind::CarrionCrow || pose.kind == EntityKind::Chicken || pose.kind == EntityKind::Owl || pose.kind == EntityKind::Woodpecker ?
                    DebrisKind::Feather : (pose.kind == EntityKind::Mosquito || pose.kind == EntityKind::Wasp) ? DebrisKind::SeedHusk : DebrisKind::BoneChip,
                3, seed);
        }
        if (pose.seen && pose.health > 0 &&
            (pose.kind == EntityKind::WaspNest || pose.kind == EntityKind::RootTurret || pose.kind == EntityKind::BrambleGuard)) {
            scatter_material(cosmetics.debris, pose.cell, DebrisKind::WoodChip, 5, seed);
            scatter_material(cosmetics.debris, pose.cell, DebrisKind::OakLeaf, 3, seed+1);
        }
        pose = {};
        return;
    }
    if (same && entity.cell != pose.cell) {
        if (entity.kind != EntityKind::GroundItem && entity.kind != EntityKind::RailLayer)
            push_debris(cosmetics.debris, entity.cell,
                entity.kind == EntityKind::Train ? 2.2F : 1.1F,
                entity.kind == EntityKind::Train ? .18F : .035F, entity.cell - pose.cell);
        ++pose.steps;
        if (surface_wet(game.stage.at_or_border(entity.cell)) && wading_actor(entity))
            spawn_water_rings(cosmetics, entity.cell, true);
        else if (entity.kind == EntityKind::Player || entity.kind == EntityKind::Zombie ||
            entity.kind == EntityKind::ZombieStack)
            spawn_footprint(cosmetics, entity.cell, entity.kind,
                            (pose.steps & 1U) != 0, seed);
        pose.angle = static_cast<float>(visual_bits(seed) % 31U) - 15.0F;
    }
    if (same && (entity.attack_wait > pose.attack_wait ||
                 entity.use_flash > pose.use_flash)) {
        pose.angle = attack_angle(entity.facing);
        if ((entity.kind == EntityKind::Zombie || entity.kind == EntityKind::ZombieStack) &&
            entity.attack_wait > pose.attack_wait)
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
    if (entity.kind == EntityKind::Player) step_camera_guide(pose, entity, same);
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
    switch (sound.sound) {
    case SoundId::BottleBreak:
        scatter_material(cosmetics.debris, sound.cell, DebrisKind::Pottery, 4, seed);
        break;
    case SoundId::ArrowImpact:
        scatter_material(cosmetics.debris, sound.cell, DebrisKind::WoodChip, 2, seed);
        scatter_material(cosmetics.debris, sound.cell, DebrisKind::Feather, 1, seed ^ 951U);
        break;
    case SoundId::GraveRise:
        spawn_debris(cosmetics, sound.cell, seed);
        break;
    case SoundId::ZombieTopple:
        spawn_death(cosmetics, sound.cell, EntityKind::Zombie, 85.0F, seed);
        push_debris(cosmetics.debris, sound.cell, 1.6F, .07F);
        break;
    case SoundId::Explosion: case SoundId::Explosion1:
    case SoundId::Explosion2: case SoundId::Explosion3:
        push_debris(cosmetics.debris, sound.cell, 4.0F, .24F);
        cosmetics.flashes.push_back({{sound.cell, 8, 1.45F,
                                      {1.0F, 0.51F, 0.20F}}, 18, 18});
        break;
    case SoundId::SmallLaser:
        cosmetics.flashes.push_back({{sound.cell, 4, 0.80F,
                                      {1.0F, 0.82F, 0.46F}}, 5, 5});
        break;
    case SoundId::HitBlock1: case SoundId::SturdyBlockBouncedOn:
        cosmetics.flashes.push_back({{sound.cell, 3, 0.58F,
                                      {0.78F, 0.88F, 1.0F}}, 4, 4});
        break;
    default: break;
    }
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
    if (!cosmetics.camera_ready ||
        std::abs(cosmetics.camera.x - static_cast<float>(focus.x)) > 12.0F ||
        std::abs(cosmetics.camera.y - static_cast<float>(focus.y)) > 12.0F) {
        cosmetics.camera = focus;
        cosmetics.camera_ready = true;
    }
    step_particles(cosmetics);
    const bool fresh_debris = !cosmetics.debris.ready;
    prepare_debris(cosmetics.debris, game.stage);
    step_debris(cosmetics.debris, game.stage, game.tick, game.run.floor <= 4);
    for (int slot = 0; slot < max_entities; ++slot)
        observe_entity(cosmetics, game, slot);
    for (int index = 0; index < game.sound_count; ++index)
        observe_sound(cosmetics, game.sounds[static_cast<std::size_t>(index)], focus);
    // TERRAIN: A replayed tile hit must not emit its fragments a second time.
    for (int index = 0; index < game.impact_count; ++index) {
        const auto key = (1ULL << 63) | (game.tick << 8) |
                         (static_cast<std::uint64_t>(index) + 1);
        if (std::find(cosmetics.seen_events.begin(), cosmetics.seen_events.end(), key) !=
            cosmetics.seen_events.end()) continue;
        cosmetics.seen_events[cosmetics.next_event++ % cosmetics.seen_events.size()] = key;
        const ImpactEvent& impact = game.impacts[static_cast<std::size_t>(index)];
        if (distance(impact.cell, focus) <= 18)
            spawn_terrain_impact(cosmetics, impact, key);
        if (impact.prop != PropKind::None && !fresh_debris)
            scatter_prop_debris(cosmetics.debris, impact.cell, impact.prop, key);
        else if (impact.prop == PropKind::None && impact.damage > 0)
            scatter_material(cosmetics.debris, impact.cell,
                impact.material == Sprite::ForestTree ? DebrisKind::Bark :
                impact.material == Sprite::ForestTimber ? DebrisKind::WoodChip : DebrisKind::StoneChip,
                             impact.broken ? 6 : 2, key);
    }
    if (game.tick % 6 == 0)
        for (std::size_t slot = 0; slot < game.entities.size(); ++slot) {
            const Entity& entity = game.entities[slot];
            if (entity.kind == EntityKind::None || entity.health <= 0 ||
                distance(entity.cell, focus) > 18) continue;
            const bool fire = entity.kind == EntityKind::Campfire && entity.fire_tramples < 5;
            const bool burning = entity.scorch_ticks > 0 || entity.burn_ticks > 0;
            if (fire || burning) {
                spawn_flame(cosmetics, entity.cell, game.tick + slot * 17U, burning);
                if (game.tick % 18 == 0)
                    spawn_campfire_smoke(cosmetics, entity.cell, game.tick + slot * 17U);
            }
        }
    observe_raking(cosmetics, game);
    observe_gunfire(cosmetics, game, focus);
    observe_water(cosmetics, game, focus);
    observe_surfaces(cosmetics, game, focus);
    if (game.run.phase == RunPhase::Arena || (game.run.floor > 4 && game.run.floor <= 8))
        spawn_weather_cloud(cosmetics, focus, game.tick ^ 0x752ac012U, zoom);
}

ViewCamera camera_for(const Cosmetics& cosmetics, const Game& game, int owner) {
    if (owner >= 0 && owner < 4) {
        const Handle handle = game.players[static_cast<std::size_t>(owner)];
        if (const Entity* player = get_entity(game, handle)) {
            const EntityPose& pose = cosmetics.poses[static_cast<std::size_t>(handle.slot)];
            if (pose.seen && pose.camera_guide_ready && pose.generation == player->generation)
                return camera_guide_position(pose, cosmetics.frame_alpha);
        }
    }
    if (cosmetics.camera_ready) return cosmetics.camera;
    if (owner >= 0 && owner < 4)
        if (const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]))
            return player->cell;
    return {32.0F, 32.0F};
}
