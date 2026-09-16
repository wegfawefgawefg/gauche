#pragma once

#include "../game.hpp"

#include <vector>

enum class RoomRole { Entrance, Exit, Clearing, Thicket, Brook, Ruins,
                      Den, Cache, Shrine, Workshop, Orchard, Secret,
                      Reservoir, FishingHut, Bathhouse, IceQuarry, Observatory, Shelter, EchoTunnel, WeatherStation, CliffPath, MemorialCourt, Chapel, CrystalGallery, ServicePassage, BoilerGallery, Workfront, BlastingAlcove, AssemblyLine, RepairBay, ScrapYard, CoolingWorks, CableTrench, KilnCourt, PayOffice, LampAlcove, SlagBank, AshLoft, HoistShaft, CastingFloor, SettlingTanks, FreightSiding };
enum class RoomShape { Clearing, Cross, BentHall, TwinCave, Courtyard, Pillars, Gallery, Steps, ChapelNave, IceShelf, ThawCavern, WorkHall, ExcavatedHall, BearHollow };

struct RoomPlan {
    Cell grid{}, center{};
    int half_width = 7, half_height = 6;
    int depth = 0;
    RoomRole role = RoomRole::Clearing;
    RoomShape shape = RoomShape::Clearing;
    bool mirrored = false;
    int shelf_variant=0;
};

struct RouteEdge { int a = 0, b = 0; };
struct ShelfReward { Cell island{},bank{},direction{}; };
struct ThawChannel {
    int a=0,b=0;
    Cell along{},across{},source{},mouth{},bank{},prize{},frozen{},eel{};
};
struct IndustrialLink {
    int a=0,b=0;
    Cell along{},across{},middle{},load{},unload{};
    std::vector<Cell> belt;
};
struct ForestDen {
    int a=0,b=0;
    Cell along{},across{},spring{},cache{};
    std::vector<Cell> beds;
};
struct FloorPlan {
    std::vector<ForestDen> forest_dens;
    std::vector<RoomPlan> rooms;
    std::vector<RouteEdge> edges;
    std::vector<ShelfReward> shelf_rewards;
    int shelf_links=0;
    std::vector<ThawChannel> thaw_channels;
    int industry_profile=0;
    std::vector<IndustrialLink> industrial_links;
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
struct PopulationReport;
void populate_rooms(Game& game, const FloorPlan& plan, PopulationReport* report=nullptr);
void scatter_room_props(Game& game, const FloorPlan& plan);
const char* room_name(RoomRole role);
