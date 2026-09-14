#include "text.hpp"

#include <string>

void small_ui_text(SDL_Renderer* renderer, float x, float y,
                   std::string_view value, std::uint8_t red,
                   std::uint8_t green, std::uint8_t blue) {
    float scale_x = 1.0F;
    float scale_y = 1.0F;
    SDL_GetRenderScale(renderer, &scale_x, &scale_y);
    SDL_SetRenderScale(renderer, scale_x * 0.75F, scale_y * 0.75F);
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    const std::string copy{value};
    SDL_RenderDebugText(renderer, x / 0.75F, y / 0.75F, copy.c_str());
    SDL_SetRenderScale(renderer, scale_x, scale_y);
}
