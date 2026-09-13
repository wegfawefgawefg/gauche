#include "input.hpp"

#include <cmath>
#include <algorithm>

namespace {

bool down(const bool* keys, SDL_Scancode key) {
    return keys[static_cast<int>(key)];
}

} // namespace

Input read_local_input(const Game& game, const GubsyFrame& frame) {
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
    input.interact = down(keys, SDL_SCANCODE_F) || down(keys, SDL_SCANCODE_E);
    input.confirm = down(keys, SDL_SCANCODE_RETURN);
    for (int index = 0; index < quick_slots; ++index) {
        const auto code = static_cast<SDL_Scancode>(static_cast<int>(SDL_SCANCODE_1) + index);
        if (down(keys, code)) input.select = index;
    }

    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
    const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    if ((buttons & SDL_BUTTON_LMASK) != 0) {
        int window_w = 0;
        int window_h = 0;
        SDL_GetWindowSize(frame.window, &window_w, &window_h);
        if (window_w > 0 && window_h > 0 && frame.render_width > 0 && frame.render_height > 0) {
            const float scale = std::min(static_cast<float>(window_w) /
                                             static_cast<float>(frame.render_width),
                                         static_cast<float>(window_h) /
                                             static_cast<float>(frame.render_height));
            const float left = (static_cast<float>(window_w) -
                                static_cast<float>(frame.render_width) * scale) * 0.5F;
            const float top = (static_cast<float>(window_h) -
                               static_cast<float>(frame.render_height) * scale) * 0.5F;
            const float render_x = (mouse_x - left) / scale;
            const float render_y = (mouse_y - top) / scale;
            const Entity* player = get_entity(game, game.players[0]);
            if (player != nullptr && render_x >= 0.0F && render_y >= 0.0F &&
                render_x < static_cast<float>(frame.render_width) &&
                render_y < static_cast<float>(frame.render_height)) {
                const float logical_x = render_x * 640.0F / static_cast<float>(frame.render_width);
                const float logical_y = render_y * 360.0F / static_cast<float>(frame.render_height);
                const int target_x = static_cast<int>(std::floor((logical_x - 320.0F) / 32.0F));
                const int target_y = static_cast<int>(std::floor((logical_y - 160.0F) / 32.0F));
                input.aim = {target_x, target_y};
                input.use = true;
            }
        }
    }
    return input;
}
