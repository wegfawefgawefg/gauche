#include "input.hpp"

#include <cmath>

namespace {

bool down(const bool* keys, SDL_Scancode key) {
    return keys[static_cast<int>(key)];
}

} // namespace

Input read_local_input(const Game& game, SDL_Renderer* renderer) {
    Input input;
    const bool* keys = SDL_GetKeyboardState(nullptr);
    input.move.x = down(keys, SDL_SCANCODE_A) ? -1 : (down(keys, SDL_SCANCODE_D) ? 1 : 0);
    input.move.y = down(keys, SDL_SCANCODE_W) ? -1 : (down(keys, SDL_SCANCODE_S) ? 1 : 0);
    input.aim.x = down(keys, SDL_SCANCODE_LEFT) ? -1 : (down(keys, SDL_SCANCODE_RIGHT) ? 1 : 0);
    input.aim.y = down(keys, SDL_SCANCODE_UP) ? -1 : (down(keys, SDL_SCANCODE_DOWN) ? 1 : 0);
    input.use = input.aim.x != 0 || input.aim.y != 0 || down(keys, SDL_SCANCODE_SPACE);
    input.pickup = down(keys, SDL_SCANCODE_E);
    input.drop = down(keys, SDL_SCANCODE_Q);
    input.reload = down(keys, SDL_SCANCODE_R);
    for (int index = 0; index < quick_slots; ++index) {
        const auto code = static_cast<SDL_Scancode>(static_cast<int>(SDL_SCANCODE_1) + index);
        if (down(keys, code)) input.select = index;
    }

    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
    const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    if ((buttons & SDL_BUTTON_LMASK) != 0) {
        float render_x = 0.0F;
        float render_y = 0.0F;
        if (SDL_RenderCoordinatesFromWindow(renderer, mouse_x, mouse_y, &render_x, &render_y)) {
            const Entity* player = get_entity(game, game.players[0]);
            if (player != nullptr) {
                const int target_x = static_cast<int>(std::floor((render_x - 320.0F) / 32.0F));
                const int target_y = static_cast<int>(std::floor((render_y - 160.0F) / 32.0F));
                input.aim = {target_x, target_y};
                input.use = true;
            }
        }
    }
    return input;
}
