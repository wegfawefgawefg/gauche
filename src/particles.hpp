#pragma once

#include "game.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

struct SDL_Renderer;

struct Particle {
    float x = 0.0F;
    float y = 0.0F;
    float vx = 0.0F;
    float vy = 0.0F;
    int life = 0;
    int span = 0;
    std::uint8_t red = 255;
    std::uint8_t green = 255;
    std::uint8_t blue = 255;
};

struct Cosmetics {
    std::array<Particle, 192> particles{};
    std::array<std::uint64_t, 128> seen_events{};
    std::size_t next_particle = 0;
    std::size_t next_event = 0;
    std::uint64_t last_tick = 0;
};

void update_cosmetics(Cosmetics& cosmetics, const Game& game, Cell focus);
void draw_cosmetics(SDL_Renderer* renderer, const Cosmetics& cosmetics, Cell camera);
