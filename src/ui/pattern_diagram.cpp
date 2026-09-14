#include "pattern_diagram.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {

SDL_Color effect_color(PatternEffect effect) {
    switch (effect) {
    case PatternEffect::Damage: return {224, 76, 61, 255};
    case PatternEffect::Heal: return {91, 214, 126, 255};
    case PatternEffect::Utility: return {211, 214, 205, 255};
    case PatternEffect::None: break;
    }
    return {90, 98, 91, 255};
}

void colored_cell(SDL_Renderer* renderer, const PatternDiagramLayout& layout,
                  int x, int y, PatternEffect effect, bool travel) {
    const float inset = std::min(1.0F, layout.cell_size * 0.16F);
    SDL_FRect cell{layout.x + static_cast<float>(x - layout.min_x) * layout.cell_size + inset,
                   layout.y + static_cast<float>(y - layout.min_y) * layout.cell_size + inset,
                   layout.cell_size - inset * 2.0F, layout.cell_size - inset * 2.0F};
    const SDL_Color color = effect_color(effect);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, travel ? 76 : 178);
    if (!travel) SDL_RenderFillRect(renderer, &cell);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, travel ? 142 : 230);
    SDL_RenderRect(renderer, &cell);
}

} // namespace

PatternDiagramLayout pattern_diagram_layout(ItemPattern pattern,
                                             float x, float y,
                                             float width, float height) {
    // BOUNDS: Include the player and the full effect, then one quiet cell on each edge.
    const int blast = std::max(0, pattern.blast_radius);
    const int target = std::max(0, pattern.maximum);
    const int half = std::max(pattern.half_width, blast);
    PatternDiagramLayout layout;
    layout.min_x = std::min(0, target - blast) - 1;
    layout.max_x = target + blast + 1;
    layout.min_y = -half - 1;
    layout.max_y = half + 1;
    layout.columns = layout.max_x - layout.min_x + 1;
    layout.rows = layout.max_y - layout.min_y + 1;
    layout.cell_size = std::min({16.0F, width / static_cast<float>(layout.columns),
                                 height / static_cast<float>(layout.rows)});
    layout.x = x + (width - static_cast<float>(layout.columns) * layout.cell_size) * 0.5F;
    layout.y = y + (height - static_cast<float>(layout.rows) * layout.cell_size) * 0.5F;
    return layout;
}

void draw_pattern_diagram(SDL_Renderer* renderer, const Item& item,
                          float x, float y, float width, float height) {
    const ItemPattern pattern = item_pattern(item);
    if (pattern.effect == PatternEffect::None || width <= 0 || height <= 0) return;
    const PatternDiagramLayout layout = pattern_diagram_layout(pattern, x, y, width, height);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // GRID: The backing follows the effect's bounds instead of filling the whole card.
    for (int row = layout.min_y; row <= layout.max_y; ++row)
        for (int column = layout.min_x; column <= layout.max_x; ++column) {
            SDL_FRect cell{layout.x + static_cast<float>(column - layout.min_x) * layout.cell_size,
                           layout.y + static_cast<float>(row - layout.min_y) * layout.cell_size,
                           layout.cell_size, layout.cell_size};
            SDL_SetRenderDrawColor(renderer, 57, 68, 61, 72);
            SDL_RenderFillRect(renderer, &cell);
            SDL_SetRenderDrawColor(renderer, 110, 126, 112, 80);
            SDL_RenderRect(renderer, &cell);
        }

    // EFFECT: Dashed travel has no hit; the destination and nearby lanes do.
    if (pattern.minimum == 0 && pattern.maximum == 0)
        colored_cell(renderer, layout, 0, 0, pattern.effect, false);
    else if (pattern.blast_radius > 0) {
        for (int reach = 1; reach < pattern.maximum; ++reach)
            colored_cell(renderer, layout, reach, 0, pattern.effect, true);
        for (int dy = -pattern.blast_radius; dy <= pattern.blast_radius; ++dy)
            for (int dx = -pattern.blast_radius; dx <= pattern.blast_radius; ++dx)
                if (std::abs(dx) + std::abs(dy) <= pattern.blast_radius &&
                    (!pattern.cross_blast || dx == 0 || dy == 0))
                    colored_cell(renderer, layout, pattern.maximum + dx, dy,
                                 pattern.effect, false);
    } else {
        for (int reach = 1; reach <= pattern.maximum; ++reach)
            for (int lane = -pattern_half_width(pattern, reach); lane <= pattern_half_width(pattern, reach); ++lane)
                colored_cell(renderer, layout, reach, lane,
                             pattern.effect, reach < pattern.minimum);
    }
    SDL_FRect player{layout.x + static_cast<float>(-layout.min_x) * layout.cell_size,
                     layout.y + static_cast<float>(-layout.min_y) * layout.cell_size,
                     layout.cell_size, layout.cell_size};
    const SDL_Color marker = pattern.minimum == 0 && pattern.maximum == 0 ?
        effect_color(pattern.effect) : SDL_Color{212, 207, 169, 255};
    SDL_SetRenderDrawColor(renderer, marker.r, marker.g, marker.b, 255);
    SDL_RenderFillRect(renderer, &player);
    if (layout.cell_size >= 6.0F) {
        const float scale = layout.cell_size / 8.0F;
        float old_x = 1.0F, old_y = 1.0F;
        SDL_GetRenderScale(renderer, &old_x, &old_y);
        SDL_SetRenderScale(renderer, old_x * scale, old_y * scale);
        SDL_SetRenderDrawColor(renderer, 18, 23, 20, 255);
        SDL_RenderDebugText(renderer, player.x / scale, player.y / scale, "P");
        SDL_SetRenderScale(renderer, old_x, old_y);
    } else {
        SDL_FRect dot{player.x + layout.cell_size * 0.25F,
                      player.y + layout.cell_size * 0.25F,
                      layout.cell_size * 0.5F, layout.cell_size * 0.5F};
        SDL_SetRenderDrawColor(renderer, 18, 23, 20, 255);
        SDL_RenderFillRect(renderer, &dot);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
