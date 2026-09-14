#include "knight_render.hpp"
#include "mirror_knight.hpp"

#include <algorithm>
#include <cmath>

void draw_knight_shield(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& knight, SDL_FRect rect, LightColor light) {
    if (knight.health <= 0) return;
    const bool guard = knight_reflects(knight, {-knight.facing.x, -knight.facing.y});
    const float lift = guard ? 1.0F : knight.label_a == KnightRaise ?
        1.0F - static_cast<float>(std::clamp(knight.timer_a, 0, 18)) / 18.0F : 0;
    const float size = .46F + lift * .12F;
    const float forward = .23F + lift * .09F;
    // FACING: The shield is placed on the actual guarded edge, even facing up/down.
    SDL_FRect shield{rect.x + rect.w * (.5F - size * .5F + static_cast<float>(knight.facing.x) * forward),
        rect.y + rect.h * (.56F - size * .5F + static_cast<float>(knight.facing.y) * forward + (1-lift)*.13F),
        rect.w * size, rect.h * size};
    SDL_Texture* texture = texture_for(graphics, guard ? Sprite::KnightShieldLit : Sprite::KnightShield);
    SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
    const double angle = std::atan2(static_cast<double>(knight.facing.y), static_cast<double>(knight.facing.x)) * 180.0 / 3.141592653589793;
    SDL_RenderTextureRotated(renderer, texture, nullptr, &shield, angle, nullptr, SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
}
