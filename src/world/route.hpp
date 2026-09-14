#pragma once

#include "../game.hpp"

#include <vector>

enum class RoomRole { Entrance, Exit, Clearing, Thicket, Brook, Ruins,
                      Den, Cache, Shrine, Workshop, Orchard, Secret,
                      Reservoir, FishingHut, Bathhouse, IceQuarry, Observatory, Shelter, EchoTunnel, WeatherStation, CliffPath, MemorialCourt, Chapel };
enum class RoomShape { Clearing, Cross, BentHall, TwinCave, Courtyard, Pillars, Gallery, Steps };

struct RoomPlan {
    Cell grid{}, center{};
    int half_width = 7, half_height = 6;
    int depth = 0;
    RoomRole role = RoomRole::Clearing;
    RoomShape shape = RoomShape::Clearing;
    bool mirrored = false;
};

struct RouteEdge { int a = 0, b = 0; };
struct FloorPlan {
    std::vector<RoomPlan> rooms;
    std::vector<RouteEdge> edges;
    std::vector<std::uint8_t> protected_cells;
    int exit_room = 0, objective_room = 0;
    int width = 0, height = 0;
    int secret_room = -1;
    Cell door{}, secret_door{};
    bool protected_cell(Cell cell) const;
};

FloorPlan plan_floor(Game& game);
void carve_floor(Game& game, FloorPlan& plan);
void place_forest_terrain(Game& game, const FloorPlan& plan);
void place_room_gates(Game& game, FloorPlan& plan);
bool floor_lock_required(const Game& game);
void populate_rooms(Game& game, const FloorPlan& plan);
void scatter_room_props(Game& game, const FloorPlan& plan);
const char* room_name(RoomRole role);
