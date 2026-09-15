#include "water.hpp"
#include "../entities/bell_diver.hpp"
#include "route.hpp"

#include <cstdlib>

bool shallow_water(TileKind kind) {
    return kind == TileKind::ShallowWater || kind == TileKind::Spring || kind == TileKind::IceHole;
}

bool wading_actor(const Entity& actor) {
    if (actor.move_interval <= 0 || actor.health <= 0 || diver_submerged(actor)) return false;
    switch (actor.kind) {
    case EntityKind::Wasp: case EntityKind::CarrionCrow:
    case EntityKind::Mosquito: case EntityKind::Owl: case EntityKind::Woodpecker:
    case EntityKind::Bat: case EntityKind::FrostBat: case EntityKind::LanternMoth:
    case EntityKind::Train: case EntityKind::RailLayer:
        return false;
    default: return true;
    }
}

void place_water_scenes(Game& game, const FloorPlan& plan) {
    if (!forest_floor(game.run.floor)) return;
    for (const RoomPlan& room : plan.rooms) {
        if (room.role != RoomRole::Brook) continue;
        // SPRING: An actual northern wall feeds a reachable pool beside the dry route.
        bool placed = false;
        for (int y = -room.half_height; y < -1 && !placed; ++y)
            for (int x = -room.half_width + 1; x < room.half_width && !placed; ++x) {
                const Cell cell = room.center + Cell{x, y};
                Tile* tile = game.stage.at(cell);
                if (tile == nullptr || !walkable(tile->kind) || plan.protected_cell(cell) ||
                    game.stage.at_or_border(cell + Cell{0, -1}).kind != TileKind::Wall ||
                    !walkable(game.stage.at_or_border(cell + Cell{0, 1}).kind)) continue;
                for (int dy = 0; dy <= 3; ++dy)
                    for (int dx = -2; dx <= 2; ++dx) {
                        const Cell pool = cell + Cell{dx, dy};
                        Tile* ground = game.stage.at(pool);
                        if (ground == nullptr || plan.protected_cell(pool) ||
                            !walkable(ground->kind) || std::abs(dx) + std::abs(dy - 1) > 3) continue;
                        *ground = {TileKind::ShallowWater, 0, 0};
                    }
                *tile = {TileKind::Spring, 0, 0};
                placed = true;
            }
    }
}
