#include "route.hpp"

#include <cstdlib>

namespace {

void clear_approach(Game& game, FloorPlan& plan, Cell from, Cell to) {
    const Cell step{from.x == to.x ? 0 : to.x > from.x ? 1 : -1,
                    from.y == to.y ? 0 : to.y > from.y ? 1 : -1};
    for (;;) {
        *game.stage.at(from) = {TileKind::Empty, 0, 0};
        plan.protected_cells[static_cast<std::size_t>(from.y * plan.width + from.x)] = 1;
        if (from == to) return;
        from = from + step;
    }
}

Cell gate_room(Game& game, FloorPlan& plan, int room_index) {
    const RoomPlan& room = plan.rooms[static_cast<std::size_t>(room_index)];
    int neighbor = -1;
    for (RouteEdge edge : plan.edges) {
        if (edge.a == room_index) neighbor = edge.b;
        if (edge.b == room_index) neighbor = edge.a;
    }
    if (neighbor < 0) return room.center;
    const RoomPlan& approach = plan.rooms[static_cast<std::size_t>(neighbor)];
    const Cell side = approach.grid - room.grid;
    const int width = room.half_width + 1, height = room.half_height + 1;
    const Cell door = room.center + Cell{side.x * width, side.y * height};
    // BOUNDARY: Offset centers can create several parallel crossings at a bend.
    // Enclose the leaf room with one opening instead of trusting a corridor endpoint.
    // These are ordinary diggable walls; tools may still earn a deliberate shortcut.
    for (int y = -height; y <= height; ++y)
        for (int x = -width; x <= width; ++x) {
            if (std::abs(x) != width && std::abs(y) != height) continue;
            const Cell cell = room.center + Cell{x, y};
            Tile* tile = game.stage.at(cell);
            if (tile == nullptr) continue;
            const bool opening = cell == door;
            *tile = opening ? Tile{TileKind::Empty, 0, 0} : Tile{TileKind::Wall, 100, 0};
            plan.protected_cells[static_cast<std::size_t>(cell.y * plan.width + cell.x)] = opening ? 1 : 0;
        }
    // APPROACH: The old bend may lie along the wall we just closed. Join the
    // doorway to its neighbor on the outside of the boundary, then turn inward.
    const Cell bend = side.x != 0 ? Cell{approach.center.x, door.y} : Cell{door.x, approach.center.y};
    clear_approach(game, plan, door, bend);
    clear_approach(game, plan, bend, approach.center);
    return door;
}

} // namespace

void place_room_gates(Game& game, FloorPlan& plan) {
    plan.door = gate_room(game, plan, plan.exit_room);
    if (plan.secret_room < 0) return;
    plan.secret_door = gate_room(game, plan, plan.secret_room);
    // SECRET: The optional entrance keeps its chipped, damageable barrier.
    *game.stage.at(plan.secret_door) = {TileKind::Wall, 25, 0, 30, BreakRule::Damageable, 0};
}
