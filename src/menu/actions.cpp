#include "../input/icon_set.hpp"
#include "../debug/playtest.hpp"
#include "actions.hpp"
#include "profiles.hpp"
#include "audio.hpp"
#include "../menu_shell.hpp"

#include <gubsy/settings/top_level_settings.hpp>
#include <gubsy/settings/audio.hpp>
#include <gubsy/input/profile_settings.hpp>
#include "src/engine_state.hpp"
#include "src/graphics.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <string>

namespace {

std::uint16_t port_or_zero(std::string_view text) {
    int parsed = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (error != std::errc{} || end != text.data() + text.size() ||
        parsed < 1 || parsed > 65535) return 0;
    return static_cast<std::uint16_t>(parsed);
}

std::filesystem::path audio_path() {
    char* root = SDL_GetPrefPath("gauche", "Gauche");
    if (root == nullptr) return {};
    const std::filesystem::path path = std::filesystem::path{root} /
        "gubsy/settings_profiles/audio.lisp";
    SDL_free(root);
    return path;
}

void save_audio(MenuShell& menu) {
    EngineState& engine = gubsy_runtime_engine(*menu.runtime);
    engine.audio_settings.vol_master = menu.front.master_volume;
    engine.audio_settings.vol_music = menu.front.music_volume;
    engine.audio_settings.vol_sfx = menu.front.sfx_volume;
    set_top_level_setting_float(engine.top_level_game_settings,
        "gubsy.audio.master_volume", menu.front.master_volume);
    set_top_level_setting_float(engine.top_level_game_settings,
        "gubsy.audio.music_volume", menu.front.music_volume);
    set_top_level_setting_float(engine.top_level_game_settings,
        "gubsy.audio.sfx_volume", menu.front.sfx_volume);
    (void)save_top_level_game_settings(engine.top_level_game_settings);
    const auto path = audio_path();
    if (!path.empty()) (void)save_audio_settings(engine, path.string());
}


void leave_session(MenuShell& menu) {
    if (menu.rooms.active || menu.rooms.busy) { leave_room_session(menu); return; }
    if (gubsy_get_lobby_state(*menu.runtime).online) {
        std::string message;
        if (!gubsy_leave_lobby_room(*menu.runtime, message))
            menu.front.toast = message.empty() ? "Could not leave session" : message;
    }
    leave_network_game(*menu.network);
}

void back(MenuShell& menu) {
    FrontPage& page = menu.front;
    switch (page.screen) {
    case MenuScreen::Main: break;
    case MenuScreen::Rooms:
        if (menu.rooms.active || menu.rooms.busy) leave_room_session(menu);
        show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Party: leave_room_session(menu); break;
    case MenuScreen::Lobby:
        if (gubsy_get_lobby_state(*menu.runtime).online) leave_session(menu);
        show_menu_screen(page, MenuScreen::Main); break;
    case MenuScreen::DirectHost: case MenuScreen::DirectJoin:
        show_menu_screen(page, MenuScreen::NetworkOptions); break;
    case MenuScreen::Host:
        if (menu.rooms.busy) leave_room_session(menu);
        show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Rules: case MenuScreen::NetworkOptions: case MenuScreen::Join:
    case MenuScreen::Players: show_menu_screen(page, MenuScreen::Lobby); break;
    case MenuScreen::Player: show_menu_screen(page, MenuScreen::Players); break;
    case MenuScreen::Settings: show_menu_screen(page, page.settings_return); break;
    case MenuScreen::Display: case MenuScreen::Audio: case MenuScreen::Controls:
        show_menu_screen(page, MenuScreen::Settings); break;
    case MenuScreen::ProfileEditor: show_menu_screen(page, MenuScreen::Controls); break;
    case MenuScreen::Bindings: show_menu_screen(page, MenuScreen::ProfileEditor); break;
    case MenuScreen::BindDetail: show_menu_screen(page, MenuScreen::Bindings); break;
    case MenuScreen::BindChoices: show_menu_screen(page, MenuScreen::BindDetail); break;
    case MenuScreen::InputOptions: show_menu_screen(page, MenuScreen::ProfileEditor); break;
    case MenuScreen::Pause:
        menu.visible = menu.front_visible = false; break;
    case MenuScreen::Death: case MenuScreen::Victory: break;
    }
}

void set_video(MenuShell& menu, std::string_view action) {
    FrontPage& page = menu.front;
    EngineState& engine = gubsy_runtime_engine(*menu.runtime);
    const GubsyFrame frame = gubsy_get_frame(*menu.runtime);
    constexpr struct { int width; int height; const char* label; } sizes[]{
        {640, 360, "640x360"}, {960, 540, "960x540"},
        {1280, 720, "1280x720"}, {1920, 1080, "1920x1080"},
    };
    if (action == "display:render-resolution") {
        const int next = (page.render_resolution + 1) % 4;
        if (set_render_resolution(engine, sizes[next].width, sizes[next].height)) {
            page.render_resolution = next;
            set_top_level_setting_int(engine.top_level_game_settings,
                "gubsy.video.match_render_to_window", 0);
            set_top_level_setting_string(engine.top_level_game_settings,
                "gubsy.video.render_resolution", sizes[next].label);
        } else page.toast = "Could not set game resolution";
    } else if (action == "display:window-resolution") {
        const int next = (page.window_resolution + 1) % 4;
        if (set_window_dimensions(engine, sizes[next].width, sizes[next].height)) {
            page.window_resolution = next;
            set_top_level_setting_string(engine.top_level_game_settings,
                "gubsy.video.window_resolution", sizes[next].label);
        } else page.toast = "Could not resize window";
    } else if (action == "display:window-mode") {
        const int next = (page.window_mode + 1) % 3;
        if (set_window_display_mode(engine, static_cast<WindowDisplayMode>(next))) {
            page.window_mode = next;
            page.fullscreen = next == 2;
            constexpr const char* labels[]{"windowed", "borderless", "fullscreen"};
            set_top_level_setting_string(engine.top_level_game_settings,
                "gubsy.video.window_mode", labels[next]);
        } else page.toast = "Could not change window mode";
    } else if (action == "display:vsync") {
        const bool next = !page.vsync;
        if (frame.renderer && SDL_SetRenderVSync(frame.renderer, next ? 1 : 0)) {
            page.vsync = next;
            set_top_level_setting_int(engine.top_level_game_settings,
                "gubsy.video.vsync", next ? 1 : 0);
        } else page.toast = SDL_GetError();
    } else if (action == "display:frame-cap") {
        page.frame_cap = (page.frame_cap + 1) % 4;
        constexpr const char* caps[]{"0", "60", "120", "144"};
        set_top_level_setting_string(engine.top_level_game_settings,
            "gubsy.video.frame_cap", caps[page.frame_cap]);
    } else if (action == "display:show-fps") {
        page.show_fps = !page.show_fps;
        set_top_level_setting_int(engine.top_level_game_settings,
            "gubsy.video.show_fps", page.show_fps ? 1 : 0);
    }
    (void)save_top_level_game_settings(engine.top_level_game_settings);
    page.dirty = true;
}

void set_audio(MenuShell& menu, std::string_view action) {
    float* level = nullptr;
    if (action == "audio:master") level = &menu.front.master_volume;
    if (action == "audio:music") level = &menu.front.music_volume;
    if (action == "audio:sfx") level = &menu.front.sfx_volume;
    if (level == nullptr) return;
    *level = *level >= 0.99F ? 0.0F : std::min(1.0F, *level + 0.1F);
    save_audio(menu);
    menu.front.dirty = true;
}

void start_local(MenuShell& menu) {
    if (menu.network->role != NetRole::Solo) return;
    start_solo_run(*menu.solo_game, SDL_GetTicks() + 1, menu.death_policy);
    menu.solo_game->run.death_policy = menu.death_policy;
    menu.playing = true;
    menu.visible = menu.front_visible = false;
}

void pause_action(MenuShell& menu, std::string_view action) {
    if (action == "resume") {
        menu.visible = menu.front_visible = false;
    } else if (action == "restart") {
        if (menu.network->role == NetRole::Host)
            restart_host_run(*menu.network, SDL_GetTicks() + 1);
        else if (menu.network->role == NetRole::Solo) start_local(menu);
        menu.visible = menu.front_visible = false;
    } else if (action == "title") {
        leave_session(menu);
        *menu.solo_game = {};
        menu.playing = false;
        show_title_menu(menu);
    }
}

} // namespace

