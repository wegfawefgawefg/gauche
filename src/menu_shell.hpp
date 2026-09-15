#pragma once

#include "game.hpp"
#include "net_session.hpp"
#include "net/rooms/runtime.hpp"
#include "menu/front_page.hpp"

#include <gubsy/runtime.hpp>

#include <string>

struct MenuShell {
    GubsyRuntime* runtime = nullptr;
    Game* solo_game = nullptr;
    NetSession* network = nullptr;
    DeathPolicy death_policy = DeathPolicy::NextFloor;
    std::string identity_path;
    bool visible = false;
    bool playing = false;
    bool quit_requested = false;
    bool lobby_configured = false;
    bool front_visible = false;
    FrontPage front{};
    RoomRuntime rooms{};
};

void init_menu_shell(MenuShell& menu, GubsyRuntime& runtime, Game& game,
                     NetSession& network, DeathPolicy policy,
                     const std::string& identity_path);
void show_title_menu(MenuShell& menu);
void open_game_menu(MenuShell& menu);
void open_end_menu(MenuShell& menu, bool victory);
void update_menu_shell(MenuShell& menu, MenuInputState input, float dt,
                       int width, int height);
void render_menu_shell(MenuShell& menu, SDL_Renderer* renderer, int width, int height);
bool process_menu_shell_event(MenuShell& menu, const SDL_Event& event,
                              const GubsyFrame& frame);
void shutdown_menu_shell(MenuShell& menu);
