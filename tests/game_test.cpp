#include "../src/game.hpp"

#include <array>
#include <cstdio>

namespace {

bool check(bool condition, const char* message) {
    if (!condition) std::fprintf(stderr, "FAIL: %s\n", message);
    return condition;
}

Game small_game() {
    Game game;
    game.stage.width = 8;
    game.stage.height = 5;
    game.stage.tiles.resize(40);
    game.players[0] = spawn_entity(game, EntityKind::Player, {2, 2});
    game.started = true;
    return game;
}

bool deterministic_replay() {
    Game first;
    Game second;
    start_test_arena(first, 7654321);
    start_test_arena(second, 7654321);
    for (int tick = 0; tick < 240; ++tick) {
        std::array<Input, 4> inputs{};
        if (tick % 20 < 10) inputs[0].move = {1, 0};
        else inputs[0].move = {0, 1};
        if (tick == 50) inputs[0].select = 1;
        if (tick % 12 == 0) {
            inputs[0].aim = {1, 0};
            inputs[0].use = true;
        }
        step_game(first, inputs);
        step_game(second, inputs);
        if (game_hash(first) != game_hash(second)) return check(false, "replay diverged");
    }
    return true;
}

bool handle_reuse() {
    Game game = small_game();
    const Handle old = spawn_entity(game, EntityKind::Zombie, {3, 2});
    remove_entity(game, old);
    const Handle fresh = spawn_entity(game, EntityKind::Chicken, {3, 2});
    return check(fresh.slot == old.slot, "slot did not recycle") &&
           check(fresh.generation != old.generation, "generation did not advance") &&
           check(get_entity(game, old) == nullptr, "stale handle resolved") &&
           check(get_entity(game, fresh) != nullptr, "fresh handle missing");
}

bool buckler_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->inventory.slots[0] = make_item(ItemKind::Buckler);
    *game.stage.at({4, 2}) = {TileKind::Wall, 100, 0};
    const Handle zombie = spawn_entity(game, EntityKind::Zombie, {3, 2});
    const bool shoved = use_held_item(game, game.players[0].slot, {3, 2});
    if (!check(shoved && get_entity(game, zombie)->health == 0,
               "wall did not crush shoved actor")) return false;
    damage_entity(game, game.players[0].slot, 5, {3, 2});
    if (!check(player->health == 100 && player->inventory.held()->durability == 25,
               "front block did not absorb hit and lose durability")) return false;
    for (int hit = 0; hit < 5; ++hit) damage_entity(game, game.players[0].slot, 5, {3, 2});
    if (!check(player->inventory.held()->kind == ItemKind::None, "buckler did not break")) return false;

    Game teammates = small_game();
    Entity* blocker = get_entity(teammates, teammates.players[0]);
    blocker->inventory.slots[0] = make_item(ItemKind::Buckler);
    const Handle pushed = spawn_entity(teammates, EntityKind::Zombie, {3, 2});
    spawn_entity(teammates, EntityKind::Player, {4, 2});
    use_held_item(teammates, teammates.players[0].slot, {3, 2});
    return check(get_entity(teammates, pushed)->health == 40,
                 "teammate incorrectly became a crush surface");
}

bool track_before_train() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->inventory.slots[0] = make_item(ItemKind::ConductorHat);
    *game.stage.at({5, 2}) = {TileKind::Wall, 100, 0};
    if (!check(use_held_item(game, game.players[0].slot, player->cell),
               "hat failed to start rail layer")) return false;
    for (int tick = 0; tick < 9; ++tick) step_game(game, {});
    bool train = false;
    for (const Entity& entity : game.entities) train |= entity.kind == EntityKind::Train;
    return check(game.stage.at({5, 2})->kind == TileKind::Rail,
                 "rail did not cut through wall") &&
           check(train, "train did not follow completed track");
}

bool forest_progression() {
    for (std::uint64_t seed = 1; seed <= 64; ++seed) {
        Game game;
        start_run(game, seed);
        if (!check(floor_reachable(game), "generated forest floor is locked")) return false;
        for (int floor = 1; floor <= 12; ++floor) {
            if (!check(game.run.floor == floor, "floor number drifted")) return false;
            if (!check(floor_reachable(game), "generated floor is locked")) return false;
            if (floor > 4) {
                const TileKind expected = floor <= 8 ? TileKind::Lava : TileKind::Ice;
                bool themed = false;
                for (const Tile& tile : game.stage.tiles) themed |= tile.kind == expected;
                if (!check(themed, "world lacks its terrain")) return false;
            }
            finish_floor(game);
            choose_reward(game, 0, 1);
            if (game.run.phase == RunPhase::Reward) {
                Entity* player = get_entity(game, game.players[0]);
                player->inventory.slots[0] = {};
                choose_reward(game, 0, 1);
            }
            if (game.run.phase == RunPhase::Shop) {
                game.run.shop_ready[0] = true;
                advance_run(game);
            }
        }
        if (!check(game.run.phase == RunPhase::Won, "twelve-floor run did not clear"))
            return false;
    }
    return true;
}

} // namespace

int main() {
    if (!deterministic_replay() || !handle_reuse() || !buckler_rules() ||
        !track_before_train() || !forest_progression())
        return 1;
    std::puts("game rules passed");
    return 0;
}
