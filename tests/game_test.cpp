#include "../src/game.hpp"

#include <algorithm>
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
    if (!check(get_entity(item_game, item) == nullptr,
               "item shoved into a wall did not break")) return false;

    Game hard = small_game();
    get_entity(hard, hard.players[0])->inventory.slots[0] = make_item(ItemKind::Buckler);
    const Handle den = spawn_entity(hard, EntityKind::Den, {3, 2});
    use_held_item(hard, hard.players[0].slot, {3, 2});
    return check(get_entity(hard, den)->cell == Cell{3, 2},
                 "buckler shoved a hard fixture");
}

bool artifact_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::AllPiercing);
    player->inventory.slots[0] = make_item(ItemKind::Pistol);
    *game.stage.at({5, 2}) = {TileKind::Wall, 100, 0};
    const Handle first = spawn_entity(game, EntityKind::Zombie, {3, 2});
    const Handle second = spawn_entity(game, EntityKind::Zombie, {4, 2});
    const Handle behind_wall = spawn_entity(game, EntityKind::Zombie, {6, 2});
    if (!check(use_held_item(game, game.players[0].slot, {6, 2}),
               "piercing shot failed")) return false;
    if (!check(get_entity(game, first)->health == 24 &&
               get_entity(game, second)->health == 24 &&
               get_entity(game, behind_wall)->health == 40 &&
               game.stage.at({5, 2})->hp == 100,
               "all piercing did not stop at solid terrain")) return false;
    remove_entity(game, first);
    remove_entity(game, second);
    remove_entity(game, behind_wall);
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

bool offline_reward_rules() {
    Game game;
    start_run(game, 4888);
    game.run.online[1] = true;
    game.players[1] = spawn_entity(game, EntityKind::Player, game.run.spawn + Cell{1, 0});
    get_entity(game, game.players[1])->owner = 1;
    finish_floor(game);
    int safe_choice = -1;
    for (int index = 0; index < 3; ++index) {
        const Reward reward = game.run.offers[1][static_cast<std::size_t>(index)];
        if (reward.kind == RewardKind::Health || reward.kind == RewardKind::Speed)
            safe_choice = index;
    }
    if (!check(safe_choice >= 0, "reward offer lacks a non-item choice")) return false;
    for (int index = 0; index < 3; ++index) {
        const Reward reward = game.run.offers[0][static_cast<std::size_t>(index)];
        if (reward.kind == RewardKind::Health || reward.kind == RewardKind::Speed) {
            choose_reward(game, 0, index);
            break;
        }
    }
    if (!check(game.run.phase == RunPhase::Reward,
               "online teammate did not hold reward screen")) return false;
    game.run.online[1] = false;
    advance_run(game);
    if (!check(game.run.floor == 2 && game.run.pending_count[1] == 1 &&
               game.run.phase == RunPhase::Playing,
               "offline teammate blocked floor or lost reward")) return false;
    game.run.online[1] = true;
    const Reward pending = game.run.pending_offers[1][0][static_cast<std::size_t>(safe_choice)];
    Entity* player = get_entity(game, game.players[1]);
    const int before = pending.kind == RewardKind::Health ? player->max_health : player->move_interval;
    choose_pending_reward(game, 1, safe_choice);
    return check(game.run.pending_count[1] == 0 &&
                 (pending.kind == RewardKind::Health ?
                  player->max_health == before + pending.amount :
                  player->move_interval == std::max(3, before - pending.amount)),
                 "reconnected player could not claim missed reward");
}

bool entrance_respawn_rules() {
    Game game = small_game();
    game.run.phase = RunPhase::Playing;
    game.run.death_policy = DeathPolicy::Entrance;
    game.run.spawn = {2, 2};
    game.run.online[0] = true;
    get_entity(game, game.players[0])->owner = 0;
    damage_entity(game, game.players[0].slot, 1000, {3, 2});
    Entity* player = get_entity(game, game.players[0]);
    player->spawn_wait = 1;
    const Handle blocker = spawn_entity(game, EntityKind::Spawner, game.run.spawn);
    step_game(game, {});
    return check(player->health == player->max_health && player->impassable &&
                 player->cell != get_entity(game, blocker)->cell &&
                 walkable(game.stage.at(player->cell)->kind),
                 "entrance respawn overlapped a hard blocker");
}

