#pragma once

#include "../game.hpp"

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

enum class ParticleLayer : std::uint8_t { Ground, Foreground, Weather };
enum class ParticleMotion : std::uint8_t { Still, Drift, Accelerate, Arc, Animated };

struct SpriteParticle {
    Sprite sprite = Sprite::BloodSmall;
    Sprite next_sprite = Sprite::BloodSmall;
    ParticleLayer layer = ParticleLayer::Foreground;
    ParticleMotion motion = ParticleMotion::Still;
    float x = 0.0F;
    float y = 0.0F;
    float vx = 0.0F;
    float vy = 0.0F;
    float ax = 0.0F;
    float ay = 0.0F;
    float arc = 0.0F;
    float width = 1.0F;
    float height = 1.0F;
    float angle = 0.0F;
    float spin = 0.0F;
    int depth_height = 0;
    float alpha = 1.0F;
    int life = 0;
    int span = 0;
};

struct RibbonParticle {
    ParticleLayer layer = ParticleLayer::Foreground;
    std::array<SDL_FPoint, 8> points{};
    int count = 0;
    int life = 0;
    int span = 0;
    std::uint8_t red = 255;
    std::uint8_t green = 255;
    std::uint8_t blue = 255;
};

struct RingParticle {
    ParticleLayer layer = ParticleLayer::Foreground;
    float x = 0.0F;
    float y = 0.0F;
    float radius = 0.0F;
    float speed = 0.0F;
    int life = 0;
    int span = 0;
    std::uint8_t red = 255;
    std::uint8_t green = 255;
    std::uint8_t blue = 255;
};

struct TileShake {
    Cell cell{};
    float strength = 0.0F;
    int life = 0;
};

struct EntityPose {
    std::uint32_t generation = 0;
    EntityKind kind = EntityKind::None;
    Cell cell{};
    int health = 0;
    int attack_wait = 0;
    int use_flash = 0;
    float angle = 0.0F;
    float shake = 0.0F;
    bool horizontal_flip = false;
    bool seen = false;
    std::uint32_t steps = 0;
};

struct Cosmetics {
    std::vector<SpriteParticle> sprites;
    std::vector<RibbonParticle> ribbons;
    std::vector<RingParticle> rings;
    std::vector<TileShake> tile_shakes;
    std::array<EntityPose, max_entities> poses{};
    std::array<std::uint64_t, 256> seen_events{};
    std::size_t next_event = 0;
    std::uint64_t last_tick = 0;
    int last_floor = -1;
    RunPhase last_phase = RunPhase::Arena;
};

void step_particles(Cosmetics& cosmetics);
void update_cosmetics(Cosmetics& cosmetics, const Game& game, Cell focus,
                      float zoom = 2.0F);
void draw_particles(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Cosmetics& cosmetics, ParticleLayer layer, Cell camera, float zoom);
