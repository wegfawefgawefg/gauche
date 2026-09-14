#include "route.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../entities/dispatch.hpp"

#include <algorithm>
#include <optional>
#include <vector>

namespace {

struct Supplies { int threat, healing, ammunition, equipment, stashes; };

std::optional<Cell> room_space(Game& game, const RoomPlan& room) {
    const int width = room.half_width * 2 + 1;
    std::vector<bool> seen(static_cast<std::size_t>(width * (room.half_height * 2 + 1)), false);
    std::vector<Cell> queue{room.center}, choices;
    constexpr Cell sides[]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Cell cell = queue[next];
        const int x = cell.x - room.center.x + room.half_width;
        const int y = cell.y - room.center.y + room.half_height;
        if (x < 0 || x >= width || y < 0 || y > room.half_height * 2) continue;
        const auto index = static_cast<std::size_t>(y * width + x);
        if (seen[index]) continue;
        seen[index] = true;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || tile->kind == TileKind::Lava) continue;
        if (distance(cell, game.run.spawn) >= 4 && entity_at(game, cell, false) < 0)
            choices.push_back(cell);
        for (Cell side : sides) queue.push_back(cell + side);
    }
    // ISLANDS: Required supplies never roll onto a bank isolated by water or lava.
    if (choices.empty()) return std::nullopt;
    return choices[random_u32(game) % choices.size()];
}

void enemy(Game& game, const RoomPlan& room, EntityKind kind, int cost, Supplies& budget) {
    if (cost > budget.threat) return;
    if (const auto cell = room_space(game, room)) {
        spawn_entity(game, kind, *cell);
        budget.threat -= cost;
    }
}

void encounter(Game& game, const RoomPlan& room, Supplies& budget) {
    const int round = (game.run.floor - 1) % 4;
    if (game.run.floor > 4) {
        const EntityKind hazard = game.run.floor <= 8 ? EntityKind::Ember : EntityKind::FrostBat;
        enemy(game, room, hazard, 2, budget);
        if (round >= 2) enemy(game, room, hazard, 2, budget);
        return;
    }
    switch (room.role) {
    case RoomRole::Thicket:
        enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::Wolf : EntityKind::Boar, 2, budget);
        if (round > 0) enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Brook:
        enemy(game, room, random_u32(game) % 2 == 0 ? EntityKind::SporeToad : EntityKind::LanternMoth, 1, budget);
        if (round > 0) enemy(game, room, EntityKind::Bat, 1, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Bunny, *cell);
        break;
    case RoomRole::Ruins:
        enemy(game, room, round >= 2 ? EntityKind::ZombieStack : EntityKind::Zombie,
              round >= 2 ? 3 : 1, budget);
        break;
    case RoomRole::Den:
        enemy(game, room, EntityKind::Wolf, 2, budget);
        if (round > 0) enemy(game, room, EntityKind::Den, 4, budget);
        break;
    case RoomRole::Cache:
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::CrateMimic, 2, budget);
        else enemy(game, room, EntityKind::ThornSnail, 2, budget);
        break;
    case RoomRole::Shrine:
        enemy(game, room, round > 0 ? EntityKind::Bear : EntityKind::Wolf,
              round > 0 ? 3 : 2, budget);
        break;
    case RoomRole::Orchard:
        if (const auto cell = room_space(game, room)) spawn_chicken_family(game, *cell);
        break;
    case RoomRole::Workshop:
        if (round > 0 && random_u32(game) % 2 == 0) enemy(game, room, EntityKind::CrateMimic, 2, budget);
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Dog, *cell);
        break;
    case RoomRole::Clearing:
        if (random_u32(game) % 2 == 0) enemy(game, room, EntityKind::Bat, 1, budget);
        break;
    default: break;
    }
}

void supply(Game& game, const RoomPlan& room, ItemKind kind, int count, int& remaining) {
    if (remaining <= 0) return;
    if (const auto cell = room_space(game, room)) {
        place_ground_item(game, *cell, kind, count);
        --remaining;
    }
}

void stash(Game& game, const RoomPlan& room, Supplies& budget) {
    if (budget.stashes <= 0) return;
    if (const auto cell = room_space(game, room)) {
        place_coins(game, *cell, 6 + static_cast<int>(random_u32(game) % 7));
        --budget.stashes;
    }
}

