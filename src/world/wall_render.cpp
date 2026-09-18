#include "wall_render.hpp"

#include <algorithm>
#include <array>

namespace {

SDL_FColor rim_color(LightColor light, LightColor tint) {
    return {std::clamp(light.red * tint.red * .27F, 0.0F, 1.0F),
            std::clamp(light.green * tint.green * .31F, 0.0F, 1.0F),
            std::clamp(light.blue * tint.blue * .23F, 0.0F, 1.0F), 1};
}

void edge(tr::Renderer* renderer, SDL_FRect rect, SDL_FColor a, SDL_FColor b,
           bool horizontal) {
    const std::array<SDL_Vertex, 4> corners{{
        {{rect.x, rect.y}, a, {}},
        {{rect.x + rect.w, rect.y}, horizontal ? b : a, {}},
        {{rect.x + rect.w, rect.y + rect.h}, b, {}},
        {{rect.x, rect.y + rect.h}, horizontal ? a : b, {}},
    }};
    constexpr int indices[]{0, 1, 2, 0, 2, 3};
    tr::geometry(renderer, nullptr, corners.data(), 4, indices, 6);
}

} // namespace

void draw_wall_contour(tr::Renderer* renderer, const Stage& stage, Cell cell,
                       SDL_FRect rect, const LightingCache& lighting, LightColor tint) {
    const float stroke = std::max(.5F, rect.w / 16);
    const SDL_FColor nw = rim_color(light_at_corner(lighting, cell), tint);
    const SDL_FColor ne = rim_color(light_at_corner(lighting, cell + Cell{1, 0}), tint);
    const SDL_FColor sw = rim_color(light_at_corner(lighting, cell + Cell{0, 1}), tint);
    const SDL_FColor se = rim_color(light_at_corner(lighting, cell + Cell{1, 1}), tint);
    // CONTOUR: Connected walls share a quiet surface; only exposed edges receive a rim.
    if (stage.at_or_border(cell + Cell{0, -1}).kind != TileKind::Wall)
        edge(renderer, {rect.x, rect.y, rect.w, stroke}, nw, ne, true);
    if (stage.at_or_border(cell + Cell{0, 1}).kind != TileKind::Wall)
        edge(renderer, {rect.x, rect.y + rect.h - stroke, rect.w, stroke}, sw, se, true);
    if (stage.at_or_border(cell + Cell{-1, 0}).kind != TileKind::Wall)
        edge(renderer, {rect.x, rect.y, stroke, rect.h}, nw, sw, false);
    if (stage.at_or_border(cell + Cell{1, 0}).kind != TileKind::Wall)
        edge(renderer, {rect.x + rect.w - stroke, rect.y, stroke, rect.h}, ne, se, false);
}
