#include "render.hpp"

#include <algorithm>
#include <cmath>

void draw_props(SDL_Renderer* renderer, const GameGraphics& graphics, const Stage& stage,
                 ViewCamera camera, float zoom, const LightingCache& lighting) {
    const int radius_x = static_cast<int>(320.0F / tile_pixels(zoom)) + 2;
    const int radius_y = static_cast<int>(180.0F / tile_pixels(zoom)) + 2;
    const int center_x = static_cast<int>(std::floor(camera.x));
    const int center_y = static_cast<int>(std::floor(camera.y));
    for (int y = std::max(0, center_y - radius_y); y < std::min(stage.height, center_y + radius_y); ++y)
        for (int x = std::max(0, center_x - radius_x); x < std::min(stage.width, center_x + radius_x); ++x) {
            const Cell cell{x, y};
            const Prop& prop = stage.at(cell)->prop;
            if (prop.kind == PropKind::None || prop.broken) continue;
            const PropSpec spec = prop_spec(prop.kind);
            const LightColor light = light_at_cell(lighting, cell);
            const Sprite sprite = prop.kind == PropKind::Shoot && prop.growth_ticks <= 90 ?
                Sprite::ShootTall : prop.kind == PropKind::IceBlock && prop.growth_ticks <= 120 ?
                Sprite::IceBlockThaw : spec.sprite;
            SDL_Texture* texture = texture_for(graphics, sprite);
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
            SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, 0, nullptr,
                prop.variant % 2 == 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
            SDL_SetTextureColorModFloat(texture, 1, 1, 1);
            if (prop.hp < spec.health) {
                SDL_FRect bar{rect.x + rect.w * .2F, rect.y + rect.h * .87F,
                    rect.w * .6F * static_cast<float>(prop.hp) / static_cast<float>(spec.health), 1};
                SDL_SetRenderDrawColorFloat(renderer, light.red * .8F,
                                           light.green * .6F, light.blue * .3F, 1);
                SDL_RenderFillRect(renderer, &bar);
            }
        }
}
