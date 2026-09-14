#include "templates.hpp"
#include "../view.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

std::uint32_t bits(std::uint64_t value) {
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    return static_cast<std::uint32_t>(value ^ (value >> 31));
}

float unit(std::uint32_t value) {
    return static_cast<float>(value & 65535U) / 65535.0F;
}

void add(Cosmetics& cosmetics, SpriteParticle particle) {
    if (cosmetics.sprites.size() >= 2048) cosmetics.sprites.erase(cosmetics.sprites.begin());
    cosmetics.sprites.push_back(particle);
}

void spray(Cosmetics& cosmetics, Cell cell, std::uint64_t seed,
           int count, Sprite sprite, float speed, float gravity) {
    for (int index = 0; index < count; ++index) {
        const std::uint32_t roll = bits(seed + static_cast<std::uint64_t>(index));
        SpriteParticle particle;
        particle.sprite = sprite;
        particle.layer = ParticleLayer::Foreground;
        particle.motion = ParticleMotion::Accelerate;
        particle.x = static_cast<float>(cell.x) + 0.5F;
        particle.y = static_cast<float>(cell.y) + 0.5F;
        particle.vx = (unit(roll) * 2.0F - 1.0F) * speed;
        particle.vy = -(0.35F + unit(roll >> 8)) * speed;
        particle.ay = gravity;
        particle.width = particle.height = 0.14F + unit(roll >> 16) * 0.28F;
        particle.angle = unit(roll >> 4) * 360.0F;
        particle.spin = (unit(roll >> 11) - 0.5F) * 24.0F;
        particle.life = particle.span = 5 + static_cast<int>((roll >> 24) % 14U);
        add(cosmetics, particle);
    }
}

void puddle(Cosmetics& cosmetics, Cell cell, std::uint64_t seed, int count) {
    for (int index = 0; index < count; ++index) {
        const std::uint32_t roll = bits(seed + static_cast<std::uint64_t>(index));
        SpriteParticle particle;
        particle.sprite = (roll & 3U) == 0 ? Sprite::BloodMedium : Sprite::BloodSmall;
        particle.layer = ParticleLayer::Ground;
        particle.x = static_cast<float>(cell.x) + 0.5F + (unit(roll) - 0.5F) * 1.2F;
        particle.y = static_cast<float>(cell.y) + 0.85F + (unit(roll >> 8) - 0.5F) * 0.4F;
        particle.width = particle.height = 0.13F + unit(roll >> 16) * 0.25F;
        particle.angle = unit(roll >> 5) * 360.0F;
        particle.alpha = 0.7F;
        particle.life = particle.span = 3600;
        add(cosmetics, particle);
    }
}

void shockwave(Cosmetics& cosmetics, Cell cell, std::uint8_t red,
               std::uint8_t green, std::uint8_t blue) {
    if (cosmetics.rings.size() >= 96) cosmetics.rings.erase(cosmetics.rings.begin());
    RingParticle ring;
    ring.x = static_cast<float>(cell.x) + 0.5F;
    ring.y = static_cast<float>(cell.y) + 0.5F;
    ring.radius = 0.1F;
    ring.speed = 0.15F;
    ring.life = ring.span = 12;
    ring.red = red; ring.green = green; ring.blue = blue;
    cosmetics.rings.push_back(ring);
}

void shake_tiles(Cosmetics& cosmetics, Cell cell, float strength, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            const int length = std::abs(dx) + std::abs(dy);
            if (length > radius) continue;
            if (cosmetics.tile_shakes.size() >= 96)
                cosmetics.tile_shakes.erase(cosmetics.tile_shakes.begin());
            cosmetics.tile_shakes.push_back(
                {{cell.x + dx, cell.y + dy}, strength / static_cast<float>(length + 1), 24});
        }
    }
}

} // namespace

