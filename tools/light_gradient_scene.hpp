#pragma once

#include "../src/lighting/render.hpp"
#include "../src/ui/text.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

// Static diagnostic: enlarge the canopy cell with the strongest diagonal gradient.
inline void render_light_gradient(SDL_Renderer* renderer, const Game& game, ViewCamera camera) {
    LightingCache light;
    build_lighting(light, game, camera, 2);
    Cell example{12, 10};
    float worst = 0;
    for (int y = 4; y < 14; ++y)
        for (int x = 4; x < 24; ++x) {
            const float nw = light_at_corner(light, {x, y}).green;
            const float ne = light_at_corner(light, {x+1, y}).green;
            const float sw = light_at_corner(light, {x, y+1}).green;
            const float se = light_at_corner(light, {x+1, y+1}).green;
            const float bias = std::abs(nw - ne - sw + se) * .25F;
            if (bias > worst) { worst = bias; example = {x, y}; }
        }
    SDL_Surface* pixel = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
    if (pixel == nullptr) return;
    SDL_FillSurfaceRect(pixel, nullptr, SDL_MapSurfaceRGB(pixel, 255, 255, 255));
    SDL_Texture* white = SDL_CreateTextureFromSurface(renderer, pixel);
    SDL_DestroySurface(pixel);
    if (white == nullptr) return;
    const auto color = [&light](Cell cell) {
        const LightColor value = light_at_corner(light, cell);
        return SDL_FColor{std::clamp(value.red, 0.0F, 1.0F),
            std::clamp(value.green, 0.0F, 1.0F), std::clamp(value.blue, 0.0F, 1.0F), 1};
    };
    const SDL_Vertex original[]{
        {{40, 75}, color(example), {0, 0}},
        {{280, 75}, color(example + Cell{1, 0}), {1, 0}},
        {{280, 315}, color(example + Cell{1, 1}), {1, 1}},
        {{40, 315}, color(example + Cell{0, 1}), {0, 1}},
    };
    constexpr int indices[]{0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, white, original, 4, indices, 6);
    draw_lit_tile(renderer, white, {360, 75, 240, 240}, example, light);
    small_ui_text(renderer, 40, 45, "TWO TRIANGLES");
    small_ui_text(renderer, 360, 45, "BILINEAR SUBDIVISION");
    small_ui_text(renderer, 40, 330, "SAME CORNERS / SAME CANOPY FIELD");
    std::printf("Canopy cell %d,%d: former diagonal-center error %.5f; subdivision bound %.5f\n",
        example.x, example.y, static_cast<double>(worst), static_cast<double>(worst / 16));
    SDL_DestroyTexture(white);
}
