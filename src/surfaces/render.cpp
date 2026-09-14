#include "render.hpp"
#include "../particles/templates.hpp"

#include <algorithm>
#include <cmath>

namespace {

void draw_puddle(SDL_Renderer* renderer, const Stage& stage, Cell cell,
                 SDL_FRect rect, LiquidKind kind) {
    const auto same = [&](Cell side) {
        const Surface& neighbor = stage.at_or_border(cell + side).surface;
        return neighbor.liquid == kind && neighbor.liquid_ticks > 0;
    };
    const bool left = same({-1, 0}), right = same({1, 0});
    const bool top = same({0, -1}), bottom = same({0, 1});
    // BANKS: Joined cells form one quiet pool, with stepped edges only at its banks.
    for (int row = 0; row < 8; ++row) {
        if ((row == 0 && !top) || (row == 7 && !bottom)) continue;
        const float inset = (row <= 1 || row >= 6) ? .25F : .125F;
        const float begin = left ? 0 : inset, end = right ? 1 : 1 - inset;
        const SDL_FRect strip{rect.x + rect.w * begin, rect.y + rect.h * static_cast<float>(row) / 8,
                              rect.w * (end - begin), rect.h / 8};
        SDL_RenderFillRect(renderer, &strip);
    }
}

void draw_haze(SDL_Renderer* renderer, SDL_FRect rect, LightColor light,
               bool sleep, float opacity, std::uint64_t tick, Cell cell) {
    const float drift = std::sin(static_cast<float>(tick % 360) / 57.3F + static_cast<float>(cell.x));
    rect.x -= rect.w * (.12F + drift * .04F); rect.y -= rect.h * .22F;
    rect.w *= 1.24F; rect.h *= 1.24F;
    for (int row = 0; row < 8; ++row) {
        const float inset = row == 0 || row == 7 ? .30F : row == 1 || row == 6 ? .14F : .04F;
        SDL_SetRenderDrawColorFloat(renderer, light.red * (sleep ? .48F : .30F),
            light.green * (sleep ? .43F : .31F), light.blue * (sleep ? .65F : .32F),
            opacity * (row == 0 || row == 7 ? .35F : row == 1 || row == 6 ? .65F : 1));
        const SDL_FRect strip{rect.x + rect.w * inset, rect.y + rect.h * static_cast<float>(row) / 8,
                              rect.w * (1 - inset * 2), rect.h / 8};
        SDL_RenderFillRect(renderer, &strip);
    }
}

// SCENT: A few rising strokes leave the underlying ground readable.
void draw_scent(SDL_Renderer* renderer, SDL_FRect rect, LightColor light,
                float fade, std::uint64_t tick, Cell cell) {
    for (int wisp = 0; wisp < 2; ++wisp) {
        const int offset = (cell.x * 13 + cell.y * 29 + wisp * 47) % 90;
        const float age = static_cast<float>((tick + static_cast<std::uint64_t>(offset + 90)) % 90) / 90;
        const float opacity = .65F * fade * std::sin(age * 3.14159265F);
        SDL_SetRenderDrawColorFloat(renderer, light.red * .60F, light.green * .73F,
            light.blue * .32F, opacity);
        SDL_FPoint points[6];
        for (int i = 0; i < 6; ++i) {
            const float phase = static_cast<float>(i) / 5;
            points[i] = {rect.x + rect.w * (.30F + static_cast<float>(wisp) * .35F +
                .08F * std::sin(phase * 6.2831853F + age * 4)),
                rect.y + rect.h * (.80F - age * .35F - phase * .4F)};
        }
        SDL_RenderLines(renderer, points, 6);
    }
}

} // namespace

void draw_surfaces(SDL_Renderer* renderer, const Game& game, ViewCamera camera,
                    float zoom, const LightingCache& lighting, bool clouds) {
    const float pixels = tile_pixels(zoom);
    const int rx = static_cast<int>(320 / pixels) + 2, ry = static_cast<int>(180 / pixels) + 2;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int y = std::max(0, static_cast<int>(camera.y) - ry); y < std::min(game.stage.height, static_cast<int>(camera.y) + ry); ++y)
        for (int x = std::max(0, static_cast<int>(camera.x) - rx); x < std::min(game.stage.width, static_cast<int>(camera.x) + rx); ++x) {
            const Cell cell{x, y};
            const Surface& surface = game.stage.at(cell)->surface;
            const LightColor light = light_at_cell(lighting, cell);
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            if (clouds) {
                const int time = std::max(surface.smoke_ticks, surface.sleep_ticks);
                if (surface.scent_ticks > 0)
                    draw_scent(renderer, rect, light,
                        std::min(1.0F, static_cast<float>(surface.scent_ticks) / 90), game.tick, cell);
                if (time <= 0) continue;
                draw_haze(renderer, rect, light, surface.sleep_ticks > 0,
                    .32F * std::min(1.0F, static_cast<float>(time) / 60), game.tick, cell);
                continue;
            }
            if (surface.liquid == LiquidKind::None) continue;
            LightColor color;
            switch (surface.liquid) {
            case LiquidKind::Rot: color = {.34F, .38F, .14F}; break;
            case LiquidKind::SpentSap: color = {.28F, .20F, .12F}; break;
            case LiquidKind::Oil: color = {.13F, .14F, .22F}; break;
            case LiquidKind::Sap: color = {.50F, .31F, .13F}; break;
            case LiquidKind::Water: color = {.24F, .46F, .48F}; break;
            case LiquidKind::Honey: color = {.58F, .40F, .15F}; break;
            default: continue;
            }
            SDL_SetRenderDrawColorFloat(renderer, light.red * color.red, light.green * color.green,
                light.blue * color.blue, .65F * std::min(1.0F, static_cast<float>(surface.liquid_ticks) / 90));
            draw_puddle(renderer, game.stage, cell, rect, surface.liquid);
        }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void observe_surfaces(Cosmetics& cosmetics, const Game& game, Cell focus) {
    if (game.tick % 6 != 0) return;
    for (int y = focus.y - 14; y <= focus.y + 14; ++y)
        for (int x = focus.x - 22; x <= focus.x + 22; ++x) {
            const Tile* tile = game.stage.at({x, y});
            if (tile == nullptr || tile->surface.fire_ticks == 0) continue;
            const auto seed = game.tick ^ static_cast<std::uint64_t>(x * 71 + y * 131);
            spawn_flame(cosmetics, {x, y}, seed, false);
            if (game.tick % 24 == 0) spawn_campfire_smoke(cosmetics, {x, y}, seed);
        }
}