void spawn_footprint(Cosmetics& cosmetics, Cell cell, EntityKind kind,
                     bool right_foot, std::uint64_t seed) {
    const Sprite sprite = kind == EntityKind::Player ? Sprite::PlayerFootprint :
                          Sprite::ZombieFootprint;
    SpriteParticle particle;
    particle.sprite = sprite;
    particle.layer = ParticleLayer::Ground;
    particle.x = static_cast<float>(cell.x) + (right_foot ? 0.7F : 0.3F);
    particle.y = static_cast<float>(cell.y) + 0.84F;
    particle.width = particle.height = 0.5F;
    particle.angle = (unit(bits(seed)) - 0.5F) * 20.0F;
    particle.alpha = 0.48F;
    particle.life = particle.span = 150;
    add(cosmetics, particle);
}

void spawn_hit(Cosmetics& cosmetics, Cell cell, std::uint64_t seed, int damage) {
    const int count = std::clamp(4 + damage / 2, 4, 24);
    spray(cosmetics, cell, seed, count, Sprite::BloodSmall, 0.12F, 0.015F);
    puddle(cosmetics, cell, seed ^ 0x6b6b6b6bU, std::clamp(damage / 4, 2, 5));
}

void spawn_zombie_scratch(Cosmetics& cosmetics, Cell cell, Cell facing,
                          std::uint64_t seed) {
    SpriteParticle scratch;
    scratch.sprite = Sprite::ZombieScratch1;
    scratch.layer = ParticleLayer::Foreground;
    scratch.x = static_cast<float>(cell.x) + 0.5F + 0.7F * static_cast<float>(facing.x);
    scratch.y = static_cast<float>(cell.y) + 0.5F + 0.7F * static_cast<float>(facing.y);
    scratch.width = scratch.height = 0.9F;
    scratch.angle = (unit(bits(seed)) - 0.5F) * 90.0F;
    scratch.life = scratch.span = 12;
    add(cosmetics, scratch);
}

void spawn_debris(Cosmetics& cosmetics, Cell cell, std::uint64_t seed) {
    spray(cosmetics, cell, seed, 8, Sprite::Ruin, 0.1F, 0.018F);
}

void spawn_terrain_impact(Cosmetics& cosmetics, const ImpactEvent& impact,
                           std::uint64_t seed) {
    shake_tiles(cosmetics, impact.cell, impact.damage > 0 ? 0.12F : 0.035F, 0);
    if (impact.damage <= 0) return;
    spray(cosmetics, impact.cell, seed, impact.broken ? 14 : 5,
          impact.material, impact.broken ? 0.15F : 0.08F, 0.012F);
}

void spawn_death(Cosmetics& cosmetics, Cell cell, EntityKind kind,
                 float angle, std::uint64_t seed) {
    if (kind == EntityKind::Zombie || kind == EntityKind::Player) {
        SpriteParticle corpse;
        corpse.sprite = kind == EntityKind::Zombie ? Sprite::ZombieDead : Sprite::PlayerDead;
        corpse.layer = ParticleLayer::Ground;
        corpse.x = static_cast<float>(cell.x) + 0.5F;
        corpse.y = static_cast<float>(cell.y) + 0.5F;
        corpse.angle = angle;
        corpse.life = corpse.span = 900;
        add(cosmetics, corpse);
    }
    spray(cosmetics, cell, seed, 32, Sprite::BloodSmall, 0.18F, 0.015F);
    puddle(cosmetics, cell, seed ^ 0x23459bdeU, 8);
}

