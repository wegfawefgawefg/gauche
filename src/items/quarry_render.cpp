#include "quarry_render.hpp"
#include "quarry_tools.hpp"
#include "../ui/text.hpp"

#include <algorithm>

void draw_brick_prepare(SDL_Renderer* renderer, const Entity& user,
    SDL_FRect rect, LightColor light) {
    if (user.kind != EntityKind::Player || user.inventory.held()->kind != ItemKind::IceBrick ||
        user.counter_a <= 0 || user.label_b != user.inventory.selected + 1) return;
    const bool ready = user.counter_a >= brick_throw_hold_ticks;
    const float center = rect.x + rect.w * .5F;
    SDL_FRect bar{center - 12, rect.y - 4, 24, 1.5F};
    SDL_SetRenderDrawColorFloat(renderer, light.red * .2F, light.green * .2F, light.blue * .2F, 1);
    SDL_RenderFillRect(renderer, &bar);
    bar.w *= std::min(1.0F, static_cast<float>(user.counter_a) / brick_throw_hold_ticks);
    SDL_SetRenderDrawColorFloat(renderer, light.red * .65F, light.green * .85F, light.blue, 1);
    SDL_RenderFillRect(renderer, &bar);
    small_ui_text(renderer, center - 12, rect.y - 11, ready ? "THROW" : "PLACE", 183, 206, 211);
}