void initialize_menu_settings(MenuShell& menu) {
    EngineState& engine = gubsy_runtime_engine(*menu.runtime);
    FrontPage& page = menu.front;
    page.fullscreen = get_top_level_setting_string(engine.top_level_game_settings,
        "gubsy.video.window_mode", "windowed") == "fullscreen";
    const std::string mode = get_top_level_setting_string(engine.top_level_game_settings,
        "gubsy.video.window_mode", "windowed");
    page.window_mode = mode == "fullscreen" ? 2 : (mode == "borderless" ? 1 : 0);
    page.vsync = get_top_level_setting_int(engine.top_level_game_settings,
        "gubsy.video.vsync", 1) != 0;
    page.show_fps = get_top_level_setting_int(engine.top_level_game_settings,
        "gubsy.video.show_fps", 0) != 0;
    const auto resolution_index = [](int width, int height) {
        if (width >= 1920 || height >= 1080) return 3;
        if (width >= 1280 || height >= 720) return 2;
        if (width >= 960 || height >= 540) return 1;
        return 0;
    };
    const auto render = get_render_dimensions(engine);
    const auto window = get_window_dimensions(engine);
    page.render_resolution = resolution_index(render.x, render.y);
    page.window_resolution = resolution_index(window.x, window.y);
    const std::string cap = get_top_level_setting_string(engine.top_level_game_settings,
        "gubsy.video.frame_cap", "0");
    page.frame_cap = cap == "60" ? 1 : (cap == "120" ? 2 : (cap == "144" ? 3 : 0));
    page.master_volume = engine.audio_settings.vol_master;
    page.music_volume = engine.audio_settings.vol_music;
    page.sfx_volume = engine.audio_settings.vol_sfx;
    for (const BindsProfile& binds : gubsy_get_binds_profiles(*menu.runtime))
        ensure_input_pair(engine, binds);
}

