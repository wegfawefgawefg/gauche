#include <SDL3/SDL.h>
#include <gubsy/runtime.hpp>

#include "graphics.hpp"
#include "audio.hpp"
#include "game.hpp"
#include "input.hpp"
#include "render.hpp"
#include "net_session.hpp"
#include "menu_shell.hpp"
#include "menu/actions.hpp"
#include "particles/system.hpp"
#include "ui/interaction.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
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

GubsyAppConfig app_config() {
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
    config.resizable_window = true;
    config.apply_display_settings = true;
    return config;
}

} // namespace

int main(int argc, char** argv) {
    const bool smoke = wants_smoke(argc, argv);
    const bool border_smoke = has_arg(argc, argv, "--smoke-border");
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
    const auto audio_settings_path = user_data_root() / "gubsy" /
                                     "settings_profiles" / "audio.lisp";
    sync_audio_settings(audio, audio_settings_path);

    Game game;
    Game title_scene;
    start_run(title_scene, 380161);
    NetSession network;
    const std::string identity_path = (user_data_root() / "player_id").string();
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
        if (hostname.empty() || !port || *port <= 0 || *port > 65535 ||
            !join_game(network, hostname, static_cast<std::uint16_t>(*port),
                       load_or_create_identity(identity_path), network_error)) {
            std::fprintf(stderr, "Join failed: %s\n", network_error.c_str());
            shutdown_audio(audio); unload_graphics(graphics); cleanup_gubsy_runtime(host);
            return 1;
        }
    } else {
        if (has_arg(argc, argv, "--smoke-game")) start_test_arena(game, 12345);
        if (has_arg(argc, argv, "--smoke-run") ||
            border_smoke ||
            has_arg(argc, argv, "--smoke-reward") ||
            has_arg(argc, argv, "--smoke-inventory")) start_run(game, 12345);
        if (has_arg(argc, argv, "--smoke-reward")) finish_floor(game);
        if (border_smoke) {
            game.run.roof_lights[0] = {game.stage.width - 2, game.stage.height / 2};
            game.run.roof_light_count = 1;
        }
    }
    MenuShell menu;
    init_menu_shell(menu, host, game, network, requested_death_policy(argc, argv),
                    identity_path);
    menu.playing = network.role == NetRole::Host || game.started;
    Cosmetics cosmetics;
    InteractionUi interaction;
    if (has_arg(argc, argv, "--smoke-inventory")) {
        interaction.inventory_open = true;
        interaction.slide = 1.0F;
    }
    const bool menu_smoke = has_arg(argc, argv, "--smoke-menu");
    const bool lobby_smoke = has_arg(argc, argv, "--smoke-lobby");
    const bool leave_smoke = has_arg(argc, argv, "--smoke-leave");
    if (!menu.playing && network.role == NetRole::Solo &&
        (!smoke || menu_smoke || lobby_smoke || leave_smoke ||
         !value_arg(argc, argv, "--smoke-menu-page").empty()))
        show_title_menu(menu);
    const std::string_view menu_page = value_arg(argc, argv, "--smoke-menu-page");
    if (!menu_page.empty()) {
        constexpr struct { std::string_view name; MenuScreen screen; } pages[]{
            {"main", MenuScreen::Main}, {"lobby", MenuScreen::Lobby},
            {"rules", MenuScreen::Rules}, {"host", MenuScreen::Host},
            {"join", MenuScreen::Join}, {"players", MenuScreen::Players},
            {"settings", MenuScreen::Settings}, {"display", MenuScreen::Display},
            {"audio", MenuScreen::Audio}, {"controls", MenuScreen::Controls},
            {"profile", MenuScreen::ProfileEditor}, {"bindings", MenuScreen::Bindings},
            {"detail", MenuScreen::BindDetail},
            {"input", MenuScreen::InputOptions}, {"pause", MenuScreen::Pause},
            {"death", MenuScreen::Death}, {"victory", MenuScreen::Victory},
        };
        for (const auto& target : pages)
            if (menu_page == target.name) show_menu_screen(menu.front, target.screen);
        if (!gubsy_get_binds_profiles(host).empty()) {
            menu.front.selected_profile = gubsy_get_binds_profiles(host).front().id;
            menu.front.profile_name = gubsy_get_binds_profiles(host).front().name;
        }
    }
    Game& opening_game = network.role != NetRole::Solo ? network.rollback.game : game;
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
    bool lobby_smoke_failed = false;
    float zoom = std::clamp(decimal_arg(value_arg(argc, argv, "--zoom")).value_or(2.0F),
                            0.5F, 8.0F);
    InputReaderState input_reader{};
    int frames = 0;
    std::uint64_t last_ticks = SDL_GetTicks();
    double accumulated = 0.0;
    std::uint64_t steps = 0;
    while (running) {
        const std::uint64_t frame_begin = SDL_GetTicksNS();
        MenuInputState menu_input{};
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            const bool editing_menu_text = menu.front_visible && menu.front.text_input_active;
            const bool capturing_bind = menu.front_visible && menu.front.capturing_bind;
            gubsy_process_sdl_event(host, event);
            if (process_menu_shell_event(menu, event, gubsy_get_frame(host))) continue;
            const Game& event_game = network.role == NetRole::Solo ? game : network.rollback.game;
            if (menu.playing && !menu.visible &&
                interaction_event(interaction, event, gubsy_get_frame(host), event_game,
                                  network.role == NetRole::Solo ? 0 : network.local_owner))
                continue;
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    if (menu.visible && !capturing_bind) menu_input.back = true;
                    else if (menu.playing) open_game_menu(menu);
                    else running = false;
                }
                menu_input.up |= event.key.key == SDLK_UP ||
                                 (!editing_menu_text && event.key.key == SDLK_W);
                menu_input.down |= event.key.key == SDLK_DOWN ||
                                   (!editing_menu_text && event.key.key == SDLK_S);
                menu_input.left |= event.key.key == SDLK_LEFT ||
                                   (!editing_menu_text && event.key.key == SDLK_A);
                menu_input.right |= event.key.key == SDLK_RIGHT ||
                                    (!editing_menu_text && event.key.key == SDLK_D);
                menu_input.select |= event.key.key == SDLK_RETURN ||
                                     (!editing_menu_text && event.key.key == SDLK_SPACE);
            }
            if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
                const auto button = event.gbutton.button;
                menu_input.up |= button == SDL_GAMEPAD_BUTTON_DPAD_UP;
                menu_input.down |= button == SDL_GAMEPAD_BUTTON_DPAD_DOWN;
                menu_input.left |= button == SDL_GAMEPAD_BUTTON_DPAD_LEFT;
                menu_input.right |= button == SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
                menu_input.select |= button == SDL_GAMEPAD_BUTTON_SOUTH;
                menu_input.back |= button == SDL_GAMEPAD_BUTTON_EAST;
                if (button == SDL_GAMEPAD_BUTTON_START && menu.playing && !menu.visible)
                    open_game_menu(menu);
            }
            if (event.type == SDL_EVENT_MOUSE_WHEEL && menu.playing && !menu.visible)
                zoom = std::clamp(zoom + event.wheel.y * 0.25F, 0.5F, 8.0F);
            if (event.type == SDL_EVENT_KEY_DOWN && menu.playing && !menu.visible &&
                !event.key.repeat && (event.key.key == SDLK_MINUS ||
                                      event.key.key == SDLK_EQUALS))
                zoom = std::clamp(zoom + (event.key.key == SDLK_EQUALS ? 0.25F : -0.25F),
                                  0.5F, 8.0F);
        }
        gubsy_update_device_state(host);
        if (frames == 1 && !value_arg(argc, argv, "--smoke-menu-action").empty())
            apply_menu_action(menu, value_arg(argc, argv, "--smoke-menu-action"));
        if (menu_smoke) {
            menu_input.select = frames == 1 || frames == 3 || frames == 5;
            menu_input.down = frames == 2 || frames == 4;
        }
        if (lobby_smoke || leave_smoke) {
            menu_input.select = frames == 1;
            std::string message;
            if (frames == 2) {
                const auto port = static_cast<std::uint16_t>(40000 + SDL_GetTicks() % 10000);
                if (!gubsy_host_lobby_direct(host, port, message)) {
                    std::fprintf(stderr, "Gubsy direct host failed: %s\n", message.c_str());
                    lobby_smoke_failed = true;
                }
            }
            if (frames == 3 && !lobby_smoke_failed &&
                !gubsy_start_lobby_game(host, message)) {
                std::fprintf(stderr, "Gubsy hosted start failed: %s\n", message.c_str());
                lobby_smoke_failed = true;
            }
        }
        if (leave_smoke) {
            if (frames == 4) open_game_menu(menu);
            menu_input.down = frames == 5 || frames == 7 || frames == 9;
            menu_input.select |= frames == 11;
        }
        if (network.role != NetRole::Solo) pump_network(network);
        if (!join_address.empty() && network.role == NetRole::Client && network.ready)
            menu.playing = true;
        const GubsyFrame menu_frame = gubsy_get_frame(host);
        update_menu_shell(menu, menu_input, 1.0F / 60.0F,
                          menu_frame.render_width, menu_frame.render_height);
        if (frames % 30 == 0) sync_audio_settings(audio, audio_settings_path);
        if (menu.quit_requested) running = false;
        const bool networked = network.role != NetRole::Solo;
        if (network.role == NetRole::Client && network.ready &&
            !network.rollback.needs_snapshot) {
            for (int catchup = 0; catchup < 8 &&
                 !network.rollback.game.game_over &&
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
            const bool simulating = (menu.playing && (!menu.visible || networked)) ||
                (network.role == NetRole::Client && network.ready && network.host_tick > 0);
            if (ready && active.started && !active.game_over && simulating) {
                std::array<Input, 4> inputs{};
                if (!smoke && menu.playing && !menu.visible) {
                    Input& local = inputs[static_cast<std::size_t>(owner)];
                    local = read_local_input(host, active, gubsy_get_frame(host), owner, zoom,
                                             camera_for(cosmetics, active, owner), input_reader);
                    apply_interaction_input(interaction, active, owner, host, local);
                }
                if (networked) step_network_game(network, inputs[static_cast<std::size_t>(owner)]);
                else step_game(game, inputs);
                const Entity* listener = get_entity(active,
                    active.players[static_cast<std::size_t>(owner)]);
                play_game_sounds(audio, active, listener == nullptr ? Cell{} : listener->cell);
                update_cosmetics(cosmetics, active,
                                 listener == nullptr ? Cell{} : listener->cell, zoom);
            }
            accumulated -= step_seconds;
        }

        const Game& ended = network.role == NetRole::Solo ? game : network.rollback.game;
        if (menu.playing && !menu.visible &&
            (ended.game_over || ended.run.phase == RunPhase::Won))
            open_end_menu(menu, ended.run.phase == RunPhase::Won);

        const GubsyFrame frame = gubsy_get_frame(host);
        if (menu.playing && !menu.visible && !interaction.inventory_open &&
            !has_reward_offer(ended, networked ? network.local_owner : 0) &&
            ended.run.phase != RunPhase::Shop) SDL_HideCursor();
        else SDL_ShowCursor();
        if (frame.renderer == nullptr || frame.render_target == nullptr) {
            std::fprintf(stderr, "Gubsy render target unavailable\n");
            shutdown_audio(audio);
            unload_graphics(graphics);
            shutdown_menu_shell(menu);
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
        if (border_smoke) { cosmetics.camera = active.run.roof_lights[0]; cosmetics.camera_ready = true; }
        if (active.started && menu.playing && (!networked || network.ready) &&
            audio.current_song != 1) play_song(audio, 1);
        if (!menu.playing && audio.current_song != 0) play_song(audio, 0);
        if (active.started && (!networked || network.ready) && menu.playing) {
            render_game(frame.renderer, graphics, active, networked ? network.local_owner : 0,
                        zoom, &cosmetics,
                        read_pointer(frame, active, networked ? network.local_owner : 0,
                                     zoom, camera_for(cosmetics, active,
                                                      networked ? network.local_owner : 0)),
                        !interaction.inventory_open && !border_smoke &&
                        !has_reward_offer(active, networked ? network.local_owner : 0) &&
                        active.run.phase != RunPhase::Shop, interaction.compact_details);
            if (networked) SDL_RenderDebugText(frame.renderer, 18.0F, 272.0F,
                                                network.status.c_str());
            draw_interaction(frame.renderer, graphics, active,
                             networked ? network.local_owner : 0, interaction);
        } else if (menu.visible) {
            render_title_backdrop(frame.renderer, graphics, title_scene);
        } else if (!menu.visible) {
            SDL_FRect title_rect{24.0F, 24.0F, 64.0F, 64.0F};
            SDL_RenderTexture(frame.renderer, texture_for(graphics, Sprite::Player), nullptr, &title_rect);
            SDL_RenderDebugText(frame.renderer, 104.0F, 40.0F, "GAUCHE");
            SDL_RenderDebugText(frame.renderer, 104.0F, 60.0F,
                                networked ? network.status.c_str() : "PRESS ENTER TO START");
        }
        SDL_SetRenderScale(frame.renderer, 1.0F, 1.0F);
        render_menu_shell(menu, frame.renderer, frame.render_width, frame.render_height);
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
            shutdown_menu_shell(menu);
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
        const bool networked = network.role != NetRole::Solo;
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
    const bool menu_smoke_failed = menu_smoke && menu.death_policy != DeathPolicy::Entrance;
    if (menu_smoke_failed) std::fprintf(stderr, "Gubsy game settings did not change death policy\n");
    lobby_smoke_failed |= lobby_smoke &&
        (network.role != NetRole::Host || !menu.playing ||
         network.rollback.game.run.death_policy != requested_death_policy(argc, argv) ||
         gubsy_get_lobby_state(host).max_players != 4);
    if (lobby_smoke_failed) std::fprintf(stderr, "Gubsy direct lobby did not start the hosted run\n");
    const bool leave_smoke_failed = leave_smoke &&
        (network.role != NetRole::Solo || menu.playing || !menu.visible ||
         gubsy_get_lobby_state(host).online);
    if (leave_smoke_failed) std::fprintf(stderr, "Gubsy menu did not leave the hosted run\n");
    const bool page_smoke_failed = !menu_page.empty() &&
        (!menu.front.compiled || !menu.front_visible || menu.quit_requested);
    if (page_smoke_failed) std::fprintf(stderr, "GView menu page did not render\n");
    shutdown_audio(audio);
    unload_graphics(graphics);
    shutdown_menu_shell(menu);
    cleanup_gubsy_runtime(host);
    return menu_smoke_failed || lobby_smoke_failed || leave_smoke_failed ||
           page_smoke_failed ? 1 : 0;
}
