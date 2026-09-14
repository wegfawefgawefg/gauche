#pragma once

#include <gview/gview.hpp>
#include <gview/sdl3_renderer.hpp>
#include <gubsy/runtime.hpp>
#include <gubsy/input/binds_profile.hpp>

#include <memory>
#include <string>
#include <unordered_map>

enum class MenuScreen {
    Main, Lobby, Rules, Host, Join, Players, Player, Settings, Display, Audio,
    Controls, ProfileEditor, Bindings, BindDetail, BindChoices, InputOptions, Pause,
};

struct FrontPage {
    gview::Runtime runtime{};
    std::unique_ptr<gview::Sdl3Renderer> painter;
    std::unordered_map<std::string, SDL_Texture*> textures;
    GubsyRuntime* backend = nullptr;
    gview::InputFrame input{};
    MenuScreen screen = MenuScreen::Main;
    MenuScreen settings_return = MenuScreen::Main;
    std::string action;
    std::string toast;
    std::string join_host = "127.0.0.1";
    std::string join_port = "35355";
    std::string host_port = "35355";
    std::string profile_name;
    std::string projection;
    int selected_player = 0;
    int selected_profile = -1;
    int selected_bind_action = 0;
    BindsActionType selected_bind_type = BindsActionType::Button;
    bool capturing_bind = false;
    bool text_input_active = false;
    bool allow_restart = true;
    bool compiled = false;
    bool dirty = true;
    bool fullscreen = false;
    bool vsync = true;
    bool show_fps = false;
    int window_mode = 0;
    int render_resolution = 0;
    int window_resolution = 0;
    int frame_cap = 0;
    float master_volume = 1.0F;
    float music_volume = 1.0F;
    float sfx_volume = 1.0F;
    int width = 0;
    int height = 0;
};

bool init_front_page(FrontPage& page, GubsyRuntime& backend, SDL_Renderer* renderer);
void shutdown_front_page(FrontPage& page);
bool front_page_event(FrontPage& page, const SDL_Event& event, const GubsyFrame& frame);
std::string update_front_page(FrontPage& page, const MenuInputState& input,
                              int width, int height, int death_policy);
void render_front_page(FrontPage& page);
void show_menu_screen(FrontPage& page, MenuScreen screen);
