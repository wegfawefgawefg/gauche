#pragma once

#include "game.hpp"

// Rust rendered at 1280x720; the C++ target is half that size.
constexpr float view_width = 640.0F;
constexpr float view_height = 360.0F;
constexpr float view_center_x = view_width * 0.5F;
constexpr float view_center_y = view_height * 0.5F;
constexpr float tile_pixels(float zoom) { return 8.0F * zoom; }

struct ViewCamera {
    float x = 0.0F;
    float y = 0.0F;
    ViewCamera() = default;
    ViewCamera(float x_value, float y_value) : x(x_value), y(y_value) {}
    ViewCamera(Cell cell) : x(static_cast<float>(cell.x)),
                            y(static_cast<float>(cell.y)) {}
};

inline SDL_FRect tile_rect(Cell cell, ViewCamera camera, float zoom) {
    const float pixels = tile_pixels(zoom);
    return {view_center_x + (static_cast<float>(cell.x) - camera.x) * pixels,
            view_center_y + (static_cast<float>(cell.y) - camera.y) * pixels,
            pixels, pixels};
}
