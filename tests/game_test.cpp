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
    if (!check(get_entity(teammates, pushed)->health == 40,
               "teammate incorrectly became a crush surface")) return false;

    Game item_game = small_game();
    get_entity(item_game, item_game.players[0])->inventory.slots[0] =
        make_item(ItemKind::Buckler);
    *item_game.stage.at({4, 2}) = {TileKind::Wall, 100, 0};
    const Handle item = spawn_entity(item_game, EntityKind::GroundItem, {3, 2});
    get_entity(item_game, item)->ground_item = make_item(ItemKind::Bandage);
    use_held_item(item_game, item_game.players[0].slot, {3, 2});
    return check(get_entity(item_game, item) == nullptr,
                 "item shoved into a wall did not break");
}

bool artifact_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::AllPiercing);
    player->inventory.slots[0] = make_item(ItemKind::Pistol);
    *game.stage.at({3, 2}) = {TileKind::Wall, 100, 0};
    const Handle first = spawn_entity(game, EntityKind::Zombie, {4, 2});
    const Handle second = spawn_entity(game, EntityKind::Zombie, {5, 2});
    if (!check(use_held_item(game, game.players[0].slot, {6, 2}),
               "piercing shot failed")) return false;
    if (!check(get_entity(game, first)->health == 24 &&
               get_entity(game, second)->health == 24 &&
               game.stage.at({3, 2})->hp == 50,
               "all piercing failed through wall and actors")) return false;
    remove_entity(game, first);
    remove_entity(game, second);
    player->artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::Hearth);
    player->health = 70;
    const Handle friend_handle = spawn_entity(game, EntityKind::Player, {2, 3});
    get_entity(game, friend_handle)->health = 80;
    game.players[1] = friend_handle;
    for (int tick = 0; tick < 60; ++tick) step_game(game, {});
    if (!check(player->health == 71 && get_entity(game, friend_handle)->health == 81,
               "hearth did not heal nearby party members")) return false;
    Game reflection = small_game();
    Entity* defender = get_entity(reflection, reflection.players[0]);
    defender->artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::Reflector);
    const Handle attacker = spawn_entity(reflection, EntityKind::Zombie, {3, 2});
    for (std::uint64_t seed = 1; seed < 100; ++seed) {
        Game probe;
        probe.rng = seed;
        if (random_u32(probe) % 4 == 0) { reflection.rng = seed; break; }
    }
    damage_entity(reflection, reflection.players[0].slot, 10, {3, 2});
    return check(defender->health == 90 && get_entity(reflection, attacker)->health == 35,
                 "reflector did not return a deterministic hit");
}

bool status_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->inventory.slots[0] = make_item(ItemKind::SleepMeds, 2);
    const Handle wolf = spawn_entity(game, EntityKind::Wolf, {4, 2});
    if (!check(use_held_item(game, game.players[0].slot, {4, 2}),
               "sleep meds failed to sedate target")) return false;
    const Cell original = get_entity(game, wolf)->cell;
    for (int tick = 0; tick < 60; ++tick) step_game(game, {});
    if (!check(get_entity(game, wolf)->cell == original &&
               get_entity(game, wolf)->sleep_ticks == 120,
               "sleeping enemy moved or timer drifted")) return false;
    damage_entity(game, wolf.slot, 1, player->cell);
    if (!check(get_entity(game, wolf)->sleep_ticks == 0,
               "damage did not wake sleeper")) return false;
    player->burn_ticks = 60;
    const int health = player->health;
    for (int tick = 0; tick < 30; ++tick) step_game(game, {});
    return check(player->health <= health - 4, "burn did not deal periodic damage");
}

bool equipment_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->inventory.slots[0] = make_item(ItemKind::Shotgun);
    player->inventory.slots[1] = make_item(ItemKind::SMG);
    player->inventory.slots[2] = make_item(ItemKind::Ammo);
    player->inventory.selected = 0;
    if (!check(use_held_item(game, game.players[0].slot, {3, 2}),
               "shotgun failed to fire")) return false;
    if (!check(player->inventory.slots[0].loaded == 5 &&
               player->inventory.slots[1].loaded == 30,
               "weapons shared a magazine")) return false;
    player->inventory.selected = 2;
    use_held_item(game, game.players[0].slot, player->cell);
    if (!check(player->inventory.slots[0].spare == 48 &&
               player->inventory.slots[1].spare == 210,
               "ammo did not refill each gun independently")) return false;

    Game trap_game = small_game();
    Entity* trapper = get_entity(trap_game, trap_game.players[0]);
    trapper->inventory.slots[0] = make_item(ItemKind::BearTrap);
    if (!check(use_held_item(trap_game, trap_game.players[0].slot, {3, 2}),
               "bear trap placement failed")) return false;
    const Handle wolf = spawn_entity(trap_game, EntityKind::Wolf, {4, 2});
    move_entity(trap_game, wolf.slot, {3, 2});
    step_game(trap_game, {});
    return check(get_entity(trap_game, wolf)->health == 27 &&
                 get_entity(trap_game, wolf)->stun_ticks > 0,
                 "placed bear trap did not catch a moving actor");
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
        !artifact_rules() || !status_rules() || !equipment_rules() ||
        !track_before_train() || !forest_progression())
        return 1;
    std::puts("game rules passed");
    return 0;
}
