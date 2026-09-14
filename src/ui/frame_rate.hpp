#pragma once

#include "text.hpp"

#include <cstdio>

// Frame diagnostics: use the HUD's logical coordinates, below the zoom label.
inline void draw_frame_rate(SDL_Renderer* renderer, int fps) {
    char text[24];
    if (fps <= 0) std::snprintf(text, sizeof(text), "FPS --");
    else std::snprintf(text, sizeof(text), "FPS %d", fps);
    small_ui_text(renderer, 584.0F, 19.0F, text, 158, 168, 147);
}