void spawn_sound_effect(Cosmetics& cosmetics, const SoundEvent& sound,
                        std::uint64_t seed) {
    switch (sound.sound) {
    case SoundId::StinkBreak:
        spray(cosmetics, sound.cell, seed, 5, Sprite::DebrisPottery, .05F, .008F);
        break;
    case SoundId::FruitSplat:
        spray(cosmetics, sound.cell, seed, 7, Sprite::DebrisMushroomStem, .05F, .006F);
        break;
    case SoundId::PitchBurst:
        spray(cosmetics, sound.cell, seed, 8, Sprite::FlameA, .07F, .002F);
        shockwave(cosmetics, sound.cell, 221, 131, 59);
        break;
    case SoundId::HandBell:
        shockwave(cosmetics, sound.cell, 212, 185, 105);
        break;
    case SoundId::CrackerBang:
        shockwave(cosmetics, sound.cell, 255, 213, 128);
        spray(cosmetics, sound.cell, seed, 8, Sprite::DebrisCloth, 0.14F, 0.014F);
        break;
    case SoundId::NetCatch: case SoundId::NetFall:
        spray(cosmetics, sound.cell, seed, 4, Sprite::DebrisCloth, .045F, .004F);
        break;
    case SoundId::RabbitEscape:
        spray(cosmetics, sound.cell, seed, 5, Sprite::DebrisGrassBlade, .06F, .006F);
        break;
    case SoundId::AcornBurst:
        spray(cosmetics, sound.cell, seed, 18, Sprite::DebrisWoodChip, .13F, .005F);
        shockwave(cosmetics, sound.cell, 200, 167, 107);
        break;
    case SoundId::SpringLaunch:
        spray(cosmetics, sound.cell, seed, 5, Sprite::DebrisTwig, .055F, .008F);
        break;
    case SoundId::SpringBreak:
        spray(cosmetics, sound.cell, seed, 9, Sprite::DebrisWoodChip, .08F, .008F);
        break;
    case SoundId::SwapFold:
        shockwave(cosmetics, sound.cell, 186, 137, 228);
        spray(cosmetics, sound.cell, seed, 6, Sprite::SwapSeed, .04F, 0.0F);
        break;
    case SoundId::ToadPuff: case SoundId::MothDust:
        spray(cosmetics, sound.cell, seed, sound.sound == SoundId::ToadPuff ? 22 : 10,
              Sprite::DebrisSpore, .055F, 0.0F);
        break;
    case SoundId::BoarHit: case SoundId::BearSlam:
        shake_tiles(cosmetics, sound.cell, .16F, 2);
        spray(cosmetics, sound.cell, seed, 8, Sprite::DebrisWoodChip, .08F, .01F);
        break;
    case SoundId::ShellKnock:
        spray(cosmetics, sound.cell, seed, 4, Sprite::DebrisStoneChip, .05F, .01F);
        break;
    case SoundId::Explosion: case SoundId::Explosion1:
    case SoundId::Explosion2: case SoundId::Explosion3:
        shockwave(cosmetics, sound.cell, 255, 169, 67);
        shake_tiles(cosmetics, sound.cell, 0.45F, 3);
        spray(cosmetics, sound.cell, seed, 12, Sprite::Ruin, 0.18F, 0.012F);
        for (int index = 0; index < 4; ++index) {
            SpriteParticle fragment;
            fragment.sprite = Sprite::Ruin;
            fragment.motion = ParticleMotion::Arc;
            fragment.x = static_cast<float>(sound.cell.x) + 0.5F;
            fragment.y = static_cast<float>(sound.cell.y) + 0.5F;
            fragment.vx = (static_cast<float>(index) - 1.5F) * 0.06F;
            fragment.arc = 0.9F;
            fragment.width = fragment.height = 0.2F;
            fragment.spin = 12.0F;
            fragment.life = fragment.span = 24;
            add(cosmetics, fragment);
        }
        break;
    case SoundId::RailPlace: case SoundId::SturdyBlockBouncedOn:
    case SoundId::HitBlock1:
        shake_tiles(cosmetics, sound.cell, 0.18F, 1);
        spray(cosmetics, sound.cell, seed, 5, Sprite::Ruin, 0.09F, 0.018F);
        break;
    case SoundId::TrainPassing: {
        if (cosmetics.ribbons.size() >= 96) cosmetics.ribbons.erase(cosmetics.ribbons.begin());
        RibbonParticle ribbon;
        ribbon.count = 3;
        ribbon.points[0] = {static_cast<float>(sound.cell.x) + 1.8F,
                            static_cast<float>(sound.cell.y) + 0.25F};
        ribbon.points[1] = {static_cast<float>(sound.cell.x) + 1.1F,
                            static_cast<float>(sound.cell.y) + 0.4F};
        ribbon.points[2] = {static_cast<float>(sound.cell.x) + 0.5F,
                            static_cast<float>(sound.cell.y) + 0.35F};
        ribbon.red = 255; ribbon.green = 211; ribbon.blue = 135;
        ribbon.life = ribbon.span = 12;
        cosmetics.ribbons.push_back(ribbon);
        break;
    }
    default: break;
    }
}

