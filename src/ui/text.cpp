#include "text.hpp"

#include <string>
#include <cmath>

void small_ui_text(SDL_Renderer* renderer, float x, float y,
                   std::string_view value, std::uint8_t red,
                   std::uint8_t green, std::uint8_t blue) {
    float scale_x = 1.0F;
    float scale_y = 1.0F;
    SDL_GetRenderScale(renderer, &scale_x, &scale_y);
    const bool clipped = SDL_RenderClipEnabled(renderer);
    SDL_Rect clip{};
    SDL_GetRenderClipRect(renderer, &clip);
    SDL_SetRenderScale(renderer, scale_x * 0.75F, scale_y * 0.75F);
    if (clipped) {
        const SDL_Rect scaled{static_cast<int>(std::floor(static_cast<float>(clip.x) / .75F)),
            static_cast<int>(std::floor(static_cast<float>(clip.y) / .75F)),
            static_cast<int>(std::ceil(static_cast<float>(clip.w) / .75F)),
            static_cast<int>(std::ceil(static_cast<float>(clip.h) / .75F))};
        SDL_SetRenderClipRect(renderer, &scaled);
    }
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    const std::string copy{value};
    SDL_RenderDebugText(renderer, x / 0.75F, y / 0.75F, copy.c_str());
    SDL_SetRenderScale(renderer, scale_x, scale_y);
    SDL_SetRenderClipRect(renderer, clipped ? &clip : nullptr);
}
