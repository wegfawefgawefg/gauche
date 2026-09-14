#include "floor_internal.hpp"
#include "ground_items.hpp"

#include <array>

void place_ground_item(Game& game, Cell cell, ItemKind kind, int count) {
    const Handle handle = spawn_entity(game, EntityKind::GroundItem,
                                       nearby_ground_item_cell(game, cell));
    if (Entity* entity = get_entity(game, handle)) {
        entity->ground_item = make_item(kind, count);
        entity->sprite = item_sprite(kind);
    }
}


void populate_floor(Game& game, int columns, Cell branch, Cell extra) {
    const int branch_x = branch.x;
    const int extra_x = extra.x;
    const int extra_y = extra.y;
    // Corridor: short, readable encounters with a chance of early equipment.
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
                (roll == 0 ? EntityKind::Dog :
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
            place_ground_item(game, {x, 17}, loot[random_u32(game) % loot.size()], 1);
    }
    place_ground_item(game, {branch_x + 3, 5}, ItemKind::Buckler);
    place_ground_item(game, {branch_x - 2, 25}, ItemKind::Bow);
    const int world = (game.run.floor - 1) / 4;
    const int local_floor = (game.run.floor - 1) % 4;
    if (world == 0) spawn_entity(game, EntityKind::Campfire, {4, 13});
    if (world == 0 && local_floor == 1)
        place_ground_item(game, {branch_x - 3, 25}, ItemKind::Pickaxe);
    place_ground_item(game, {extra_x + 2, extra_y},
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
    // Side room: dens, spawners, and the crusher share one branch socket.
    const bool den_room = world == 0 && (local_floor == 1 || local_floor == 3) &&
                          random_u32(game) % 2 == 0;
    if (den_room) {
        spawn_entity(game, EntityKind::Den, {extra_x - 2, extra_y});
        spawn_entity(game, EntityKind::Wolf, {extra_x + 2, extra_y - 2});
        spawn_entity(game, EntityKind::Wolf, {extra_x + 2, extra_y + 2});
        place_ground_item(game, {extra_x, extra_y + 2}, ItemKind::Bow);
    } else if (local_floor == 3)
        spawn_entity(game, EntityKind::Spawner, {extra_x - 2, extra_y});
    if (world == 0 && local_floor == 2) {
        *game.stage.at({extra_x - 4, extra_y}) = {TileKind::Wall, 100, 0};
        *game.stage.at({extra_x - 3, extra_y}) = {TileKind::Empty, 0, 0};
        *game.stage.at({extra_x + 4, extra_y}) = {TileKind::Wall, 100, 0};
        spawn_entity(game, EntityKind::Crusher, {extra_x - 3, extra_y});
        place_ground_item(game, {extra_x + 2, extra_y + 1}, ItemKind::Buckler);
    }
    if (game.run.floor == 1) place_ground_item(game, {8, 15}, ItemKind::Stick);
    spawn_entity(game, EntityKind::Spawner, {branch_x + 3, 25});
    if (world == 0 && local_floor >= 2)
        spawn_entity(game, EntityKind::ZombieStack, {branch_x - 3, 26});
}
