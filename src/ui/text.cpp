#include "text.hpp"

#include <string>
#include <cmath>

void small_ui_text(tr::Renderer* renderer, float x, float y,
                   std::string_view value, std::uint8_t red,
                   std::uint8_t green, std::uint8_t blue) {
    float scale_x = 1.0F;
    float scale_y = 1.0F;
    tr::get_scale(renderer, &scale_x, &scale_y);
    const bool clipped = tr::clip_enabled(renderer);
    SDL_Rect clip{};
    tr::get_clip(renderer, &clip);
    tr::set_scale(renderer, scale_x * 0.75F, scale_y * 0.75F);
    if (clipped) {
        const SDL_Rect scaled{static_cast<int>(std::floor(static_cast<float>(clip.x) / .75F)),
            static_cast<int>(std::floor(static_cast<float>(clip.y) / .75F)),
            static_cast<int>(std::ceil(static_cast<float>(clip.w) / .75F)),
            static_cast<int>(std::ceil(static_cast<float>(clip.h) / .75F))};
        tr::set_clip(renderer, &scaled);
    }
    tr::set_color_bytes(renderer, red, green, blue, 255);
    const std::string copy{value};
    tr::debug_text(renderer, x / 0.75F, y / 0.75F, copy.c_str());
    tr::set_scale(renderer, scale_x, scale_y);
    tr::set_clip(renderer, clipped ? &clip : nullptr);
}
