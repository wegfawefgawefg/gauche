#include "display.hpp"
#include "../menu_shell.hpp"
#include "src/engine_state.hpp"
#include "src/graphics.hpp"
#include <gubsy/settings/top_level_settings.hpp>
#include <algorithm>
#include <string>

bool sync_browser_render_resolution(MenuShell& menu) {
    auto& engine = gubsy_runtime_engine(*menu.runtime);
    const auto frame = gubsy_get_frame(*menu.runtime);
    int width = 0, height = 0;
    if (!frame.window || !SDL_GetWindowSizeInPixels(frame.window, &width, &height)) return false;
    const int percent = menu.front.browser_render_percent;
    width = std::max(16, (width * percent + 50) / 100);
    height = std::max(16, (height * percent + 50) / 100);
    // The browser owns the canvas dimensions. Only the game's offscreen target
    // follows this percentage; resize/fullscreen must retain the chosen scale.
    set_top_level_setting_int(engine.top_level_game_settings, "gubsy.video.match_render_to_window", 0);
    if (frame.render_width == width && frame.render_height == height) return true;
    if (!set_render_resolution(engine, width, height)) {
        menu.front.toast = "Could not change render resolution";
        return false;
    }
    set_top_level_setting_string(engine.top_level_game_settings, "gubsy.video.render_resolution",
                                std::to_string(width) + "x" + std::to_string(height));
    menu.front.dirty = true;
    return true;
}