void spawn_weather_cloud(Cosmetics& cosmetics, Cell focus, std::uint64_t seed,
                         float zoom) {
    const std::uint32_t roll = bits(seed);
    if (roll % 80U != 0) return;
    SpriteParticle cloud;
    const std::uint32_t details = bits(seed + 17U);
    cloud.sprite = details % 3U == 0 ? Sprite::Cloud1 :
                   (details % 3U == 1 ? Sprite::Cloud2 : Sprite::Cloud3);
    cloud.layer = ParticleLayer::Weather;
    cloud.motion = ParticleMotion::Drift;
    cloud.depth_height = 50;
    const float width = view_width / tile_pixels(zoom);
    const float height = view_height / tile_pixels(zoom);
    cloud.x = static_cast<float>(focus.x) - width * 0.5F - 16.0F;
    cloud.y = static_cast<float>(focus.y) - height * 0.5F +
              unit(roll >> 8) * height;
    cloud.vx = 0.005F + unit(bits(seed + 13U)) * 0.010F;
    cloud.width = cloud.height = 4.0F + unit(roll >> 16) * 12.0F;
    cloud.alpha = 0.05F + unit(bits(seed + 7U)) * 0.05F;
    cloud.life = cloud.span = static_cast<int>((width + 16.0F) / cloud.vx);
    add(cosmetics, cloud);
}

void spawn_campfire_smoke(Cosmetics& cosmetics, Cell cell, std::uint64_t seed) {
    const std::uint32_t roll = bits(seed);
    SpriteParticle smoke;
    smoke.sprite = Sprite::Cloud1;
    smoke.next_sprite = Sprite::Cloud2;
    smoke.motion = ParticleMotion::Animated;
    smoke.x = static_cast<float>(cell.x) + 0.45F + (unit(roll) - 0.5F) * 0.3F;
    smoke.y = static_cast<float>(cell.y) + 0.15F;
    smoke.vx = (unit(roll >> 8) - 0.5F) * 0.01F;
    smoke.vy = -0.025F;
    smoke.width = smoke.height = 0.45F;
    smoke.alpha = 0.38F;
    smoke.life = smoke.span = 72;
    add(cosmetics, smoke);
}

void spawn_flame(Cosmetics& cosmetics, Cell cell, std::uint64_t seed, bool on_actor) {
    const std::uint32_t roll = bits(seed);
    SpriteParticle flame;
    flame.sprite = Sprite::FlameA;
    flame.next_sprite = Sprite::FlameB;
    flame.layer = on_actor ? ParticleLayer::Foreground : ParticleLayer::Flames;
    flame.motion = ParticleMotion::Animated;
    flame.x = static_cast<float>(cell.x) + 0.5F + (unit(roll) - 0.5F) * 0.36F;
    // ANCHOR: The flame's lower pixels overlap the wood instead of hovering above it.
    flame.y = static_cast<float>(cell.y) + (on_actor ? 0.58F : 0.48F);
    flame.vx = (unit(roll >> 8) - 0.5F) * 0.007F;
    flame.vy = -0.012F;
    flame.width = on_actor ? 0.22F : 0.42F;
    flame.height = on_actor ? 0.32F : 0.65F;
    flame.self_glow = {1.0F, 0.82F, 0.55F};
    flame.life = flame.span = 24;
    add(cosmetics, flame);
}
