#include "route.hpp"
#include "room_frame.hpp"
#include "forest_den.hpp"
#include "giant_tree.hpp"
#include "timber_grove.hpp"
#include "spider_cave.hpp"
#include "snake_tunnel.hpp"
#include "root_maze.hpp"
#include "industrial_geometry.hpp"
#include "ice_shelves.hpp"
#include "ice_thaw.hpp"
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
        RoomRole::IceQuarry, RoomRole::Observatory, RoomRole::Shelter, RoomRole::EchoTunnel, RoomRole::Cache, RoomRole::WeatherStation, RoomRole::CliffPath, RoomRole::MemorialCourt, RoomRole::Chapel, RoomRole::CrystalGallery, RoomRole::ServicePassage, RoomRole::BoilerGallery};
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
        if (room.role == RoomRole::Reservoir) {
            room.shape = RoomShape::Clearing;
            room.half_width = std::max(room.half_width,8);
            room.half_height = std::max(room.half_height,7);
        }
        if (room.role==RoomRole::FishingHut) {
            room.shape=RoomShape::Clearing;
            room.half_width=std::max(room.half_width,8);
            room.half_height=std::max(room.half_height,7);
        }
        if (room.role == RoomRole::Bathhouse) room.shape = RoomShape::Courtyard;
        if (room.role == RoomRole::EchoTunnel) room.shape = RoomShape::BentHall;
        if (room.role == RoomRole::CrystalGallery) room.shape = RoomShape::Clearing;
        if (room.role == RoomRole::BoilerGallery) room.shape = RoomShape::Pillars;
        if (room.role == RoomRole::ServicePassage) room.shape = RoomShape::Gallery;
        if (room.role == RoomRole::Chapel) {
            room.shape = RoomShape::ChapelNave;
            room.half_width = std::max(room.half_width,7);
            room.half_height = std::max(room.half_height,6);
        }
        if (room.role == RoomRole::MemorialCourt) room.shape = RoomShape::Courtyard;
        if (room.role == RoomRole::CliffPath) room.shape = RoomShape::Steps;
        if (room.role == RoomRole::WeatherStation) room.shape = RoomShape::Courtyard;
        if (room.role == RoomRole::Observatory) room.shape = RoomShape::Pillars;
    }
    plan.rooms[0].role = RoomRole::Entrance;
    plan.rooms[0].shape = RoomShape::Clearing;
    plan.rooms[static_cast<std::size_t>(plan.exit_room)].role = RoomRole::Exit;
    plan.rooms[static_cast<std::size_t>(plan.objective_room)].role = RoomRole::Shrine;
    if (industrial_floor(game.run.floor)) {
        for (std::size_t i=1;i<plan.rooms.size();++i) {
            if (static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.objective_room ||
                static_cast<int>(i)==plan.secret_room) continue;
            RoomPlan& room=plan.rooms[i];
            room.role=RoomRole::Workfront; room.shape=RoomShape::Courtyard;
            room.half_width=9; room.half_height=7;
            break;
        }
        for (std::size_t i=1;i<plan.rooms.size();++i) {
            auto& room=plan.rooms[i];
            if (static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.objective_room || room.role==RoomRole::Workfront) continue;
            room.role=RoomRole::BlastingAlcove; room.shape=RoomShape::Clearing;
            room.half_width=8; room.half_height=6;
            break;
        }
        for (std::size_t i=1;i<plan.rooms.size();++i) {
            auto& room=plan.rooms[i];
            if (static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.objective_room ||
                room.role==RoomRole::Workfront || room.role==RoomRole::BlastingAlcove) continue;
            room.role=RoomRole::AssemblyLine; room.shape=RoomShape::Clearing;
            room.half_width=9; room.half_height=7;
            break;
        }
        // Ordinary installations are a rotation, not one rare roll shared by the
        // whole catalog. Keep separate rooms for work crews, blasting and belts.
        std::array installations{RoomRole::RepairBay,RoomRole::CoolingWorks,RoomRole::CableTrench,
            RoomRole::KilnCourt,RoomRole::PayOffice,RoomRole::LampAlcove,RoomRole::SlagBank,
            RoomRole::AshLoft,RoomRole::HoistShaft,RoomRole::CastingFloor,RoomRole::SettlingTanks,
            RoomRole::FreightSiding,RoomRole::ScrapYard};
        for (std::size_t i=installations.size();i>1;--i)
            std::swap(installations[i-1],installations[random_u32(game)%i]);
        std::vector<std::size_t> candidates;
        for (std::size_t i=1;i<plan.rooms.size();++i) {
            const auto role=plan.rooms[i].role;
            if (static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.objective_room ||
                role==RoomRole::Workfront || role==RoomRole::BlastingAlcove || role==RoomRole::AssemblyLine) continue;
            candidates.push_back(i);
        }
        for (std::size_t i=candidates.size();i>1;--i)
            std::swap(candidates[i-1],candidates[random_u32(game)%i]);
        const std::size_t count=std::min(candidates.size(),static_cast<std::size_t>(2+(game.run.floor-1)%4/2));
        for (std::size_t i=0;i<count;++i) {
            RoomPlan& room=plan.rooms[candidates[i]];
            room.role=installations[i];room.shape=RoomShape::Clearing;
            room.half_width=8;room.half_height=7;
        }
    }

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
            if (degree != 1 || plan.rooms[i].role==RoomRole::Workfront || plan.rooms[i].role==RoomRole::BlastingAlcove || plan.rooms[i].role==RoomRole::AssemblyLine || plan.rooms[i].role==RoomRole::RepairBay || plan.rooms[i].role==RoomRole::ScrapYard || plan.rooms[i].role==RoomRole::CoolingWorks || plan.rooms[i].role==RoomRole::CableTrench || plan.rooms[i].role==RoomRole::KilnCourt || plan.rooms[i].role==RoomRole::PayOffice || plan.rooms[i].role==RoomRole::LampAlcove || plan.rooms[i].role==RoomRole::SlagBank || plan.rooms[i].role==RoomRole::AshLoft || plan.rooms[i].role==RoomRole::HoistShaft || plan.rooms[i].role==RoomRole::CastingFloor || plan.rooms[i].role==RoomRole::SettlingTanks || plan.rooms[i].role==RoomRole::FreightSiding) continue;
            plan.secret_room = static_cast<int>(i);
            plan.rooms[i].role = RoomRole::Secret;
            break;
        }
    }
    plan_giant_tree(game,plan);
    plan_timber_grove(game,plan);
    plan_forest_den(game,plan);
    plan_spider_cave(game,plan);
    plan_snake_tunnel(game,plan);
    plan_root_maze(game,plan);
    plan_industrial_geometry(game,plan);
    plan_ice_shelves(game,plan);
    plan_ice_thaw(game,plan);
    for (auto& room:plan.rooms) if (socket_room(game,room)) {
        room.turns=static_cast<int>(random_u32(game)%4);
        if (room.turns&1) std::swap(room.half_width,room.half_height);
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
        "Reservoir", "Fishing hut", "Bathhouse", "Ice quarry", "Observatory", "Shelter", "Echo tunnel", "Weather station", "Cliff path", "Memorial court", "Candle chapel", "Crystal gallery", "Service passages", "Boiler gallery", "Work front", "Blasting alcove", "Assembly line", "Repair bay", "Scrap yard", "Cooling works", "Cable trench", "Kiln court", "Pay office", "Lamp alcove", "Slag bank", "Ash loft", "Hoist shaft", "Casting floor", "Settling tanks", "Freight siding"};
    static_assert(std::size(names) == static_cast<std::size_t>(RoomRole::FreightSiding) + 1);
    return names[static_cast<std::size_t>(role)];
}
