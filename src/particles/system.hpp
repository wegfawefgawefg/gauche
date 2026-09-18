#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
#include "../debris/system.hpp"
#include "../scenery/ice.hpp"

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

enum class ParticleLayer : std::uint8_t { Ground, Flames, Foreground, Weather };
enum class ParticleMotion : std::uint8_t { Still, Drift, Accelerate, Arc, Animated, Fall };

struct SpriteParticle {
    Handle owner{}; // Sleep motes end when this generation wakes or disappears.
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
    LightColor self_glow{};
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
    bool water = false;
    std::uint8_t red = 255;
    std::uint8_t green = 255;
    std::uint8_t blue = 255;
};

struct TileShake {
    Cell cell{};
    float strength = 0.0F;
    int life = 0;
};

// Presentation only: prop collision and damage always use the unchanged cell.
struct PropJolt {
    Cell cell{}, direction{};
    int life=12;
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
    ViewCamera previous_camera{}, camera_position{};
    ViewCamera camera_velocity{};
    std::array<ViewCamera, 60> camera_samples{};
    int camera_sample_index = 0, camera_stride = 7;
    bool camera_guide_ready = false;
};

struct Cosmetics {
    LooseDebris debris;
    std::vector<SpriteParticle> sprites;
    std::vector<RibbonParticle> ribbons;
    std::vector<RingParticle> rings;
    std::vector<TileShake> tile_shakes;
    std::vector<PropJolt> prop_jolts;
    std::vector<LightFlash> flashes;
    mutable LightingCache lighting;
    mutable IceScenery ice_scenery;
    std::array<EntityPose, max_entities> poses{};
    std::array<std::uint64_t, 256> seen_events{};
    std::size_t next_event = 0;
    std::uint64_t last_tick = 0;
    std::uint64_t sleep_tick = UINT64_MAX;
    int last_floor = -1;
    RunPhase last_phase = RunPhase::Arena;
    ViewCamera camera{};
    bool camera_ready = false;
    float frame_alpha = 1.0F;
};

void step_particles(Cosmetics& cosmetics);
void update_cosmetics(Cosmetics& cosmetics, const Game& game, Cell focus,
                      float zoom = 2.0F);
ViewCamera camera_for(const Cosmetics& cosmetics, const Game& game, int owner);
void draw_particles(tr::Renderer* renderer, const GameGraphics& graphics,
                    const Cosmetics& cosmetics, ParticleLayer layer, ViewCamera camera,
                    float zoom, const LightingCache* lighting = nullptr,
                    const Stage* stage = nullptr);
