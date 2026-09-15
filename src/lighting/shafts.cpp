#include "canopy.hpp"

#include <algorithm>
#include <array>

void draw_canopy_shafts(SDL_Renderer* renderer, const Game& game,
                        ViewCamera camera, float zoom) {
    if (game.run.phase == RunPhase::Arena || !forest_floor(game.run.floor)) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const float pixels = tile_pixels(zoom);
    for (int i = 0; i < game.run.roof_light_count; ++i) {
        const StageLight& source = game.run.roof_lights[static_cast<std::size_t>(i)];
        // GAP: Anchor on the stencil; sway changes brightness without jumping the landing.
        Cell best = source.cell;
        float strongest = 0;
        for (int dy = -4; dy <= 4; ++dy)
            for (int dx = -4; dx <= 4; ++dx) {
                const Cell cell = source.cell + Cell{dx, dy};
                if (!walkable(game.stage.at_or_border(cell).kind)) continue;
                const float strength = canopy_strength(source, static_cast<float>(cell.x) + .5F,
                    static_cast<float>(cell.y) + .5F, 0);
                if (strength > strongest) { strongest = strength; best = cell; }
            }
        if (strongest < .35F) continue;
        const float x = view_center_x + (static_cast<float>(best.x) + .5F - camera.x) * pixels;
        const float y = view_center_y + (static_cast<float>(best.y) + .5F - camera.y) * pixels;
        if (x < -pixels * 8 || x > 640 + pixels || y < 0 || y > 360 + pixels * 10) continue;
        const float alpha = .09F * canopy_strength(source, static_cast<float>(best.x) + .5F,
                                                   static_cast<float>(best.y) + .5F, game.tick);
        const float top_x = x + pixels * 5;
        const float top_y = y - pixels * 9;
        // HAZE: Fade both edges and the landing, so the shaft has no rectangular cap.
        constexpr float progress[]{0, .35F, .86F, 1};
        constexpr float brightness[]{0, .35F, 1, 0};
        constexpr float cross[]{-1, -.45F, .45F, 1};
        std::array<SDL_Vertex, 16> beam{};
        std::array<int, 54> indices{};
        int index_count = 0;
        for (int row = 0; row < 4; ++row) {
            const float t = progress[row];
            const float center_x = top_x + (x - top_x) * t / .86F;
            const float center_y = top_y + (y - top_y) * t / .86F;
            const float half_width = pixels * (.15F + t * .65F);
            for (int column = 0; column < 4; ++column) {
                const float opacity = column == 0 || column == 3 ? 0 : alpha * brightness[row];
                beam[static_cast<std::size_t>(row * 4 + column)] =
                    {{center_x + cross[column] * half_width, center_y}, {1, .94F, .7F, opacity}, {}};
                if (row == 3 || column == 3) continue;
                const int base = row * 4 + column;
                for (int vertex : {base, base + 1, base + 5, base, base + 5, base + 4})
                    indices[static_cast<std::size_t>(index_count++)] = vertex;
            }
        }
        SDL_RenderGeometry(renderer, nullptr, beam.data(), static_cast<int>(beam.size()),
                           indices.data(), index_count);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