void apply_menu_action(MenuShell& menu, std::string_view action) {
    if (action.empty()) return;
    if (room_action(menu, action)) { menu.front.dirty = true; return; }
    FrontPage& page = menu.front;
    if (page.screen == MenuScreen::ProfileEditor && !save_profile_name(page)) return;
    if (action == "back") { menu_back_sound(page); back(menu); return; }
    if (action == "controller-icons") {
        const auto next = static_cast<ControllerIcons>((static_cast<int>(controller_icons()) + 1) % 4);
        if (!set_controller_icons(next)) page.toast = "Could not save controller icon preference";
        page.dirty = true; return;
    }
    if (action == "join" || action == "rooms") {
        show_menu_screen(page, MenuScreen::Rooms);
        room_action(menu, "room:browse"); return;
    }
    if (action == "host") page.room_status.clear();
    if (action == "play") { show_menu_screen(page, MenuScreen::Lobby); return; }
    if (action == "quick") { start_local(menu); return; }
    if (action == "quit") { menu.quit_requested = true; return; }
    if (action == "resume" || action == "restart" || action == "title") {
        pause_action(menu, action); return;
    }
    if (action == "settings") {
        page.settings_return = page.screen;
        show_menu_screen(page, MenuScreen::Settings); return;
    }
    constexpr struct { std::string_view action; MenuScreen screen; } screens[]{
        {"players", MenuScreen::Players}, {"rules", MenuScreen::Rules},
        {"rooms", MenuScreen::Rooms}, {"party", MenuScreen::Party},
        {"host", MenuScreen::Host}, {"join", MenuScreen::Rooms},
        {"direct-host", MenuScreen::DirectHost}, {"direct-join", MenuScreen::DirectJoin},
        {"network-options", MenuScreen::NetworkOptions},
        {"display", MenuScreen::Display}, {"audio", MenuScreen::Audio},
        {"controls", MenuScreen::Controls},
    };
    for (const auto& target : screens)
        if (action == target.action) { show_menu_screen(page, target.screen); return; }
    if (action.starts_with("policy:")) {
        constexpr DeathPolicy choices[]{DeathPolicy::NextFloor, DeathPolicy::Entrance,
                                        DeathPolicy::NoRespawn};
        const int choice = action.back() - '0';
        if (choice >= 0 && choice < 3) menu.death_policy = choices[choice];
        page.dirty = true; return;
    }
    if (action == "start") {
        if (menu.rooms.active && !menu.network->match_started) {
            show_menu_screen(page, MenuScreen::Party); return;
        }
        std::string message;
        if (!gubsy_start_lobby_game(*menu.runtime, message))
            page.toast = message.empty() ? "Could not start run" : message;
        page.dirty = true; return;
    }
    if (action == "host:open") {
        const auto port = port_or_zero(page.host_port);
        std::string message;
        if (port == 0 || !gubsy_host_lobby_direct(*menu.runtime, port, message))
            page.toast = port == 0 ? "Enter a valid UDP port" : message;
        else show_menu_screen(page, MenuScreen::Lobby);
        page.dirty = true; return;
    }
    if (action == "join:connect") {
        const auto port = port_or_zero(page.join_port);
        std::string message;
        if (port == 0 || page.join_host.empty() ||
            !gubsy_join_lobby_direct(*menu.runtime, page.join_host, port, message))
            page.toast = port == 0 ? "Enter a valid UDP port" : message;
        else show_menu_screen(page, MenuScreen::Lobby);
        page.dirty = true; return;
    }
    if (action == "leave") {
        leave_session(menu); page.dirty = true; return;
    }
    if (action.starts_with("player:")) {
        const std::string value{action.substr(7)};
        if (value.starts_with("profile:")) {
            const int id = std::stoi(value.substr(8));
            (void)gubsy_set_lobby_player_binds_profile(*menu.runtime,
                page.selected_player, id);
            (void)gubsy_set_lobby_player_input_settings_profile(*menu.runtime,
                page.selected_player, id);
        } else if (value == "keyboard") {
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Keyboard, 0});
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Mouse, 0});
        } else if (value.starts_with("pad:")) {
            gubsy_toggle_lobby_player_device(*menu.runtime, page.selected_player,
                {InputSourceType::Gamepad, std::stoi(value.substr(4))});
        } else {
            page.selected_player = std::stoi(value);
            show_menu_screen(page, MenuScreen::Player);
        }
        page.dirty = true; return;
    }
    if (profile_action(page, action)) return;
    if (binding_action(page, action)) return;
    if (action == "input-options") {
        show_menu_screen(page, MenuScreen::InputOptions); return;
    }
    if (action.starts_with("display:")) { set_video(menu, action); return; }
    if (action.starts_with("audio:")) { set_audio(menu, action); return; }
}
