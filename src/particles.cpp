#include "particles.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

namespace {

std::uint32_t mix(std::uint64_t value) {
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    return static_cast<std::uint32_t>(value ^ (value >> 31));
}

float signed_unit(std::uint32_t bits) {
    return static_cast<float>(bits & 1023U) / 511.5F - 1.0F;
}

void spawn(Cosmetics& cosmetics, Cell cell, std::uint64_t seed,
           int count, int span, float speed,
           std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    for (int index = 0; index < count; ++index) {
        const std::uint32_t bits = mix(seed + static_cast<std::uint64_t>(index));
        Particle& particle = cosmetics.particles[
            cosmetics.next_particle++ % cosmetics.particles.size()];
        particle.x = static_cast<float>(cell.x) + 0.5F;
        particle.y = static_cast<float>(cell.y) + 0.5F;
        particle.vx = signed_unit(bits) * speed;
        particle.vy = signed_unit(bits >> 10) * speed;
        particle.life = span + static_cast<int>((bits >> 20) % 5U);
        particle.span = particle.life;
        particle.red = red;
        particle.green = green;
        particle.blue = blue;
    }
}

void spawn_for_sound(Cosmetics& cosmetics, const SoundEvent& sound,
                     std::uint64_t seed) {
    switch (sound.sound) {
    case SoundId::Explosion:
    case SoundId::Explosion1:
    case SoundId::Explosion2:
    case SoundId::Explosion3:
        spawn(cosmetics, sound.cell, seed, 18, 20, 0.035F, 255, 151, 54);
        break;
    case SoundId::Death:
    case SoundId::AnimalCrush1:
    case SoundId::AnimalCrush2:
        spawn(cosmetics, sound.cell, seed, 10, 17, 0.026F, 201, 55, 55);
        break;
    case SoundId::Punch1:
    case SoundId::BallHitPaddle:
    case SoundId::ZombieScratch1:
        spawn(cosmetics, sound.cell, seed, 4, 10, 0.025F, 230, 220, 177);
        break;
    case SoundId::RailPlace:
    case SoundId::SmallLaser:
        spawn(cosmetics, sound.cell, seed, 4, 12, 0.03F, 250, 215, 105);
        break;
    case SoundId::Step1:
    case SoundId::Step2:
        spawn(cosmetics, sound.cell, seed, 1, 8, 0.008F, 137, 146, 118);
        break;
    default: break;
    }
}

} // namespace

void update_cosmetics(Cosmetics& cosmetics, const Game& game, Cell focus) {
    if (game.tick < cosmetics.last_tick) cosmetics = {};
    cosmetics.last_tick = game.tick;
    for (Particle& particle : cosmetics.particles) {
        if (particle.life <= 0) continue;
        particle.x += particle.vx;
        particle.y += particle.vy;
        particle.vy += 0.0008F;
        --particle.life;
    }
    for (int index = 0; index < game.sound_count; ++index) {
        const SoundEvent& sound = game.sounds[static_cast<std::size_t>(index)];
        const std::uint64_t key = (sound.tick << 8) |
            (static_cast<std::uint64_t>(sound.sequence) + 1);
        if (std::find(cosmetics.seen_events.begin(),
                      cosmetics.seen_events.end(), key) != cosmetics.seen_events.end()) continue;
        cosmetics.seen_events[cosmetics.next_event++ % cosmetics.seen_events.size()] = key;
        if (distance(sound.cell, focus) <= 12) spawn_for_sound(cosmetics, sound, key);
    }
    if (game.tick % 10 != 0) return;
    for (std::size_t slot = 0; slot < game.entities.size(); ++slot) {
        const Entity& entity = game.entities[slot];
        if (entity.kind != EntityKind::Campfire || distance(entity.cell, focus) > 10) continue;
        spawn(cosmetics, entity.cell, game.tick + slot * 17U,
              1, 20, 0.007F, 255, 156, 66);
    }
}

void draw_cosmetics(SDL_Renderer* renderer, const Cosmetics& cosmetics, Cell camera) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    for (const Particle& particle : cosmetics.particles) {
        if (particle.life <= 0) continue;
        const float x = 320.0F + (particle.x - static_cast<float>(camera.x)) * 32.0F;
        const float y = 160.0F + (particle.y - static_cast<float>(camera.y)) * 32.0F;
        if (x < 0.0F || x >= 640.0F || y < 0.0F || y >= 285.0F) continue;
        const float size = particle.span > 15 ? 3.0F : 2.0F;
        const auto alpha = static_cast<std::uint8_t>(
            225 * particle.life / std::max(1, particle.span));
        SDL_SetRenderDrawColor(renderer, particle.red, particle.green, particle.blue, alpha);
        const SDL_FRect rect{x - size * 0.5F, y - size * 0.5F, size, size};
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
