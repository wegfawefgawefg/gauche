#include "../src/rollback.hpp"

#include <array>
#include <cstdio>
#include <vector>

namespace {

bool check(bool condition, const char* message) {
    if (!condition) std::fprintf(stderr, "FAIL: %s\n", message);
    return condition;
}

Game two_player_game() {
    Game game;
    game.rng = 12345;
    game.stage.width = 12;
    game.stage.height = 5;
    game.stage.tiles.resize(60);
    game.players[0] = spawn_entity(game, EntityKind::Player, {2, 2});
    game.players[1] = spawn_entity(game, EntityKind::Player, {7, 2});
    get_entity(game, game.players[0])->owner = 0;
    get_entity(game, game.players[1])->owner = 1;
    spawn_entity(game, EntityKind::Zombie, {9, 3});
    game.started = true;
    return game;
}

bool correction_and_resync() {
    Game host = two_player_game();
    RollbackSession client;
    begin_rollback(client, host);
    std::vector<CanonicalFrame> canonical;
    for (int tick = 1; tick <= 80; ++tick) {
        std::array<Input, 4> inputs{};
        if (tick % 17 < 8) inputs[0].move = {1, 0};
        else inputs[0].move = {-1, 0};
        if (tick % 13 < 6) inputs[1].move = {-1, 0};
        else inputs[1].move = {1, 0};
        step_game(host, inputs);
        canonical.push_back({host.tick, inputs, game_hash(host)});
        inputs[1] = {};
        predict_frame(client, inputs);
        if (tick > 5) confirm_frame(client, canonical[static_cast<std::size_t>(tick - 6)]);
        if (!check(!client.needs_snapshot, "ordinary late input requested snapshot")) return false;
    }
    for (int tick = 75; tick < 80; ++tick)
        confirm_frame(client, canonical[static_cast<std::size_t>(tick)]);
    if (!check(client.rollback_count > 0, "late input did not trigger rollback") ||
        !check(client.confirmed_through == host.tick, "confirmation did not catch up") ||
        !check(game_hash(client.game) == game_hash(host), "replay failed to repair state"))
        return false;

    client.game.stage.at({0, 0})->kind = TileKind::Wall;
    std::array<Input, 4> neutral{};
    step_game(host, neutral);
    predict_frame(client, neutral);
    confirm_frame(client, {host.tick, neutral, game_hash(host)});
    if (!check(client.needs_snapshot, "desync was not detected")) return false;
    apply_host_snapshot(client, host);
    return check(!client.needs_snapshot && game_hash(client.game) == game_hash(host),
                 "host snapshot failed to repair desync");
}

bool bounded_history() {
    RollbackSession client;
    begin_rollback(client, two_player_game());
    client.max_history = 4;
    for (int tick = 0; tick < 6; ++tick) predict_frame(client, {});
    return check(client.needs_snapshot, "late input beyond history did not request snapshot");
}

} // namespace

int main() {
    if (!correction_and_resync() || !bounded_history()) return 1;
    std::puts("rollback rules passed");
    return 0;
}
