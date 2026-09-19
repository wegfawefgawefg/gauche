#include "settings_page.hpp"
#include "page_chrome.hpp"
#include "settings.hpp"
#include "front_page.hpp"
#include <algorithm>

namespace teeming_menu {
namespace {
void choose(ViewBuilder& ui, const FrontPage& page, const char* id, const char* label,
            std::vector<std::string> choices) {
    const std::string key = std::string("setting:") + id;
    if (const auto current = read_menu_setting(page, key))
        if (const auto* text = std::get_if<std::string>(&*current))
            if (std::find(choices.begin(), choices.end(), *text) == choices.end()) choices.push_back(*text);
    ui.select("card", id, label, key, std::move(choices), "menu", 48);
}
void toggle(ViewBuilder& ui, const char* id, const char* label) {
    ui.toggle("card", id, label, std::string("setting:") + id, "menu", 48);
}
}
void settings_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Settings", 700, 400);
    button(ui, "display", "Display", "display");
    button(ui, "audio", "Audio", "audio");
    button(ui, "controls", "Controls", "controls");
    choose(ui, page, "icons", "Controller icons", {"Auto", "Xbox", "PlayStation", "Nintendo"});
    footer(ui, "display");
}
void display_page(ViewBuilder& ui, const FrontPage& page) {
    frame(ui, "Display Settings", 820, 570);
#ifdef __EMSCRIPTEN__
    button(ui, "window-mode", page.fullscreen ? "Exit fullscreen" : "Enter fullscreen", "display:window-mode");
    choose(ui, page, "render-scale", "Render resolution", {"100%", "75%", "50%"});
    choose(ui, page, "frame-limit", "Frame limit", {"Display refresh rate", "30 FPS", "60 FPS", "90 FPS", "120 FPS", "144 FPS", "165 FPS", "240 FPS"});
    toggle(ui, "show-fps", "Show FPS");
    toggle(ui, "auto-reports", "Automatic error reports");
    button(ui, "save-report", "Save debug log", "display:save-report");
    ui.label("card", "reports-help", "Technical diagnostics; no player names or room codes.", 26, 14);
    footer(ui, "window-mode");
#else
    choose(ui, page, "window-mode", "Window mode", {"Windowed", "Borderless", "Fullscreen"});
    choose(ui, page, "window-resolution", "Window size", {"640x360", "960x540", "1280x720", "1600x900", "1920x1080", "2560x1440", "3840x2160"});
    ui.spec("window-resolution").enabled = page.window_mode == 0;
    choose(ui, page, "render-resolution", "Render resolution", {"Match window", "640x360", "960x540", "1280x720", "1600x900", "1920x1080", "2560x1440", "3840x2160"});
    toggle(ui, "vsync", "V-sync");
    choose(ui, page, "frame-limit", "Frame limit", {"Unlimited", "30 FPS", "60 FPS", "90 FPS", "120 FPS", "144 FPS", "165 FPS", "240 FPS"});
    toggle(ui, "show-fps", "Show FPS");
    footer(ui, "window-mode");
#endif
}
void audio_page(ViewBuilder& ui, const FrontPage&) {
    frame(ui, "Audio Settings", 740, 430);
    ui.slider("card", "master", "Master volume (%)", "setting:master", "menu", 0, 100, 1, 72);
    ui.slider("card", "music", "Music (%)", "setting:music", "menu", 0, 100, 1, 72);
    ui.slider("card", "sfx", "Sound effects (%)", "setting:sfx", "menu", 0, 100, 1, 72);
    footer(ui, "master");
}
} // namespace teeming_menu
