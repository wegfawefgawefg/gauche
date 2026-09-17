#pragma once
#include "../game.hpp"
#include <filesystem>
#include <string>

struct MenuShell;
struct TestLoadout {
    Inventory inventory{};
    std::uint32_t artifacts = 0;
    int health = 100;
    int step_ticks = 7;
    int gold = 0;
};
struct PlaytestTools {
    bool levels = false, loadouts = false, pause = true;
    int selected_level = 0, starting_level = 0, repeat_level = 0;
    bool override_start = false, repeat = false, override_loadout = false;
    int preset = 0, selected_slot = 0;
    TestLoadout loadout{};
    bool jump_requested = false, equip_requested = false;
    unsigned int revision = 0;
    std::string save_error;
    std::filesystem::path path;
};
PlaytestTools& playtest_tools();
void init_playtest_tools(const std::filesystem::path& path);
void save_playtest_tools();
void normalize_test_item(Item& item);
void set_loadout_preset(TestLoadout& loadout, int preset);
void apply_test_loadout(Game& game, int owner);
void start_solo_run(Game& game, std::uint64_t seed, DeathPolicy policy);
void step_solo_game(Game& game, const PlayerInputs& inputs);
void process_playtest_requests(MenuShell& menu);
void draw_playtest_tools(const Game& game, bool offline);
void draw_level_editor(bool offline);
void draw_loadout_editor(const Game& game, bool offline);
