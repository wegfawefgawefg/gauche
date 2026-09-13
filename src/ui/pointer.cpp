#include "presentation.hpp"

void draw_pointer(SDL_Renderer* renderer, const GameGraphics& graphics,
                  const PointerState& pointer) {
    if (!pointer.inside) return;
    SDL_FRect rect{pointer.x, pointer.y, 12.0F, 12.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, Sprite::Cursor), nullptr, &rect);
}
