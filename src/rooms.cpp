#include "game.hpp"

#include <algorithm>
#include <array>
#include <deque>
#include <utility>
#include <vector>

namespace {

constexpr int room_width = 12;
constexpr int room_height = 10;

void carve(Stage& stage, int left, int top, int right, int bottom, TileKind kind) {
    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            Tile* tile = stage.at({x, y});
            if (tile != nullptr) *tile = {kind, 0, 0};
        }
    }
}

void stamp_room(Game& game, int column, int row, bool main_route) {
    const int x = column * room_width;
    const int y = row * room_height;
    const int world = (game.run.floor - 1) / 4;
    const int local_floor = (game.run.floor - 1) % 4;
    carve(game.stage, x + 1, y + 1, x + 10, y + 8, TileKind::Empty);
    const int variant = static_cast<int>(random_u32(game) % 5);
    const TileKind growth = world == 0 ? TileKind::Grass :
                            (world == 1 ? TileKind::Lava : TileKind::Ice);
    if (variant == 0) {
        carve(game.stage, x + 2, y + 2, x + 3, y + 3, growth);
        carve(game.stage, x + 8, y + 6, x + 9, y + 7, growth);
    } else if (variant == 1) {
        carve(game.stage, x + 2, y + 6, x + 3, y + 7, TileKind::Ruin);
        carve(game.stage, x + 8, y + 2, x + 9, y + 3, TileKind::Ruin);
    } else if (variant == 2) {
        carve(game.stage, x + 2, y + 2, x + 2, y + 7, growth);
        carve(game.stage, x + 9, y + 2, x + 9, y + 7, growth);
    } else if (variant == 3) {
        carve(game.stage, x + 3, y + 3, x + 8, y + 3, growth);
        carve(game.stage, x + 3, y + 6, x + 8, y + 6, growth);
    } else {
        carve(game.stage, x + 4, y + 3, x + 7, y + 6, growth);
    }
    if (!main_route && (variant == 2 || local_floor == 2)) {
        *game.stage.at({x + 3, y + 5}) = {TileKind::Wall, 100, 0};
        *game.stage.at({x + 8, y + 4}) = {TileKind::Wall, 100, 0};
    }
    if (main_route && local_floor == 3 && column > 0 && column < 5) {
        *game.stage.at({x + 5, y + 3}) = {TileKind::Wall, 100, 0};
        *game.stage.at({x + 6, y + 6}) = {TileKind::Wall, 100, 0};
    }
}

void connect_branch(Game& game, int column, int row) {
    stamp_room(game, column, row, false);
    const int center = column * room_width + 6;
    if (row == 0)
        carve(game.stage, center - 1, 8, center + 1, 11, TileKind::Empty);
    else
        carve(game.stage, center - 1, 18, center + 1, 21, TileKind::Empty);
}

void ground_item(Game& game, Cell cell, ItemKind kind, int count = 1) {
    const Handle handle = spawn_entity(game, EntityKind::GroundItem, cell);
    if (Entity* entity = get_entity(game, handle)) {
        entity->ground_item = make_item(kind, count);
        entity->sprite = item_sprite(kind);
    }
}

} // namespace

