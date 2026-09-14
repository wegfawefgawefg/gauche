#include "actions.hpp"
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
#include <optional>
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

void ensure_input_pair(EngineState& engine, const BindsProfile& binds) {
    const auto existing = std::find_if(engine.input_settings_profiles.begin(),
        engine.input_settings_profiles.end(), [&](const InputSettingsProfile& tuning) {
            return tuning.id == binds.id;
        });
    if (existing != engine.input_settings_profiles.end()) return;
    InputSettingsProfile tuning{
        .id = binds.id, .name = binds.name,
        .mouse_sensitivity = 1.0F, .mouse_invert_x = false, .mouse_invert_y = false,
        .controller_sensitivity = 1.0F, .stick_deadzone = 0.15F,
        .trigger_threshold = 0.1F, .controller_invert_x = false,
        .controller_invert_y = false, .vibration_enabled = true,
        .vibration_strength = 1.0F,
    };
    engine.input_settings_profiles.push_back(tuning);
    (void)save_input_settings_profile(tuning);
}

void leave_session(MenuShell& menu) {
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
    case MenuScreen::Lobby:
        if (gubsy_get_lobby_state(*menu.runtime).online) leave_session(menu);
        show_menu_screen(page, MenuScreen::Main); break;
    case MenuScreen::Rules: case MenuScreen::Host: case MenuScreen::Join:
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
    start_run(*menu.solo_game, SDL_GetTicks() + 1);
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
    FrontPage& page = menu.front;
    if (action == "back") { back(menu); return; }
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
        {"host", MenuScreen::Host}, {"join", MenuScreen::Join},
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
    if (action.starts_with("profile:")) {
        if (action == "profile:bindings") {
            show_menu_screen(page, MenuScreen::Bindings); return;
        }
        if (action == "profile:save") {
            const BindsProfile* source = gubsy_find_binds_profile(*menu.runtime,
                                                                   page.selected_profile);
            if (source == nullptr || page.profile_name.empty()) return;
            BindsProfile edited = *source;
            edited.name = page.profile_name;
            if (gubsy_replace_binds_profile(*menu.runtime, edited)) {
                EngineState& engine = gubsy_runtime_engine(*menu.runtime);
                for (InputSettingsProfile& tuning : engine.input_settings_profiles)
                    if (tuning.id == edited.id) {
                        tuning.name = edited.name;
                        (void)save_input_settings_profile(tuning);
                    }
                page.toast = "Profile saved";
            } else page.toast = "Name is already in use";
            page.dirty = true; return;
        }
        if (action == "profile:copy") {
            const BindsProfile* source = gubsy_find_binds_profile(*menu.runtime,
                                                                   page.selected_profile);
            if (source == nullptr) return;
            EngineState& engine = gubsy_runtime_engine(*menu.runtime);
            std::optional<InputSettingsProfile> source_tuning;
            for (const InputSettingsProfile& tuning : engine.input_settings_profiles)
                if (tuning.id == source->id) source_tuning = tuning;
            BindsProfile duplicate = *source;
            duplicate.id = 1;
            for (const BindsProfile& existing : gubsy_get_binds_profiles(*menu.runtime))
                duplicate.id = std::max(duplicate.id, existing.id + 1);
            duplicate.name = source->name + " copy " + std::to_string(duplicate.id);
            if (gubsy_replace_binds_profile(*menu.runtime, duplicate)) {
                if (source_tuning) {
                    source_tuning->id = duplicate.id;
                    source_tuning->name = duplicate.name;
                    engine.input_settings_profiles.push_back(*source_tuning);
                    (void)save_input_settings_profile(*source_tuning);
                } else ensure_input_pair(engine, duplicate);
                page.selected_profile = duplicate.id;
                page.profile_name = duplicate.name;
                show_menu_screen(page, MenuScreen::ProfileEditor);
            } else page.toast = "Could not duplicate profile";
            page.dirty = true; return;
        }
        if (action == "profile:new") {
            BindsProfile created;
            created.id = 1;
            for (const BindsProfile& existing : gubsy_get_binds_profiles(*menu.runtime))
                created.id = std::max(created.id, existing.id + 1);
            created.name = "Custom " + std::to_string(created.id);
            if (!gubsy_replace_binds_profile(*menu.runtime, created)) {
                page.toast = "Could not create control profile";
                page.dirty = true; return;
            }
            ensure_input_pair(gubsy_runtime_engine(*menu.runtime), created);
            page.selected_profile = created.id;
            page.profile_name = created.name;
            show_menu_screen(page, MenuScreen::ProfileEditor); return;
        }
        page.selected_profile = std::stoi(std::string{action.substr(8)});
        const BindsProfile* selected = gubsy_find_binds_profile(*menu.runtime,
                                                                  page.selected_profile);
        page.profile_name = selected == nullptr ? "" : selected->name;
        show_menu_screen(page, MenuScreen::ProfileEditor); return;
    }
    if (action.starts_with("bind:")) {
        if (action == "bind:add") {
            if (page.selected_bind_type == BindsActionType::Button) {
                page.capturing_bind = true;
                page.toast = "Press a key, mouse button, or gamepad button";
                page.dirty = true;
            } else show_menu_screen(page, MenuScreen::BindChoices);
            return;
        }
        const BindsProfile* source = gubsy_find_binds_profile(*menu.runtime,
                                                               page.selected_profile);
        if (action == "bind:clear" || action.starts_with("bind:remove:") ||
            action.starts_with("bind:choice:")) {
            if (source == nullptr) return;
            BindsProfile edited = *source;
            const bool clear = action == "bind:clear";
            const int mapping_index = action.starts_with("bind:remove:") ?
                std::stoi(std::string{action.substr(12)}) : -1;
            if (action.starts_with("bind:choice:")) {
                const int choice = std::stoi(std::string{action.substr(12)});
                const auto& options = binds_input_choices(page.selected_bind_type);
                if (choice < 0 || choice >= static_cast<int>(options.size())) return;
                if (page.selected_bind_type == BindsActionType::Analog1D)
                    (void)ginput::add_axis_1d_bind(edited,
                        {options[static_cast<std::size_t>(choice)].code,
                         page.selected_bind_action});
                else (void)ginput::add_axis_2d_bind(edited,
                    {options[static_cast<std::size_t>(choice)].code,
                     page.selected_bind_action});
            } else if (page.selected_bind_type == BindsActionType::Button) {
                const auto bindings = ginput::button_binds_for_action(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_button_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            } else if (page.selected_bind_type == BindsActionType::Analog1D) {
                const auto bindings = ginput::binds_for_axis_1d(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_axis_1d_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            } else {
                const auto bindings = ginput::binds_for_axis_2d(edited,
                    page.selected_bind_action);
                for (int index = 0; index < static_cast<int>(bindings.size()); ++index)
                    if (clear || index == mapping_index)
                        (void)ginput::remove_axis_2d_bind(edited,
                            bindings[static_cast<std::size_t>(index)]);
            }
            page.toast = gubsy_replace_binds_profile(*menu.runtime, edited) ?
                "Mappings saved" : "Could not save mappings";
            if (action.starts_with("bind:choice:"))
                show_menu_screen(page, MenuScreen::BindDetail);
            page.dirty = true; return;
        }
        const std::string encoded{action.substr(5)};
        const std::size_t separator = encoded.find(':');
        if (separator != std::string::npos) {
            page.selected_bind_type = static_cast<BindsActionType>(
                std::stoi(encoded.substr(0, separator)));
            page.selected_bind_action = std::stoi(encoded.substr(separator + 1));
            show_menu_screen(page, MenuScreen::BindDetail);
        }
        return;
    }
    if (action == "input-options") {
        show_menu_screen(page, MenuScreen::InputOptions); return;
    }
    if (action.starts_with("display:")) { set_video(menu, action); return; }
    if (action.starts_with("audio:")) { set_audio(menu, action); return; }
}