bool held_item_direction() {
    Game game = small_game();
    game.run.online[0] = true;
    Entity* player = get_entity(game, game.players[0]);
    player->owner = 0;
    player->inventory.held()->cooldown = 20;
    std::array<Input, 4> inputs{};
    inputs[0].aim = {12, 3};
    inputs[0].use = true;
    step_game(game, inputs);
    if (!check(player->facing == Cell{1, 0},
               "held item direction escaped its tile when use was cooling down")) return false;
    Game trigger = small_game();
    trigger.run.online[0] = true;
    Entity* actor = get_entity(trigger, trigger.players[0]);
    actor->owner = 0;
    actor->facing = {0, -1};
    std::array<Input, 4> press{};
    press[0].use = true;
    step_game(trigger, press);
    return check(trigger.stage.at({2, 1})->kind == TileKind::Wall,
                 "trigger with a neutral aim stick did not use facing");
}
bool switch_route() {
    Game game;
    start_run(game, 7171);
    finish_floor(game);
    game.run.chosen.fill(true);
    advance_run(game);
    if (!check(game.run.floor == 2 && game.run.objective == ObjectiveKind::Switch,
               "second floor did not use a switch route")) return false;
    Entity* player = get_entity(game, game.players[0]);
    Cell switch_cell{-1, -1};
    Cell door_cell{-1, -1};
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::Switch) switch_cell = entity.cell;
        if (entity.kind == EntityKind::Door) door_cell = entity.cell;
    }
    if (!check(game.stage.in_bounds(switch_cell) && game.stage.in_bounds(door_cell),
               "switch route lacks fixtures")) return false;
    player->cell = switch_cell + Cell{1, 0};
    if (!check(interact_with_fixture(game, 0, switch_cell) && game.run.has_key,
               "switch did not unlock route")) return false;
    player->cell = door_cell + Cell{-1, 0};
    if (!check(interact_with_fixture(game, 0, door_cell),
               "unlocked door did not open")) return false;
    for (const Entity& entity : game.entities)
        if (entity.kind == EntityKind::Door)
            return check(entity.fixture_open && !entity.impassable,
                         "open door still blocked path");
    return false;
}

bool forest_tools() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    player->inventory.slots[0] = make_item(ItemKind::Pickaxe);
    *game.stage.at({3, 2}) = {TileKind::Wall, 100, 0};
    if (!check(use_held_item(game, game.players[0].slot, {3, 2}) &&
               game.stage.at({3, 2})->hp == 50,
               "pickaxe did not crack a wall")) return false;
    player->inventory.slots[0].cooldown = 0;
    use_held_item(game, game.players[0].slot, {3, 2});
    if (!check(game.stage.at({3, 2})->kind == TileKind::Ruin,
               "pickaxe did not open a shortcut")) return false;

    const Handle chicken = spawn_entity(game, EntityKind::Chicken, {3, 2});
    for (std::uint64_t seed = 1; seed < 100; ++seed) {
        Game probe;
        probe.rng = seed;
        if (random_u32(probe) % 10 == 0) { game.rng = seed; break; }
    }
    damage_entity(game, chicken.slot, 100, player->cell);
    bool meat_dropped = false;
    for (const Entity& entity : game.entities)
        meat_dropped |= entity.kind == EntityKind::GroundItem &&
                        entity.ground_item.kind == ItemKind::RawMeat;
    if (!check(meat_dropped, "animal did not roll a meat drop")) return false;
    player->inventory.slots[0] = make_item(ItemKind::RawMeat);
    spawn_entity(game, EntityKind::Campfire, {2, 3});
    if (!check(interact_with_fixture(game, 0, {2, 3}) &&
               player->inventory.slots[0].kind == ItemKind::CookedMeat,
               "campfire did not cook carried meat")) return false;
    player->health = 75;
    return check(use_held_item(game, game.players[0].slot, player->cell) &&
                 player->health == 93,
                 "cooked meat did not heal its owner");
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

bool den_room_rules() {
    bool found_den = false;
    for (std::uint64_t seed = 1; seed <= 64; ++seed) {
        Game game;
        start_run(game, seed);
        game.run.floor = 2;
        generate_world_floor(game);
        for (const Entity& entity : game.entities)
            found_den |= entity.kind == EntityKind::Den;
        if (!check(floor_reachable(game), "wolf den blocked the forest exit")) return false;
    }
    if (!check(found_den, "forest side-room pool never placed a den")) return false;
    Game game = small_game();
    const Handle den = spawn_entity(game, EntityKind::Den, {4, 2});
    get_entity(game, den)->spawn_wait = 0;
    step_game(game, {});
    bool spawned_wolf = false;
    for (const Entity& entity : game.entities)
        spawned_wolf |= entity.kind == EntityKind::Wolf;
    return check(spawned_wolf, "living wolf den did not release a wolf");
}