void generate_world_floor(Game& game) {
    std::array<Entity, 4> previous{};
    std::array<bool, 4> joined{};
    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (const Entity* player = get_entity(game, game.players[owner])) {
            previous[owner] = *player;
            joined[owner] = true;
        }
    }
    if (!game.started) joined[0] = true;
    Game next;
    next.rng = game.rng;
    next.tick = game.tick;
    next.run = game.run;
    next.started = true;
    for (int slot = 0; slot < max_entities; ++slot)
        next.entities[static_cast<std::size_t>(slot)].generation =
            game.entities[static_cast<std::size_t>(slot)].generation;
    game = std::move(next);
    game.run.phase = RunPhase::Playing;

    const int columns = 5 + (game.run.floor - 1) % 2;
    game.stage.width = columns * room_width;
    game.stage.height = 3 * room_height;
    game.stage.tiles.assign(static_cast<std::size_t>(game.stage.width * game.stage.height),
                            {TileKind::Wall, 100, 0});
    const int branch = 1 + static_cast<int>(random_u32(game) %
                                                 static_cast<std::uint32_t>(columns - 3));
    const int extra = branch == 1 ? columns - 2 : 1;
    const int extra_row = random_u32(game) % 2 == 0 ? 0 : 2;
    for (int column = 0; column < columns; ++column) {
        stamp_room(game, column, 1, true);
        if (column < columns - 1)
            carve(game.stage, column * room_width + 10, 14,
                  (column + 1) * room_width + 2, 16, TileKind::Empty);
    }
    connect_branch(game, branch, 0);
    connect_branch(game, branch, 2);
    connect_branch(game, extra, extra_row);
    const int branch_x = branch * room_width + 6;
    const int extra_x = extra * room_width + 6;
    const int extra_y = extra_row * room_height + 5;
    game.run.spawn = {6, 15};
    game.run.exit = {columns * room_width - 6, 15};
    game.run.has_key = false;
    game.run.objective = (game.run.floor - 1) % 2 == 0 ?
        ObjectiveKind::Key : ObjectiveKind::Switch;
    game.run.roof_light_count = 0;
    for (int column = 0; column < columns; ++column)
        game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)] =
            {column * room_width + 6, 13};
    game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)] = {branch_x, 5};
    game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)] =
        {extra_x, extra_y};
    spawn_entity(game, game.run.objective == ObjectiveKind::Key ?
                 EntityKind::Key : EntityKind::Switch, {branch_x, 5});
    spawn_entity(game, EntityKind::Door, {(columns - 2) * room_width, 15});
    spawn_entity(game, EntityKind::Exit, game.run.exit);

    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (!joined[owner]) continue;
        if (previous[owner].health <= 0 && game.run.death_policy == DeathPolicy::NoRespawn)
            continue;
        const Handle handle = spawn_entity(game, EntityKind::Player,
                                            game.run.spawn + Cell{static_cast<int>(owner), 0});
        game.players[owner] = handle;
        Entity* player = get_entity(game, handle);
        player->owner = static_cast<int>(owner);
        player->impassable = game.run.online[owner];
        if (game.run.floor == 1 && previous[owner].kind == EntityKind::None) {
            player->inventory = {};
            insert_item(player->inventory, make_item(ItemKind::Fist));
            insert_item(player->inventory, make_item(ItemKind::Bandage, 3));
        } else {
            player->inventory = previous[owner].inventory;
            player->max_health = previous[owner].max_health;
            player->health = previous[owner].health > 0 ? previous[owner].health : player->max_health;
            player->move_interval = previous[owner].move_interval;
            player->artifacts = previous[owner].artifacts;
        }
    }

    constexpr std::array<ItemKind, 12> loot{
        ItemKind::Bandage, ItemKind::Bandaid, ItemKind::Buckler, ItemKind::Pistol,
        ItemKind::Bow, ItemKind::Bomb, ItemKind::Ammo, ItemKind::SleepMeds,
        ItemKind::Stick, ItemKind::BearTrap, ItemKind::Mine, ItemKind::Pickaxe};
    for (int column = 1; column < columns - 1; ++column) {
        const int x = column * room_width + 5;
        const int world = (game.run.floor - 1) / 4;
        const int local_floor = (game.run.floor - 1) % 4;
        const int enemy_count = 1 + local_floor / 2 +
                                static_cast<int>(random_u32(game) % 2);
        for (int index = 0; index < enemy_count; ++index) {
            const Cell cell{x + index, 12 + index * 2};
            if (entity_at(game, cell, true) >= 0) continue;
            const int roll = static_cast<int>(random_u32(game) % 4);
            const EntityKind kind = world == 0 ?
                (roll == 0 ? EntityKind::Zombie :
                 roll == 1 ? EntityKind::Bat : EntityKind::Wolf) :
                (world == 1 ? (roll == 0 ? EntityKind::Bat : EntityKind::Ember) :
                 (roll == 0 ? EntityKind::Wolf : EntityKind::FrostBat));
            spawn_entity(game, kind, cell);
        }
        if (world == 0 && random_u32(game) % 3 == 0)
            spawn_entity(game, random_u32(game) % 2 == 0 ? EntityKind::Bunny :
                         EntityKind::Chicken, {x + 3, 17});
        if (local_floor == 3 && column == columns - 3)
            spawn_entity(game, EntityKind::Bear, {x + 2, 17});
        if (random_u32(game) % 2 == 0)
            ground_item(game, {x, 17}, loot[random_u32(game) % loot.size()], 1);
    }
    ground_item(game, {branch_x + 3, 5}, ItemKind::Buckler);
    ground_item(game, {branch_x - 2, 25}, ItemKind::Bow);
    const int world = (game.run.floor - 1) / 4;
    const int local_floor = (game.run.floor - 1) % 4;
    if (world == 0) spawn_entity(game, EntityKind::Campfire, {4, 13});
    if (world == 0 && local_floor == 1)
        ground_item(game, {branch_x - 3, 25}, ItemKind::Pickaxe);
    ground_item(game, {extra_x + 2, extra_y},
                world == 0 ? ItemKind::Bandage :
                (world == 1 ? ItemKind::Mine : ItemKind::Musket),
                world == 0 ? 3 : 1);
    if (local_floor >= 2) {
        const EntityKind guardian = world == 0 ? EntityKind::Bear :
                                    (world == 1 ? EntityKind::Ember : EntityKind::FrostBat);
        spawn_entity(game, guardian, {branch_x + 2, 5});
        if (local_floor == 2) {
            spawn_entity(game, guardian, {branch_x + 2, 25});
            spawn_entity(game, guardian, {branch_x - 2, 25});
        }
    }
    const bool den_room = world == 0 && (local_floor == 1 || local_floor == 3) &&
                          random_u32(game) % 2 == 0;
    if (den_room) {
        spawn_entity(game, EntityKind::Den, {extra_x - 2, extra_y});
        spawn_entity(game, EntityKind::Wolf, {extra_x + 2, extra_y - 2});
        spawn_entity(game, EntityKind::Wolf, {extra_x + 2, extra_y + 2});
        ground_item(game, {extra_x, extra_y + 2}, ItemKind::Bow);
    } else if (local_floor == 3)
        spawn_entity(game, EntityKind::Spawner, {extra_x - 2, extra_y});
    if (world == 0 && local_floor == 2) {
        *game.stage.at({extra_x - 4, extra_y}) = {TileKind::Wall, 100, 0};
        *game.stage.at({extra_x - 3, extra_y}) = {TileKind::Empty, 0, 0};
        *game.stage.at({extra_x + 4, extra_y}) = {TileKind::Wall, 100, 0};
        spawn_entity(game, EntityKind::Crusher, {extra_x - 3, extra_y});
        ground_item(game, {extra_x + 2, extra_y + 1}, ItemKind::Buckler);
    }
    if (game.run.floor == 1) ground_item(game, {8, 15}, ItemKind::Stick);
    spawn_entity(game, EntityKind::Spawner, {branch_x + 3, 25});
    emit_sound(game, SoundId::LevelStart, game.run.spawn, false);
}

bool floor_reachable(const Game& game) {
    const int count = game.stage.width * game.stage.height;
    if (count <= 0) return false;
    std::vector<bool> visited(static_cast<std::size_t>(count * 2), false);
    std::deque<std::pair<Cell, bool>> pending;
    pending.push_back({game.run.spawn, false});
    constexpr std::array<Cell, 4> directions{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    Cell key{-1, -1};
    Cell door{-1, -1};
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::Key || entity.kind == EntityKind::Switch)
            key = entity.cell;
        if (entity.kind == EntityKind::Door) door = entity.cell;
    }
    while (!pending.empty()) {
        auto [cell, has_key] = pending.front();
        pending.pop_front();
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(tile->kind)) continue;
        has_key |= cell == key;
        if (cell == door && !has_key) continue;
        const int index = (cell.y * game.stage.width + cell.x) * 2 + (has_key ? 1 : 0);
        if (visited[static_cast<std::size_t>(index)]) continue;
        visited[static_cast<std::size_t>(index)] = true;
        if (cell == game.run.exit) return true;
        for (Cell direction : directions) pending.push_back({cell + direction, has_key});
    }
    return false;
}
