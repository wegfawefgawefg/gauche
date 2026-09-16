#include "route.hpp"
#include "room_frame.hpp"
#include "forest_den.hpp"
#include "industrial_geometry.hpp"
#include "ice_terrain.hpp"
#include "ice_shelves.hpp"
#include "ice_thaw.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool inside_shape(const RoomPlan& world, int x, int y) {
    const Cell local=room_local(world,{x,y});x=local.x;y=local.y;
    const RoomPlan room=unturned_room(world);
    const int ax = std::abs(x), ay = std::abs(y);
    const int w = room.half_width, h = room.half_height;
    switch (room.shape) {
    case RoomShape::BearHollow: return ax*ax+ay*ay<=88 || (ax<=3 && ay<=h);
    case RoomShape::WorkHall: return !(ax==7 && ay>=4 && ay<=5);
    case RoomShape::ExcavatedHall: return ax<=7 || ay<=h-2-(ax-8);
    case RoomShape::IceShelf: return true;
    case RoomShape::ThawCavern: return ax+ay<=18;
    case RoomShape::Clearing: return ax <= w && ay <= h && ax + ay <= w + h - 3;
    case RoomShape::Cross: return (ax <= w && ay <= h / 2) || (ax <= w / 2 && ay <= h);
    case RoomShape::BentHall: return (x <= 2 && y >= -2) || (x >= -2 && y <= 2);
    case RoomShape::TwinCave:
        return (std::abs(x + w / 2) + std::abs(y + 1) <= h) ||
               (std::abs(x - w / 2) + std::abs(y - 1) <= h) || ay <= 1;
    case RoomShape::Courtyard: return ax + ay <= w + h - 1 && !(ax == w / 2 && ay == h / 2);
    case RoomShape::Pillars: return !(ax == w - 2 && ay >= 2 && ay <= h - 1);
    case RoomShape::ChapelNave:
        // NAVE: Narrow entry and apse, broad seating and a side vestry.
        return (ay <= h-2 && ax <= w-1) || (ax <= 3 && ay <= h) ||
            (x == w && y >= -2 && y <= (room.mirrored ? 2 : 0));
    case RoomShape::Gallery: return ay <= 2 || (ax <= w - 2 && ax % 4 <= 1);
    case RoomShape::Steps: return ax <= w - (y + h) / 4 || ay <= 1;
    }
    return false;
}

void floor_cell(Game& game, FloorPlan& plan, Cell cell, TileKind kind, bool reserve) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return;
    *tile = {kind, 0, 0};
    if (reserve) plan.protected_cells[static_cast<std::size_t>(cell.y * plan.width + cell.x)] = 1;
}

TileKind room_floor(const Game& game, const RoomPlan& world, int x, int y) {
    const Cell local=turn_cell({x,y},4-world.turns);x=local.x;y=local.y;
    const RoomPlan room=unturned_room(world);
    if (forest_floor(game.run.floor) && room.mirrored) x=-x;
    if (room.shape==RoomShape::WorkHall || room.shape==RoomShape::ExcavatedHall) return TileKind::Ruin;
    if (room.shape==RoomShape::ThawCavern) return ice_thaw_floor(room,x,y);
    if (room.shape==RoomShape::IceShelf) return ice_shelf_floor(room,x,y);
    const bool trail = std::abs(x) <= 1 || std::abs(y) <= 1;
    if (trail && !socket_room(game,world)) return TileKind::Empty;
    if (ice_floor(game.run.floor)) return ice_room_floor(room, x, y);
    if (room.role==RoomRole::Workfront || room.role==RoomRole::BlastingAlcove || room.role==RoomRole::AssemblyLine || room.role==RoomRole::SettlingTanks || room.role==RoomRole::FreightSiding) return TileKind::Ruin;
    if (room.role == RoomRole::Ruins || room.role == RoomRole::Workshop ||
        room.role == RoomRole::Shrine) return TileKind::Ruin;
    if (room.role == RoomRole::Brook && std::abs(x + y / 2) <= 2) return TileKind::ShallowWater;
    if (forest_floor(game.run.floor)) {
        if (room.role == RoomRole::Thicket || room.role == RoomRole::Orchard ||
            x * x + y * y > room.half_width * room.half_height / 2) return TileKind::Grass;
    } else if (industrial_floor(game.run.floor)) {
        if (x > 2 && y > 2) return TileKind::Lava;
    }
    return TileKind::Empty;
}

