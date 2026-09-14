#include "render.hpp"

#include <algorithm>
#include <array>

namespace {

SDL_FColor vertex_color(LightColor light, LightColor tint) {
    return {std::clamp(light.red * tint.red, 0.0F, 1.0F),
            std::clamp(light.green * tint.green, 0.0F, 1.0F),
            std::clamp(light.blue * tint.blue, 0.0F, 1.0F), 1.0F};
}

} // namespace

void draw_lit_tile(SDL_Renderer* renderer, SDL_Texture* texture,
                   SDL_FRect rect, Cell cell, const LightingCache& lighting,
                   LightColor tint) {
    if (texture == nullptr) return;
    const std::array<SDL_Vertex, 4> vertices{{
        {{rect.x, rect.y},
         vertex_color(light_at_corner(lighting, cell), tint), {0.0F, 0.0F}},
        {{rect.x + rect.w, rect.y},
         vertex_color(light_at_corner(lighting, cell + Cell{1, 0}), tint), {1.0F, 0.0F}},
        {{rect.x + rect.w, rect.y + rect.h},
         vertex_color(light_at_corner(lighting, cell + Cell{1, 1}), tint), {1.0F, 1.0F}},
        {{rect.x, rect.y + rect.h},
         vertex_color(light_at_corner(lighting, cell + Cell{0, 1}), tint), {0.0F, 1.0F}},
    }};
    constexpr std::array<int, 6> indices{{0, 1, 2, 0, 2, 3}};
    SDL_RenderGeometry(renderer, texture, vertices.data(),
                       static_cast<int>(vertices.size()),
                       indices.data(), static_cast<int>(indices.size()));
}

LightColor lit_sprite_color(const LightingCache& lighting, Cell cell,
                            LightColor self_glow) {
    const LightColor local = light_at_cell(lighting, cell);
    return {std::max(local.red, self_glow.red),
            std::max(local.green, self_glow.green),
            std::max(local.blue, self_glow.blue)};
}
