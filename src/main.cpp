#include <SDL3/SDL.h>
#include <gubsy/runtime.hpp>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string_view>

namespace {

constexpr double step_seconds = 1.0 / 60.0;

bool wants_smoke(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        if (std::string_view{argv[index]} == "--smoke") {
            return true;
        }
    }
    return false;
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
        }

        const std::uint64_t now = SDL_GetTicks();
        const double elapsed = static_cast<double>(now - last_ticks) / 1000.0;
        last_ticks = now;
        accumulated += smoke ? step_seconds : std::min(elapsed, 0.25);
        while (accumulated >= step_seconds) {
            ++steps;
            accumulated -= step_seconds;
        }

        const GubsyFrame frame = gubsy_get_frame(host);
        if (frame.renderer == nullptr || frame.render_target == nullptr) {
            std::fprintf(stderr, "Gubsy render target unavailable\n");
            cleanup_gubsy_runtime(host);
            return 1;
        }
        SDL_SetRenderTarget(frame.renderer, frame.render_target);
        SDL_SetRenderDrawColor(frame.renderer, 11, 14, 12, 255);
        SDL_RenderClear(frame.renderer);
        SDL_SetRenderDrawColor(frame.renderer, 175, 206, 164, 255);
        SDL_RenderDebugText(frame.renderer, 24.0F, 24.0F, "GAUCHE");
        SDL_RenderDebugText(frame.renderer, 24.0F, 42.0F, "C++ PORT IN PROGRESS");
        SDL_SetRenderTarget(frame.renderer, nullptr);
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
        std::printf("host smoke: %d frames, %llu steps\n", frames,
                    static_cast<unsigned long long>(steps));
    }
    cleanup_gubsy_runtime(host);
    return 0;
}
