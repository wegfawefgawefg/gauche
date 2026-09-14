#include "route.hpp"
#include "ice_terrain.hpp"

#include <algorithm>
#include <array>

namespace {

constexpr int pitch = 22;
constexpr std::array<Cell, 4> sides{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

bool contains(const FloorPlan& plan, Cell grid) {
    return std::any_of(plan.rooms.begin(), plan.rooms.end(),
                      [grid](const RoomPlan& room) { return room.grid == grid; });
}

void grow_route(Game& game, FloorPlan& plan) {
    plan.rooms.push_back({{3, 3}});
    const int target = 10 + (game.run.floor - 1) % 4 * 2 + static_cast<int>(random_u32(game) % 3);
    // TREE: Any open frontier can grow; depth biases the route without fixing its direction.
    while (static_cast<int>(plan.rooms.size()) < target) {
        struct Frontier { int parent; Cell grid; };
        std::vector<Frontier> options;
        for (std::size_t i = 0; i < plan.rooms.size(); ++i) {
            int degree = 0;
            for (RouteEdge edge : plan.edges)
                if (edge.a == static_cast<int>(i) || edge.b == static_cast<int>(i)) ++degree;
            if (degree >= (i == 0 ? 2 : 3)) continue;
            for (Cell side : sides) {
                const Cell grid = plan.rooms[i].grid + side;
                if (grid.x < 0 || grid.y < 0 || grid.x >= 7 || grid.y >= 7 || contains(plan, grid)) continue;
                options.push_back({static_cast<int>(i), grid});
                if (plan.rooms[i].depth >= 2) options.push_back(options.back());
            }
        }
        if (options.empty()) break;
        const Frontier next = options[random_u32(game) % options.size()];
        RoomPlan room;
        room.grid = next.grid;
        room.depth = plan.rooms[static_cast<std::size_t>(next.parent)].depth + 1;
        plan.edges.push_back({next.parent, static_cast<int>(plan.rooms.size())});
        plan.rooms.push_back(room);
    }
}

void choose_objectives(Game& game, FloorPlan& plan) {
    for (std::size_t i = 1; i < plan.rooms.size(); ++i)
        if (plan.rooms[i].depth >= plan.rooms[static_cast<std::size_t>(plan.exit_room)].depth)
            plan.exit_room = static_cast<int>(i);
    std::vector<bool> exit_route(plan.rooms.size(), false);
    int ancestor = plan.exit_room;
    while (ancestor != 0) {
        exit_route[static_cast<std::size_t>(ancestor)] = true;
        for (RouteEdge edge : plan.edges)
            if (edge.b == ancestor) { ancestor = edge.a; break; }
    }
    int best = -1;
    for (std::size_t i = 1; i < plan.rooms.size(); ++i) {
        if (static_cast<int>(i) == plan.exit_room) continue;
        const int score = plan.rooms[i].depth + (exit_route[i] ? 0 : 20);
        if (score > best) { best = score; plan.objective_room = static_cast<int>(i); }
    }
    // LOOPS: Keep the exit a gated leaf, but let other branches reconnect.
    for (std::size_t a = 1; a < plan.rooms.size(); ++a)
        for (std::size_t b = a + 1; b < plan.rooms.size(); ++b) {
            if (static_cast<int>(a) == plan.exit_room || static_cast<int>(b) == plan.exit_room ||
                distance(plan.rooms[a].grid, plan.rooms[b].grid) != 1) continue;
            const bool linked = std::any_of(plan.edges.begin(), plan.edges.end(), [a, b](RouteEdge edge) {
                return edge.a == static_cast<int>(a) && edge.b == static_cast<int>(b);
            });
            if (!linked && random_u32(game) % 3 == 0)
                plan.edges.push_back({static_cast<int>(a), static_cast<int>(b)});
        }
}

void describe_rooms(Game& game, FloorPlan& plan) {
    Cell low{7, 7}, high{};
    for (const RoomPlan& room : plan.rooms) {
        low.x = std::min(low.x, room.grid.x); low.y = std::min(low.y, room.grid.y);
        high.x = std::max(high.x, room.grid.x); high.y = std::max(high.y, room.grid.y);
    }
    plan.width = (high.x - low.x + 1) * pitch + 2;
    plan.height = (high.y - low.y + 1) * pitch + 2;
    constexpr RoomRole roles[]{RoomRole::Clearing, RoomRole::Thicket, RoomRole::Brook,
        RoomRole::Ruins, RoomRole::Den, RoomRole::Cache, RoomRole::Workshop, RoomRole::Orchard};
    constexpr RoomRole cold_roles[]{RoomRole::Reservoir, RoomRole::FishingHut, RoomRole::Bathhouse,
        RoomRole::IceQuarry, RoomRole::Observatory, RoomRole::Shelter, RoomRole::EchoTunnel, RoomRole::Cache};
    for (RoomPlan& room : plan.rooms) {
        room.center = {(room.grid.x - low.x) * pitch + pitch / 2 + 1,
                       (room.grid.y - low.y) * pitch + pitch / 2 + 1};
        room.center.x += static_cast<int>(random_u32(game) % 3) - 1;
        room.center.y += static_cast<int>(random_u32(game) % 3) - 1;
        room.half_width = 5 + static_cast<int>(random_u32(game) % 5);
        room.half_height = 4 + static_cast<int>(random_u32(game) % 6);
        room.role = ice_floor(game.run.floor) ? cold_roles[random_u32(game) % std::size(cold_roles)] :
            roles[random_u32(game) % std::size(roles)];
        room.shape = static_cast<RoomShape>(random_u32(game) % 8);
        room.mirrored = random_u32(game) % 2 != 0;
        // ROLES: A reservoir needs broad banks; machinery uses galleries and courts.
        if (room.role == RoomRole::Reservoir) room.shape = RoomShape::Clearing;
        if (room.role == RoomRole::Bathhouse) room.shape = RoomShape::Courtyard;
        if (room.role == RoomRole::EchoTunnel) room.shape = RoomShape::BentHall;
        if (room.role == RoomRole::Observatory) room.shape = RoomShape::Pillars;
    }
    plan.rooms[0].role = RoomRole::Entrance;
    plan.rooms[0].shape = RoomShape::Clearing;
    plan.rooms[static_cast<std::size_t>(plan.exit_room)].role = RoomRole::Exit;
    plan.rooms[static_cast<std::size_t>(plan.objective_room)].role = RoomRole::Shrine;
}

} // namespace

FloorPlan plan_floor(Game& game) {
    FloorPlan plan;
    grow_route(game, plan);
    choose_objectives(game, plan);
    describe_rooms(game, plan);
    if (random_u32(game) % 2 == 0) {
        for (std::size_t i = 1; i < plan.rooms.size(); ++i) {
            if (static_cast<int>(i) == plan.exit_room || static_cast<int>(i) == plan.objective_room) continue;
            int degree = 0;
            for (RouteEdge edge : plan.edges)
                if (edge.a == static_cast<int>(i) || edge.b == static_cast<int>(i)) ++degree;
            if (degree != 1) continue;
            plan.secret_room = static_cast<int>(i);
            plan.rooms[i].role = RoomRole::Secret;
            break;
        }
    }
    return plan;
}

bool FloorPlan::protected_cell(Cell cell) const {
    return cell.x < 0 || cell.y < 0 || cell.x >= width || cell.y >= height ||
        protected_cells[static_cast<std::size_t>(cell.y * width + cell.x)] != 0;
}

const char* room_name(RoomRole role) {
    constexpr const char* names[]{"Trailhead", "Way out", "Clearing", "Thicket", "Brook",
        "Ruined court", "Den", "Hidden cache", "Old shrine", "Workshop", "Orchard", "Secret cache",
        "Reservoir", "Fishing hut", "Bathhouse", "Ice quarry", "Observatory", "Shelter", "Echo tunnel"};
    return names[static_cast<std::size_t>(role)];
}