void room_loot(Game& game, const RoomPlan& room, Supplies& budget) {
    if (room.role == RoomRole::Cache || room.role == RoomRole::Shrine ||
        room.role == RoomRole::Workshop || room.role == RoomRole::Secret) stash(game, room, budget);
    switch (room.role) {
    case RoomRole::Secret:
        if (const auto cell = room_space(game, room))
        {
            constexpr ItemKind relics[]{ItemKind::ConductorHat, ItemKind::RocketLauncher,
                ItemKind::Musket, ItemKind::Pickaxe, ItemKind::Blunderbuss, ItemKind::Crossbow};
            place_ground_item(game, *cell, relics[random_u32(game) % std::size(relics)]);
        }
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
        break;
    case RoomRole::Workshop:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::Pickaxe : ItemKind::BearTrap,
               1, budget.equipment);
        supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
        break;
    case RoomRole::Cache: {
        constexpr ItemKind equipment[]{ItemKind::Bow, ItemKind::Musket, ItemKind::Buckler,
            ItemKind::Bomb, ItemKind::Pistol, ItemKind::SleepMeds,
            ItemKind::Hatchet, ItemKind::HuntingSpear, ItemKind::WoodenMaul, ItemKind::FlintKnife};
        supply(game, room, equipment[random_u32(game) % std::size(equipment)], 1, budget.equipment);
        supply(game, room, ItemKind::Bandage, 2, budget.healing);
        break;
    }
    case RoomRole::Shrine:
        supply(game, room, ItemKind::Medkit, 1, budget.healing);
        break;
    case RoomRole::Orchard: case RoomRole::Clearing:
        supply(game, room, random_u32(game) % 2 == 0 ? ItemKind::ThrowingRock : ItemKind::Rake, 1, budget.equipment);
        supply(game, room, ItemKind::Bandage, 2, budget.healing);
        break;
    default:
        if (random_u32(game) % 3 == 0) supply(game, room, ItemKind::Ammo, 1, budget.ammunition);
        break;
    }
}

void room_light(Game& game, const RoomPlan& room) {
    if (game.run.floor > 4) return;
    if (room.role == RoomRole::Clearing || room.role == RoomRole::Orchard || room.role == RoomRole::Brook) {
        if (game.run.roof_light_count >= static_cast<int>(game.run.roof_lights.size())) return;
        StageLight& gap = game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)];
        gap = {room.center, {std::min(room.half_width, room.half_height), 1350, {240, 224, 176}}};
    } else if (room.role == RoomRole::Entrance || room.role == RoomRole::Workshop) {
        if (const auto cell = room_space(game, room)) spawn_entity(game, EntityKind::Campfire, *cell);
    }
}

} // namespace

void populate_rooms(Game& game, const FloorPlan& plan) {
    const int round = (game.run.floor - 1) % 4;
    Supplies budget{9 + round * 5, 2 + round / 2, 2 + round, 3, 3 + round / 2};
    game.run.roof_lights = {};
    game.run.roof_light_count = 0;
    // LANDMARKS: Reserve objectives before any scatter or encounter placement.
    const Cell objective = plan.rooms[static_cast<std::size_t>(plan.objective_room)].center;
    spawn_entity(game, game.run.objective == ObjectiveKind::Key ? EntityKind::Key : EntityKind::Switch, objective);
    spawn_entity(game, EntityKind::Door, plan.door);
    spawn_entity(game, EntityKind::Exit, game.run.exit);
    place_ground_item(game, game.run.spawn + Cell{0, 2}, ItemKind::Stick);
    for (const RoomPlan& room : plan.rooms) {
        room_light(game, room);
        if (room.role == RoomRole::Entrance || room.role == RoomRole::Exit) continue;
        encounter(game, room, budget);
        room_loot(game, room, budget);
    }
    // SUPPLIES: A sparse role roll must not accidentally remove all healing or new equipment.
    const RoomPlan& shrine = plan.rooms[static_cast<std::size_t>(plan.objective_room)];
    while (budget.healing > 0) {
        const int before = budget.healing;
        supply(game, shrine, ItemKind::Bandage, 2, budget.healing);
        if (before == budget.healing) break;
    }
    if (budget.equipment > 0) supply(game, shrine, ItemKind::Bow, 1, budget.equipment);
    if (budget.ammunition > 0) supply(game, shrine, ItemKind::Ammo, 1, budget.ammunition);
    while (budget.stashes > 0) {
        const int before = budget.stashes;
        stash(game, shrine, budget);
        if (before == budget.stashes) break;
    }
}
