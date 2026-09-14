#include "fire_render.hpp"

#include <cmath>

void draw_item_flame(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Item& item, SDL_FRect rect, Cell facing, std::uint64_t tick) {
    if (item.flame_ticks <= 0) return;
    // TIP: Match the held sprite's rotation and left-facing vertical flip. Fire
    // rises upright from the wood instead of rotating sideways with the weapon.
    const float local_x = rect.w * .28F;
    const float local_y = rect.h * (facing.x < 0 ? .28F : -.28F);
    const float x = rect.x + rect.w*.5F + local_x*static_cast<float>(facing.x) - local_y*static_cast<float>(facing.y);
    const float y = rect.y + rect.h*.5F + local_x*static_cast<float>(facing.y) + local_y*static_cast<float>(facing.x);
    const float flicker = 1 + .10F*std::sin(static_cast<float>(tick % 120)*.8F);
    const float height = rect.h*.80F*flicker;
    SDL_FRect flame{x-rect.w*.25F, y-height+rect.h*.12F, rect.w*.50F, height};
    SDL_Texture* texture = texture_for(graphics, (tick/6)%2 == 0 ? Sprite::FlameA : Sprite::FlameB);
    SDL_RenderTexture(renderer, texture, nullptr, &flame);
}
