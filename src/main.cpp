#include <SDL3/SDL.h>
#include <gubsy/runtime.hpp>

#include "graphics.hpp"
#include "game.hpp"
#include "input.hpp"
#include "render.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string_view>

namespace {

constexpr double step_seconds = 1.0 / 60.0;

bool wants_smoke(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        if (std::string_view{argv[index]} == "--smoke" ||
            std::string_view{argv[index]} == "--smoke-game") {
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

GubsyAppConfig app_config() {
    GubsyAppConfig config;
    config.enable_mods = false;
    config.project_root = GAUCHE_SOURCE_DIR;
    config.data_root = (std::filesystem::path{GAUCHE_SOURCE_DIR} / "data" / "gubsy").string();
    config.engine_assets_root = GAUCHE_GUBSY_ASSETS_DIR;
    config.window_title = "Gauche";
    config.window_width = 960;
    config.window_height = 540;
    config.render_width = 640;
    config.render_height = 360;
    config.resizable_window = true;
    config.apply_display_settings = false;
    return config;
}

} // namespace

int main(int argc, char** argv) {
    const bool smoke = wants_smoke(argc, argv);
    GubsyRuntime host;
    if (!init_gubsy_runtime(host, app_config()) || !gubsy_init_sdl_renderer(host)) {
        std::fprintf(stderr, "Gubsy host failed: %s\n", SDL_GetError());
        cleanup_gubsy_runtime(host);
        return 1;
    }

    std::string asset_error;
    const auto root = asset_root();
    Graphics graphics;
    if (!validate_assets(root, asset_error) ||
        !load_graphics(graphics, gubsy_get_frame(host).renderer, root, asset_error)) {
        std::fprintf(stderr, "%s\n", asset_error.c_str());
        cleanup_gubsy_runtime(host);
        return 1;
    }

    Game game;
    if (has_arg(argc, argv, "--smoke-game")) start_test_arena(game, 12345);
    const char* capture = capture_arg(argc, argv);
    bool captured = false;

    bool running = true;
    int frames = 0;
    std::uint64_t last_ticks = SDL_GetTicks();
    double accumulated = 0.0;
    std::uint64_t steps = 0;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            gubsy_process_sdl_event(host, event);
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN &&
                (!game.started || game.game_over)) start_test_arena(game, SDL_GetTicks() + 1);
        }

        const std::uint64_t now = SDL_GetTicks();
        const double elapsed = static_cast<double>(now - last_ticks) / 1000.0;
        last_ticks = now;
        accumulated += smoke ? step_seconds : std::min(elapsed, 0.25);
        while (accumulated >= step_seconds) {
            ++steps;
            if (game.started && !game.game_over) {
                std::array<Input, 4> inputs{};
                if (!smoke) inputs[0] = read_local_input(game, gubsy_get_frame(host).renderer);
                step_game(game, inputs);
            }
            accumulated -= step_seconds;
        }

        const GubsyFrame frame = gubsy_get_frame(host);
        if (frame.renderer == nullptr || frame.render_target == nullptr) {
            std::fprintf(stderr, "Gubsy render target unavailable\n");
            cleanup_gubsy_runtime(host);
            return 1;
        }
        SDL_SetRenderTarget(frame.renderer, frame.render_target);
        SDL_SetRenderScale(frame.renderer,
                           static_cast<float>(frame.render_width) / 640.0F,
                           static_cast<float>(frame.render_height) / 360.0F);
        SDL_SetRenderDrawColor(frame.renderer, 11, 14, 12, 255);
        SDL_RenderClear(frame.renderer);
        SDL_SetRenderDrawColor(frame.renderer, 175, 206, 164, 255);
        if (game.started) {
            render_game(frame.renderer, graphics, game);
        } else {
            SDL_FRect title_rect{24.0F, 24.0F, 64.0F, 64.0F};
            SDL_RenderTexture(frame.renderer, texture_for(graphics, Sprite::Player), nullptr, &title_rect);
            SDL_RenderDebugText(frame.renderer, 104.0F, 40.0F, "GAUCHE");
            SDL_RenderDebugText(frame.renderer, 104.0F, 60.0F, "PRESS ENTER TO START");
        }
        if (capture != nullptr && !captured) {
            SDL_Surface* surface = SDL_RenderReadPixels(frame.renderer, nullptr);
            if (surface == nullptr || !SDL_SaveBMP(surface, capture)) {
                std::fprintf(stderr, "Capture failed: %s\n", SDL_GetError());
            } else captured = true;
            SDL_DestroySurface(surface);
        }
        SDL_SetRenderTarget(frame.renderer, nullptr);
        SDL_SetRenderScale(frame.renderer, 1.0F, 1.0F);
        if (!gubsy_draw_frame_to_window(host)) {
            std::fprintf(stderr, "Gubsy present failed: %s\n", SDL_GetError());
            cleanup_gubsy_runtime(host);
            return 1;
        }
        gubsy_present_frame(host);
        ++frames;
        if (smoke && frames >= 3) {
            running = false;
        }
    }

    if (smoke) {
        std::printf("host smoke: %d frames, %llu steps, hash %016llx\n", frames,
                    static_cast<unsigned long long>(steps),
                    static_cast<unsigned long long>(game_hash(game)));
    }
    cleanup_gubsy_runtime(host);
    return 0;
}
