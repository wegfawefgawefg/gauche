#include "render.hpp"

#include <algorithm>
#include <array>

namespace {

SDL_FColor vertex_color(LightColor light, LightColor tint) {
    return {std::clamp(light.red * tint.red, 0.0F, 1.0F),
            std::clamp(light.green * tint.green, 0.0F, 1.0F),
            std::clamp(light.blue * tint.blue, 0.0F, 1.0F), 1.0F};
}

SDL_FColor blend(SDL_FColor a, SDL_FColor b, float t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t, 1.0F};
}

} // namespace

void draw_lit_tile(SDL_Renderer* renderer, SDL_Texture* texture,
                   SDL_FRect rect, Cell cell, const LightingCache& lighting,
                   LightColor tint,SDL_FRect uv,int quarter_turns,float opacity,bool flip_horizontal) {
    if (texture == nullptr) return;
    const SDL_FColor nw = vertex_color(light_at_corner(lighting, cell), tint);
    const SDL_FColor ne = vertex_color(light_at_corner(lighting, cell + Cell{1, 0}), tint);
    const SDL_FColor sw = vertex_color(light_at_corner(lighting, cell + Cell{0, 1}), tint);
    const SDL_FColor se = vertex_color(light_at_corner(lighting, cell + Cell{1, 1}), tint);

    // GRADIENT: A single diagonal splits nonplanar corner colors into two visible facets.
    // Bilinear subdivision preserves shared edges and the original four light samples.
    constexpr int divisions = 4, stride = divisions + 1;
    std::array<SDL_Vertex, stride * stride> vertices{};
    for (int y = 0; y <= divisions; ++y)
        for (int x = 0; x <= divisions; ++x) {
            const float u = static_cast<float>(x) / divisions;
            const float v = static_cast<float>(y) / divisions;
            float tu=u,tv=v;
            for (int turn=0;turn<(quarter_turns&3);++turn) {
                const float old=tu;tu=tv;tv=1-old;
            }
            if (flip_horizontal) tu=1-tu;
            vertices[static_cast<std::size_t>(y * stride + x)] = {
                {rect.x + rect.w * u, rect.y + rect.h * v},
                blend(blend(nw, ne, u), blend(sw, se, u), v),
                {uv.x+tu*uv.w,uv.y+tv*uv.h}};
            vertices[static_cast<std::size_t>(y * stride + x)].color.a=opacity;
        }
    static constexpr auto indices = [] {
        std::array<int, divisions * divisions * 6> result{};
        int next = 0;
        for (int y = 0; y < divisions; ++y)
            for (int x = 0; x < divisions; ++x) {
                const int a = y * stride + x;
                for (int index : {a, a + 1, a + stride + 1, a, a + stride + 1, a + stride})
                    result[static_cast<std::size_t>(next++)] = index;
            }
        return result;
    }();
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
