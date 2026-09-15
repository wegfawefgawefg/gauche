#pragma once
#include "../game.hpp"
#include <cstdint>
#include <string>
struct MenuShell;
struct MultiplayerDebug {
    bool bot = false, published = false, auto_restart = false;
    int auto_start = 0;
    std::uint64_t started_ms = 0, seconds = 0, restart_at = 0;
    std::uint32_t random = 1;
    std::uint64_t input_tick = 0;
    Cell direction{1, 0};
    std::string room_file;
};
void init_multiplayer_debug(MultiplayerDebug& debug, MenuShell& menu, int argc, char** argv);
void update_multiplayer_debug(MultiplayerDebug& debug, MenuShell& menu);
Input multiplayer_bot_input(MultiplayerDebug& debug, const Game& game, int owner);
int run_headless_client(int argc, char** argv);