bool crusher_room_rules() {
    Game room;
    start_run(room, 71);
    room.run.floor = 3;
    generate_world_floor(room);
    bool found_crusher = false;
    for (const Entity& entity : room.entities) {
        if (entity.kind != EntityKind::Crusher) continue;
        found_crusher = true;
        if (!check(walkable(room.stage.at(entity.cell)->kind),
                   "crusher started inside a wall")) return false;
    }
    if (!check(found_crusher && floor_reachable(room),
               "forest trick room blocked the route")) return false;

    Game crush = small_game();
    Entity* victim = get_entity(crush, crush.players[0]);
    victim->cell = {4, 2};
    const Handle crusher = spawn_entity(crush, EntityKind::Crusher, {3, 2});
    get_entity(crush, crusher)->script_tick = 1;
    get_entity(crush, crusher)->move_wait = 1;
    *crush.stage.at({5, 2}) = {TileKind::Wall, 100, 0};
    step_game(crush, {});
    if (!check(victim->health == 0, "crusher did not kill against a wall")) return false;

    Game push = small_game();
    Entity* pushed = get_entity(push, push.players[0]);
    pushed->cell = {4, 2};
    const Handle moving = spawn_entity(push, EntityKind::Crusher, {3, 2});
    get_entity(push, moving)->script_tick = 1;
    get_entity(push, moving)->move_wait = 1;
    step_game(push, {});
    if (!check(pushed->health == 100 && pushed->cell == Cell{5, 2},
               "crusher killed despite a free push cell")) return false;

    Game train = small_game();
    get_entity(train, train.players[0])->cell = {6, 2};
    const Handle obstacle = spawn_entity(train, EntityKind::Crusher, {2, 2});
    const Handle engine = spawn_entity(train, EntityKind::Train, {3, 2});
    *train.stage.at({2, 2}) = {TileKind::Rail, 0, 0};
    step_game(train, {});
    return check(get_entity(train, obstacle) == nullptr &&
                 get_entity(train, engine)->cell == Cell{2, 2},
                 "train did not mow through a crusher");
}

bool zombie_chicken_rules() {
    Game game = small_game();
    const Handle zombie = spawn_entity(game, EntityKind::Zombie, {4, 2});
    const Handle chicken = spawn_entity(game, EntityKind::Chicken, {4, 3});
    Entity* bird = get_entity(game, chicken);
    bird->health = bird->max_health = 30;
    bird->move_wait = 100;
    get_entity(game, zombie)->move_wait = 100;
    step_game(game, {});
    if (!check(bird->health == 25 &&
               get_entity(game, zombie)->sprite == Sprite::Zombie,
               "zombie vanished while scratching nearby chicken")) return false;
    step_game(game, {});
    return check(get_entity(game, zombie)->sprite == Sprite::Zombie,
                 "zombie stopped using its body sprite after scratching");
}

bool footstep_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, game.players[0]);
    if (!check(move_entity(game, game.players[0].slot, {3, 2}) && game.sound_count == 1,
               "first footstep was silent")) return false;
    const SoundId first = game.sounds[0].sound;
    game.sound_count = 0;
    player->move_wait = 0;
    if (!check(move_entity(game, game.players[0].slot, {2, 2}) && game.sound_count == 1 &&
               game.sounds[0].sound != first, "feet did not alternate")) return false;
    game.sound_count = 0;
    *game.stage.at({1, 2}) = {TileKind::Wall, 100, 0};
    return check(!move_entity(game, game.players[0].slot, {1, 2}) &&
                 player->move_wait == player->move_interval &&
                 game.sound_count == 1 && game.sounds[0].sound == SoundId::HitBlock1,
                 "blocked step did not take its beat and thump");
}

} // namespace

int main() {
    if (!deterministic_replay() || !handle_reuse() || !buckler_rules() ||
        !artifact_rules() || !status_rules() || !equipment_rules() ||
        !offline_reward_rules() || !entrance_respawn_rules() || !held_item_direction() ||
        !switch_route() || !forest_tools() ||
        !track_before_train() || !forest_progression() || !den_room_rules() ||
        !crusher_room_rules() || !zombie_chicken_rules() || !footstep_rules())
        return 1;
    std::puts("game rules passed");
    return 0;
}
