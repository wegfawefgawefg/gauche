#include "floating_render.hpp"
#include "floating_items.hpp"

void draw_item_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& cargo, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!floating_item(cargo)) return;
    SDL_FRect rect = tile_rect(cargo.cell, camera, zoom);
    rect.x += rect.w * .12F;
    rect.y += rect.h * .23F;
    rect.w *= .76F; rect.h *= .76F;
    const LightColor light = lit_sprite_color(lighting, cargo.cell);
    SDL_Texture* texture = texture_for(graphics, Sprite::ItemFloat);
    SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
}
