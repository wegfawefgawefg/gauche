#include "multiplayer.hpp"
#include "../app/options.hpp"
#include "../menu_shell.hpp"
#include "../menu/actions.hpp"
#include "../net/party.hpp"
#include <bit>
#include <fstream>

namespace {
std::uint32_t random_value(MultiplayerDebug& debug) {
    auto& value = debug.random;
    value ^= value << 13; value ^= value >> 17; value ^= value << 5;
    return value;
}
}

// BOT: Local input generation only. All resulting actions use the normal network input path.
Input multiplayer_bot_input(MultiplayerDebug& debug, const Game& game, int owner) {
    Input input;
    if (owner < 0 || owner >= 4) return input;
    const auto* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (!player) return input;
    const auto tick = ++debug.input_tick;
    if (tick % 18 == 1) {
        constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
        debug.direction = directions[random_value(debug) % 4];
    }
    input.move = debug.direction;
    input.aim = debug.direction;
    input.use = tick % 48 < 30;
    input.pickup = tick % 43 == 0;
    input.interact = tick % 61 == 0;
    input.reload = tick % 151 == 0;
    if (tick % 97 == 0) input.select = static_cast<int>(random_value(debug) % quick_slots);
    if (game.run.phase == RunPhase::Reward || game.run.phase == RunPhase::Shop ||
        game.run.pending_count[static_cast<std::size_t>(owner)] > 0) {
        input = {};
        if (tick % 30 == 0) {
            input.select = static_cast<int>(random_value(debug) % 3);
            input.confirm = true;
        }
    }
    return input;
}

void init_multiplayer_debug(MultiplayerDebug& debug, MenuShell& menu, int argc, char** argv) {
    debug.bot = has_arg(argc, argv, "--bot");
    debug.random = static_cast<std::uint32_t>(number_arg(value_arg(argc, argv, "--bot-seed")).value_or(1));
    if (!debug.random) debug.random = 1;
    debug.started_ms = network_clock_ms();
    debug.seconds = static_cast<std::uint64_t>(std::max(0, number_arg(value_arg(argc, argv, "--seconds")).value_or(0)));
    debug.auto_start = number_arg(value_arg(argc, argv, "--auto-start")).value_or(0);
    debug.auto_restart = has_arg(argc, argv, "--auto-restart");
    debug.room_file = value_arg(argc, argv, "--room-file");
    if (const auto name = value_arg(argc, argv, "--player-name"); !name.empty()) menu.front.player_name = name;
    if (const auto url = value_arg(argc, argv, "--room-service"); !url.empty()) menu.front.room_url = url;
    menu.front.force_relay = has_arg(argc, argv, "--force-relay") || menu.front.force_relay;
    const auto room = value_arg(argc, argv, "--host-room");
    const auto code = value_arg(argc, argv, "--join-room");
    if (!room.empty() || !code.empty()) {
        menu.playing = false; menu.visible = menu.front_visible = true;
        if (!room.empty()) { menu.front.room_name = room; room_action(menu, "room:create"); }
        else { menu.front.room_code = code; room_action(menu, "room:join"); }
    }
}

void update_multiplayer_debug(MultiplayerDebug& debug, MenuShell& menu) {
    const auto now = network_clock_ms();
    if (debug.seconds && now - debug.started_ms >= debug.seconds * 1000) menu.quit_requested = true;
    if (!debug.published && menu.rooms.active && menu.rooms.host && !debug.room_file.empty()) {
        std::ofstream output(debug.room_file);
        if (output << menu.rooms.code << '\n') debug.published = true;
    }
    if (debug.bot && menu.network->ready && !menu.network->party_ready) {
        menu.network->party_ready = true; send_party_state(*menu.network);
    }
    if (debug.auto_start > 0 && menu.rooms.host && !menu.network->match_started &&
        std::popcount(menu.network->party_ready_mask) >= debug.auto_start)
        room_action(menu, "room:start");
    if (debug.auto_restart && menu.network->role == NetRole::Host && menu.network->rollback.game.game_over) {
        if (!debug.restart_at) debug.restart_at = now + 2000;
        if (now >= debug.restart_at) { apply_menu_action(menu, "restart"); debug.restart_at = 0; }
    } else debug.restart_at = 0;
}
