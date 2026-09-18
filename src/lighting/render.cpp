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

void draw_flat_tile(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FRect rect,
                    SDL_FColor color, SDL_FRect uv, int quarter_turns, bool flip_horizontal) {
    constexpr std::array<SDL_FPoint,5> points{{{0,0},{1,0},{1,1},{0,1},{.5F,.5F}}};
    std::array<SDL_Vertex,5> vertices{};
    for (std::size_t i=0;i<points.size();++i) {
        const auto p=points[i];float u=p.x,v=p.y;
        for (int turn=0;turn<(quarter_turns&3);++turn) {const float old=u;u=v;v=1-old;}
        if (flip_horizontal) u=1-u;
        vertices[i]={{rect.x+rect.w*p.x,rect.y+rect.h*p.y},color,{uv.x+uv.w*u,uv.y+uv.h*v}};
    }
    // A center fan stays on the triangle path in SDL's software renderer. Its
    // rectangle shortcut truncates positions and widths separately, leaving gaps.
    constexpr int indices[]{0,1,4,1,2,4,2,3,4,3,0,4};
    SDL_RenderGeometry(renderer,texture,vertices.data(),static_cast<int>(vertices.size()),indices,12);
}

void draw_lit_tile(SDL_Renderer* renderer, SDL_Texture* texture,
                   SDL_FRect rect, Cell cell, const LightingCache& lighting,
                   LightColor tint,SDL_FRect uv,int quarter_turns,float opacity,bool flip_horizontal) {
    if (texture == nullptr) return;
    if (!lighting.active) {
        auto color=vertex_color({1,1,1},tint);color.a=opacity;
        draw_flat_tile(renderer,texture,rect,color,uv,quarter_turns,flip_horizontal);
        return;
    }
    const SDL_FColor nw = vertex_color(light_at_corner(lighting, cell), tint);
    const SDL_FColor ne = vertex_color(light_at_corner(lighting, cell + Cell{1, 0}), tint);
    const SDL_FColor sw = vertex_color(light_at_corner(lighting, cell + Cell{0, 1}), tint);
    const SDL_FColor se = vertex_color(light_at_corner(lighting, cell + Cell{1, 1}), tint);

    constexpr std::array<SDL_FPoint, 4> points{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    const std::array<SDL_FColor, 4> colors{nw, ne, se, sw};
    std::array<SDL_Vertex, 4> vertices{};
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto p = points[i];
        float u = p.x, v = p.y;
        for (int turn = 0; turn < (quarter_turns & 3); ++turn) {
            const float old = u;
            u = v;
            v = 1 - old;
        }
        if (flip_horizontal) u = 1 - u;
        auto color = colors[i];
        color.a = opacity;
        vertices[i] = {{rect.x + rect.w * p.x, rect.y + rect.h * p.y},
                       color, {uv.x + uv.w * u, uv.y + uv.h * v}};
    }
    constexpr std::array<int, 6> indices{0, 1, 2, 0, 2, 3};
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
