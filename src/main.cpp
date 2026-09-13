#include <SDL3/SDL.h>
#include <gubsy/runtime.hpp>

#include "graphics.hpp"
#include "audio.hpp"
#include "game.hpp"
#include "input.hpp"
#include "render.hpp"
#include "net_session.hpp"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace {

constexpr double step_seconds = 1.0 / 60.0;

bool wants_smoke(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        if (std::string_view{argv[index]} == "--smoke" ||
            std::string_view{argv[index]} == "--smoke-game" ||
            std::string_view{argv[index]} == "--smoke-run") {
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

DeathPolicy requested_death_policy(int argc, char** argv) {
    const std::string_view choice = value_arg(argc, argv, "--death");
    if (choice == "no-respawn") return DeathPolicy::NoRespawn;
    if (choice == "entrance") return DeathPolicy::Entrance;
    return DeathPolicy::NextFloor;
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
    GameGraphics graphics;
    if (!validate_assets(root, asset_error) ||
        !load_graphics(graphics, gubsy_get_frame(host).renderer, root, asset_error)) {
        std::fprintf(stderr, "%s\n", asset_error.c_str());
        unload_graphics(graphics);
        cleanup_gubsy_runtime(host);
        return 1;
    }

    GameAudio audio;
    if (!init_audio(audio, root, asset_error)) {
        std::fprintf(stderr, "%s\n", asset_error.c_str());
        unload_graphics(graphics);
        cleanup_gubsy_runtime(host);
        return 1;
    }

    Game game;
    NetSession network;
    const std::string_view host_port = value_arg(argc, argv, "--host");
    const std::string_view join_address = value_arg(argc, argv, "--join");
    if (!host_port.empty() && !join_address.empty()) {
        std::fprintf(stderr, "Choose either --host PORT or --join HOST:PORT\n");
        shutdown_audio(audio); unload_graphics(graphics); cleanup_gubsy_runtime(host);
        return 1;
    }
    std::string network_error;
    if (!host_port.empty()) {
        const auto port = number_arg(host_port);
        if (!port || *port <= 0 || *port > 65535 ||
            !host_game(network, static_cast<std::uint16_t>(*port), SDL_GetTicks() + 1,
                       requested_death_policy(argc, argv), network_error)) {
            std::fprintf(stderr, "Host failed: %s\n", network_error.c_str());
            shutdown_audio(audio); unload_graphics(graphics); cleanup_gubsy_runtime(host);
            return 1;
        }
    } else if (!join_address.empty()) {
        const std::size_t colon = join_address.rfind(':');
        const auto port = colon == std::string_view::npos ? std::nullopt :
            number_arg(join_address.substr(colon + 1));
        const std::string hostname = colon == std::string_view::npos ? "" :
            std::string{join_address.substr(0, colon)};
        const auto identity_path =
            (std::filesystem::path{GAUCHE_SOURCE_DIR} / "data" / "player_id").string();
        if (hostname.empty() || !port || *port <= 0 || *port > 65535 ||
            !join_game(network, hostname, static_cast<std::uint16_t>(*port),
                       load_or_create_identity(identity_path), network_error)) {
            std::fprintf(stderr, "Join failed: %s\n", network_error.c_str());
            shutdown_audio(audio); unload_graphics(graphics); cleanup_gubsy_runtime(host);
            return 1;
        }
    } else {
        if (has_arg(argc, argv, "--smoke-game")) start_test_arena(game, 12345);
        if (has_arg(argc, argv, "--smoke-run")) start_run(game, 12345);
    }
    const bool networked = network.role != NetRole::Solo;
    Game& opening_game = networked ? network.rollback.game : game;
    play_song(audio, opening_game.started ? 1 : 0);
    if (opening_game.started) {
        const Entity* listener = get_entity(opening_game, opening_game.players[0]);
        play_game_sounds(audio, opening_game, listener == nullptr ? Cell{} : listener->cell);
    }
    const char* capture = capture_arg(argc, argv);
    bool captured = false;
    const int capture_frame = number_arg(value_arg(argc, argv, "--capture-at")).value_or(0);
    const auto requested_frames = number_arg(value_arg(argc, argv, "--frames"));
    const int frame_limit = requested_frames && *requested_frames > 0 ? *requested_frames :
                            (smoke ? 3 : 0);

    bool running = true;
    int frames = 0;
    std::uint64_t last_ticks = SDL_GetTicks();
    double accumulated = 0.0;
    std::uint64_t steps = 0;
    while (running) {
        const std::uint64_t frame_begin = SDL_GetTicksNS();
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
                !networked && (!game.started || game.game_over)) {
                start_run(game, SDL_GetTicks() + 1);
                audio.played_events.fill(0);
                play_song(audio, 1);
            }
        }
        if (networked) pump_network(network);
        if (network.role == NetRole::Client && network.ready &&
            !network.rollback.needs_snapshot) {
            for (int catchup = 0; catchup < 8 &&
                 network.rollback.game.tick + 2 < network.host_tick; ++catchup)
                step_network_game(network, {});
        }

        const std::uint64_t now = SDL_GetTicks();
        const double elapsed = static_cast<double>(now - last_ticks) / 1000.0;
        last_ticks = now;
        accumulated += smoke ? step_seconds : std::min(elapsed, 0.25);
        while (accumulated >= step_seconds) {
            ++steps;
            Game& active = networked ? network.rollback.game : game;
            const int owner = networked ? network.local_owner : 0;
            const bool ready = !networked || network.ready;
            if (ready && active.started && !active.game_over) {
                std::array<Input, 4> inputs{};
                if (!smoke) inputs[static_cast<std::size_t>(owner)] =
                    read_local_input(active, gubsy_get_frame(host), owner);
                if (networked) step_network_game(network, inputs[static_cast<std::size_t>(owner)]);
                else step_game(game, inputs);
                const Entity* listener = get_entity(active,
                    active.players[static_cast<std::size_t>(owner)]);
                play_game_sounds(audio, active, listener == nullptr ? Cell{} : listener->cell);
            }
            accumulated -= step_seconds;
        }

        const GubsyFrame frame = gubsy_get_frame(host);
        if (frame.renderer == nullptr || frame.render_target == nullptr) {
            std::fprintf(stderr, "Gubsy render target unavailable\n");
            shutdown_audio(audio);
            unload_graphics(graphics);
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
        const Game& active = networked ? network.rollback.game : game;
        if (networked && network.ready && audio.current_song != 1) play_song(audio, 1);
        if (active.started && (!networked || network.ready)) {
            render_game(frame.renderer, graphics, active, networked ? network.local_owner : 0);
            if (networked) SDL_RenderDebugText(frame.renderer, 18.0F, 272.0F,
                                                network.status.c_str());
        } else {
            SDL_FRect title_rect{24.0F, 24.0F, 64.0F, 64.0F};
            SDL_RenderTexture(frame.renderer, texture_for(graphics, Sprite::Player), nullptr, &title_rect);
            SDL_RenderDebugText(frame.renderer, 104.0F, 40.0F, "GAUCHE");
            SDL_RenderDebugText(frame.renderer, 104.0F, 60.0F,
                                networked ? network.status.c_str() : "PRESS ENTER TO START");
        }
        if (capture != nullptr && !captured && frames >= capture_frame) {
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
            shutdown_audio(audio);
            unload_graphics(graphics);
            cleanup_gubsy_runtime(host);
            return 1;
        }
        gubsy_present_frame(host);
        ++frames;
        if (!smoke) {
            int cap = gubsy_configured_frame_cap_fps(host);
            if (cap <= 0) cap = 60;
            const std::uint64_t target_ns = std::uint64_t{1'000'000'000} /
                static_cast<std::uint64_t>(cap);
            const std::uint64_t frame_elapsed = SDL_GetTicksNS() - frame_begin;
            if (frame_elapsed < target_ns) SDL_DelayPrecise(target_ns - frame_elapsed);
        }
        if (frame_limit > 0 && frames >= frame_limit) {
            running = false;
        }
    }

    if (frame_limit > 0) {
        const Game& active = networked ? network.rollback.game : game;
        std::printf("host smoke: %d frames, %llu steps, hash %016llx\n", frames,
                    static_cast<unsigned long long>(steps),
                    static_cast<unsigned long long>(game_hash(active)));
        if (networked)
            std::printf("network: owner %d, ready %d, tick %llu, host tick %llu, status %s\n",
                        network.local_owner, network.ready ? 1 : 0,
                        static_cast<unsigned long long>(active.tick),
                        static_cast<unsigned long long>(network.host_tick),
                        network.status.c_str());
    }
    shutdown_audio(audio);
    unload_graphics(graphics);
    cleanup_gubsy_runtime(host);
    return 0;
}
