#include "pattern_diagram.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

constexpr float side = 8.0F;
constexpr int rows = 7;

SDL_Color effect_color(PatternEffect effect) {
    switch (effect) {
    case PatternEffect::Damage: return {224, 76, 61, 255};
    case PatternEffect::Heal: return {91, 214, 126, 255};
    case PatternEffect::Utility: return {211, 214, 205, 255};
    case PatternEffect::None: break;
    }
    return {90, 98, 91, 255};
}

void colored_cell(SDL_Renderer* renderer, int column, int row, int columns,
                  float x, float y, PatternEffect effect, bool travel) {
    if (column < 0 || column >= columns || row < 0 || row >= rows) return;
    const SDL_Color color = effect_color(effect);
    SDL_FRect rect{x + static_cast<float>(column) * side + 1.0F,
                   y + static_cast<float>(row) * side + 1.0F,
                   side - 2.0F, side - 2.0F};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, travel ? 76 : 178);
    if (!travel) SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, travel ? 142 : 230);
    SDL_RenderRect(renderer, &rect);
}

void blast_cells(SDL_Renderer* renderer, int center, int row, int columns,
                 int radius, float x, float y, PatternEffect effect) {
    for (int dy = -radius; dy <= radius; ++dy)
        for (int dx = -radius; dx <= radius; ++dx)
            if (std::abs(dx) + std::abs(dy) <= radius)
                colored_cell(renderer, center + dx, row + dy, columns,
                             x, y, effect, false);
}

} // namespace

void draw_pattern_diagram(SDL_Renderer* renderer, const Item& item,
                          float x, float y, float width) {
    const ItemPattern pattern = item_pattern(item);
    if (pattern.effect == PatternEffect::None) return;
    const int columns = std::max(5, static_cast<int>((width - 20.0F) / side));
    const int origin = pattern.maximum <= 3 ? columns / 2 : 1;
    constexpr int middle = rows / 2;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column) {
            SDL_FRect cell{x + static_cast<float>(column) * side,
                           y + static_cast<float>(row) * side,
                           side - 1.0F, side - 1.0F};
            SDL_SetRenderDrawColor(renderer, 57, 68, 61, 72);
            SDL_RenderFillRect(renderer, &cell);
            SDL_SetRenderDrawColor(renderer, 110, 126, 112, 80);
            SDL_RenderRect(renderer, &cell);
        }

    if (pattern.minimum == 0 && pattern.maximum == 0)
        colored_cell(renderer, origin, middle, columns, x, y, pattern.effect, false);
    else if (pattern.blast_radius > 0) {
        for (int reach = 1; reach < pattern.maximum; ++reach)
            colored_cell(renderer, origin + reach, middle, columns,
                         x, y, pattern.effect, true);
        blast_cells(renderer, origin + pattern.maximum, middle, columns,
                    pattern.blast_radius, x, y, pattern.effect);
    } else {
        for (int reach = 1; reach <= pattern.maximum; ++reach)
            for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
                colored_cell(renderer, origin + reach, middle + lane, columns,
                             x, y, pattern.effect, reach < pattern.minimum);
    }
    SDL_FRect player{x + static_cast<float>(origin) * side,
                     y + static_cast<float>(middle) * side,
                     side, side};
    SDL_SetRenderDrawColor(renderer, 212, 207, 169, 255);
    SDL_RenderFillRect(renderer, &player);
    SDL_SetRenderDrawColor(renderer, 18, 23, 20, 255);
    SDL_RenderDebugText(renderer, player.x, player.y, "P");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
