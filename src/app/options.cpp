#include "options.hpp"
#include "../graphics.hpp"
#include <charconv>
#include <cmath>

bool wants_smoke(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        if (std::string_view{argv[index]} == "--smoke" ||
            std::string_view{argv[index]} == "--smoke-game" ||
            std::string_view{argv[index]} == "--smoke-run" ||
            std::string_view{argv[index]} == "--smoke-border" ||
            std::string_view{argv[index]} == "--smoke-reward" ||
            std::string_view{argv[index]} == "--smoke-inventory" ||
            std::string_view{argv[index]} == "--smoke-menu" ||
            std::string_view{argv[index]} == "--smoke-menu-page" ||
            std::string_view{argv[index]} == "--smoke-menu-action" ||
            std::string_view{argv[index]} == "--smoke-lobby" ||
            std::string_view{argv[index]} == "--smoke-leave") {
            return true;
        }
    }
    return false;
}

bool has_arg(int argc, char** argv, std::string_view name) {
    for (int index = 1; index < argc; ++index)
        if (std::string_view{argv[index]} == name) return true;
    return false;
}

const char* capture_arg(int argc, char** argv) {
    for (int index = 1; index + 1 < argc; ++index)
        if (std::string_view{argv[index]} == "--capture") return argv[index + 1];
    return nullptr;
}

std::string_view value_arg(int argc, char** argv, std::string_view name) {
    for (int index = 1; index + 1 < argc; ++index)
        if (std::string_view{argv[index]} == name) return argv[index + 1];
    return {};
}

std::optional<int> number_arg(std::string_view text) {
    if (text.empty()) return std::nullopt;
    int value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) return std::nullopt;
    return value;
}

std::optional<float> decimal_arg(std::string_view text) {
    if (text.empty()) return std::nullopt;
    float value = 0.0F;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size() ||
        !std::isfinite(value)) return std::nullopt;
    return value;
}

DeathPolicy requested_death_policy(int argc, char** argv) {
    const std::string_view choice = value_arg(argc, argv, "--death");
    if (choice == "no-respawn") return DeathPolicy::NoRespawn;
    if (choice == "entrance") return DeathPolicy::Entrance;
    return DeathPolicy::NextFloor;
}

std::filesystem::path user_data_root() {
    char* path = SDL_GetPrefPath("gauche", "Gauche");
    if (path == nullptr) return std::filesystem::path{GAUCHE_SOURCE_DIR} / "data";
    const std::filesystem::path result{path};
    SDL_free(path);
    return result;
}

GubsyAppConfig app_config(int argc, char** argv) {
    GubsyAppConfig config;
    config.enable_mods = false;
    config.project_root = GAUCHE_SOURCE_DIR;
    config.data_root = (user_data_root() / "gubsy").string();
    config.engine_assets_root = (asset_root() / "gubsy-engine").string();
    config.window_title = "Gauche";
    config.window_width = 1280;
    config.window_height = 720;
    config.render_width = 640;
    config.render_height = 360;
    const auto render_width = number_arg(value_arg(argc, argv, "--render-width"));
    const auto render_height = number_arg(value_arg(argc, argv, "--render-height"));
    if (render_width && render_height && *render_width > 0 && *render_height > 0) {
        config.render_width = *render_width;
        config.render_height = *render_height;
    }
    config.resizable_window = true;
    config.apply_display_settings = true;
    config.draw_fps_overlay = false;
    if (const auto title = value_arg(argc, argv, "--window-title"); !title.empty()) config.window_title = title;
    if (const auto width = number_arg(value_arg(argc, argv, "--window-width")); width && *width > 0) {
        config.window_width = *width;
        config.window_height = number_arg(value_arg(argc, argv, "--window-height")).value_or(540);
        config.apply_display_settings = false;
    }
    return config;
}
