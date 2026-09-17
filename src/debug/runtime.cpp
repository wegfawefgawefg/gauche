#include "playtest.hpp"
#include "levels.hpp"
#include "../menu_shell.hpp"
#include <algorithm>

void jump_to_test_level(Game& game, int selection) {
    const auto& level = test_levels[static_cast<std::size_t>(std::clamp(selection, 0, static_cast<int>(test_levels.size())-1))];
    game.run.floor = level.floor;
    game.game_over = false;
    // JUMP: Restore dead party members so No Respawn cannot leave an empty debug level.
    for (Handle handle : controlled_entities(game))
        if (Entity* player = get_entity(game, handle)) player->health = player->max_health;
    for (auto& [id, member] : game.players) { member.chosen = false; member.shop_ready = false; member.pending_count = 0; member.pending_offers = {}; member.offers = {}; }
    game.run.roof_lights = {}; game.run.roof_light_count = 0;
    generate_world_floor(game, level.layout);
    if (playtest_tools().override_loadout)
        for (const auto& [owner, participant] : game.players) apply_test_loadout(game, owner);
    ++playtest_tools().revision;
}

// OFFLINE ENTRY: Debug preferences never enter host/client simulation or rollback.
void start_solo_run(Game& game, std::uint64_t seed, DeathPolicy policy) {
    start_run(game, seed);
    game.run.death_policy = policy;
    auto& tools = playtest_tools();
    if (tools.override_start) jump_to_test_level(game, tools.starting_level);
    else {
        if (tools.override_loadout) apply_test_loadout(game, 0);
        ++tools.revision;
    }
}

void step_solo_game(Game& game, const PlayerInputs& inputs) {
    const int floor = game.run.floor;
    std::map<PlayerId, bool> alive;
    for (const auto& [owner, participant] : game.players) {
        const Entity* player = get_entity(game, player_state(game, owner).controlled);
        alive[owner] = player && player->health > 0;
    }
    step_game(game, inputs);
    auto& tools = playtest_tools();
    // REPEAT: Reward/shop choices finish normally before the next map loops back.
    if (tools.repeat && (game.run.floor != floor || game.run.phase == RunPhase::Won)) {
        jump_to_test_level(game, tools.repeat_level);
        return;
    }
    if (game.run.floor != floor) ++tools.revision;
    if (tools.override_loadout)
        for (const auto& [owner, participant] : game.players)
            if (!alive[static_cast<std::size_t>(owner)]) apply_test_loadout(game, owner);
}

void process_playtest_requests(MenuShell& menu) {
    auto& tools = playtest_tools();
    if (menu.network->role == NetRole::Solo) {
        if (tools.jump_requested) {
            if (!menu.solo_game->started) start_run(*menu.solo_game, SDL_GetTicks() + 1);
            menu.solo_game->run.death_policy = menu.death_policy;
            jump_to_test_level(*menu.solo_game, tools.selected_level);
            menu.playing = true;
            menu.visible = menu.front_visible = false;
        }
        if (tools.equip_requested) apply_test_loadout(*menu.solo_game, 0);
    }
    tools.jump_requested = tools.equip_requested = false;
}

std::string playtest_summary() {
    const auto& tools = playtest_tools();
    std::string summary;
    if (tools.override_start) summary = std::string{"Start: "} + test_levels[static_cast<std::size_t>(tools.starting_level)].name;
    if (tools.repeat) summary += (summary.empty() ? "" : " / ") + std::string{"Repeat: "} + test_levels[static_cast<std::size_t>(tools.repeat_level)].name;
    if (tools.override_loadout) summary += std::string{summary.empty() ? "" : " / "} + "Spawn kit";
    return summary.empty() ? "" : "Offline playtest overrides [F1]: " + summary;
}
