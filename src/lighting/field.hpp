#pragma once

#include "../game.hpp"
#include "../view.hpp"

#include <span>
#include <vector>

struct LightColor {
    float red = 0.0F;
    float green = 0.0F;
    float blue = 0.0F;
};

inline LightColor light_color(LightTint tint) {
    constexpr float unit = 1.0F / 255.0F;
    return {static_cast<float>(tint.red) * unit,
            static_cast<float>(tint.green) * unit,
            static_cast<float>(tint.blue) * unit};
}

struct LightSource {
    Cell cell{};
    int radius = 0;
    float power = 0.0F;
    LightColor color{1.0F, 1.0F, 1.0F};
    Cell facing{1,0};
    LightShape shape = LightShape::Omni;
};

struct LightFlash {
    LightSource source{};
    int life = 0;
    int span = 0;
};

struct LightingCache {
    Cell origin{};
    int width = 0;
    int height = 0;
    bool active = false;
    std::uint64_t revision = 0;
    std::vector<float> ambient;
    std::vector<LightColor> cast;

    bool contains(Cell cell) const;
    std::size_t index(Cell cell) const;
};

std::vector<LightSource> collect_light_sources(const Game& game,
    const LightingCache& cache, std::span<const LightFlash> flashes);
void build_lighting(LightingCache& cache, const Game& game,
    ViewCamera camera, float zoom, std::span<const LightFlash> flashes = {});
LightColor light_at_cell(const LightingCache& cache, Cell cell);
LightColor light_at_corner(const LightingCache& cache, Cell corner);
