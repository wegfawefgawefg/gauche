#include "presentation.hpp"
#include "../input.hpp"

#include <algorithm>

void draw_pointer(tr::Renderer* renderer, const GameGraphics& graphics,
                  const PointerState& pointer) {
    if (!pointer.inside) return;
    SDL_FRect rect{pointer.x, pointer.y, 12.0F, 12.0F};
    tr::draw_texture(renderer, texture_for(graphics, Sprite::Cursor), nullptr, &rect);
}

void draw_window_pointer(tr::Renderer* renderer, SDL_Window* window, const GameGraphics& graphics) {
    if (!pointer_device_active() || SDL_GetMouseFocus() != window) return;
    int width = 0, height = 0, pixels_x = 0, pixels_y = 0;
    SDL_GetWindowSize(window, &width, &height);
    tr::output_size(renderer, &pixels_x, &pixels_y);
    if (width <= 0 || height <= 0) return;
    float x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    const float size = 12 * std::min(static_cast<float>(pixels_x) / 640,
                                     static_cast<float>(pixels_y) / 360);
    const SDL_FRect rect{x * static_cast<float>(pixels_x) / static_cast<float>(width),
                         y * static_cast<float>(pixels_y) / static_cast<float>(height), size, size};
    tr::set_viewport(renderer, nullptr);
    tr::set_scale(renderer, 1, 1);
    tr::draw_texture(renderer, texture_for(graphics, Sprite::Cursor), nullptr, &rect);
}