void carve_room(Game& game, FloorPlan& plan, const RoomPlan& room) {
    for (int y = -room.half_height; y <= room.half_height; ++y)
        for (int x = -room.half_width; x <= room.half_width; ++x) {
            // Natural rooms keep their real outline; graph corridors reserve
            // only the arms that lead to actual neighbors. Authored machinery
            // keeps its existing service aisles until its components migrate.
            const bool path = !socket_room(game,room) && (std::abs(x)<=1 || std::abs(y)<=1);
            if (!path && !inside_shape(room, x, y)) continue;
            floor_cell(game, plan, room.center + Cell{x, y}, room_floor(game, room, x, y), path);
        }
    if (room.role==RoomRole::Entrance) {
        // Four party spawn cells and the starting pickup must remain dry/free.
        for (int y=-1;y<=2;++y) for (int x=-1;x<=3;++x)
            floor_cell(game,plan,room.center+Cell{x,y},TileKind::Empty,true);
    }
}

void corridor(Game& game, FloorPlan& plan, Cell from, Cell to, int half_width) {
    const Cell step{from.x == to.x ? 0 : (to.x > from.x ? 1 : -1),
                    from.y == to.y ? 0 : (to.y > from.y ? 1 : -1)};
    const Cell sideways = step.x == 0 ? Cell{1, 0} : Cell{0, 1};
    while (true) {
        for (int offset = -half_width; offset <= half_width; ++offset)
            floor_cell(game, plan, from + Cell{sideways.x * offset, sideways.y * offset},
                       TileKind::Empty, true);
        if (from == to) break;
        from = from + step;
    }
}

void connect_rooms(Game& game, FloorPlan& plan, RouteEdge edge) {
    const RoomPlan& a = plan.rooms[static_cast<std::size_t>(edge.a)];
    const RoomPlan& b = plan.rooms[static_cast<std::size_t>(edge.b)];
    const bool exit = edge.a == plan.exit_room || edge.b == plan.exit_room;
    const bool secret = edge.a == plan.secret_room || edge.b == plan.secret_room;
    const bool gated = exit || secret;
    const int half_width = gated ? 0 : static_cast<int>(random_u32(game) % 2);
    Cell first, second;
    if (a.grid.x != b.grid.x) {
        const int middle = (a.center.x + b.center.x) / 2;
        first = {middle, a.center.y}; second = {middle, b.center.y};
    } else {
        const int middle = (a.center.y + b.center.y) / 2;
        first = {a.center.x, middle}; second = {b.center.x, middle};
    }
    corridor(game, plan, a.center, first, half_width);
    corridor(game, plan, first, second, half_width);
    corridor(game, plan, second, b.center, half_width);
}

} // namespace

void carve_floor(Game& game, FloorPlan& plan) {
    game.stage.width = plan.width;
    game.stage.height = plan.height;
    game.stage.tiles.assign(static_cast<std::size_t>(plan.width * plan.height),
                            {TileKind::Wall, 100, 0});
    plan.protected_cells.assign(game.stage.tiles.size(), 0);
    for (const RoomPlan& room : plan.rooms) carve_room(game, plan, room);
    for (RouteEdge edge : plan.edges) connect_rooms(game, plan, edge);
    carve_forest_den(game,plan);
    connect_ice_shelves(game,plan);
    carve_ice_thaw(game,plan);
    carve_industrial_geometry(game,plan);
    place_room_gates(game, plan);
    for (int y = 0; y < plan.height; ++y)
        for (int x = 0; x < plan.width; ++x)
            if (x == 0 || y == 0 || x == plan.width - 1 || y == plan.height - 1)
                *game.stage.at({x, y}) = {TileKind::Wall, 100, 0, 100, BreakRule::Unbreakable, 0};
}
