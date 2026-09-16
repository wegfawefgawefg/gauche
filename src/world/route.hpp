#pragma once

#include "../game.hpp"
#include "generation_report.hpp"

#include <vector>

enum class RoomRole { Entrance, Exit, Clearing, Thicket, Brook, Ruins,
                      Den, Cache, Shrine, Workshop, Orchard, Secret,
                      Reservoir, FishingHut, Bathhouse, IceQuarry, Observatory, Shelter, EchoTunnel, WeatherStation, CliffPath, MemorialCourt, Chapel, CrystalGallery, ServicePassage, BoilerGallery, Workfront, BlastingAlcove, AssemblyLine, RepairBay, ScrapYard, CoolingWorks, CableTrench, KilnCourt, PayOffice, LampAlcove, SlagBank, AshLoft, HoistShaft, CastingFloor, SettlingTanks, FreightSiding };
enum class RoomShape { Clearing, Cross, BentHall, TwinCave, Courtyard, Pillars, Gallery, Steps, ChapelNave, IceShelf, ThawCavern, WorkHall, ExcavatedHall, BearHollow, SpiderCave, SnakeTunnel };

struct RoomPlan {
    Cell grid{}, center{};
    int half_width = 7, half_height = 6;
    int depth = 0;
    RoomRole role = RoomRole::Clearing;
    RoomShape shape = RoomShape::Clearing;
    bool mirrored = false;
    bool landmark=false; // Population/scenery reserved by an overlay; retain base geometry.
    int shelf_variant=0;
    int turns=0; // Quarter turns; half_width/height remain world-aligned bounds.
};

inline bool reserved_habitat(const RoomPlan& room) {
    return room.landmark || room.shape==RoomShape::BearHollow || room.shape==RoomShape::SpiderCave || room.shape==RoomShape::SnakeTunnel;
}

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
struct SpiderCave {
    std::vector<int> rooms;
    std::vector<Cell> webs,broods;
    Cell entry{},cache{};
    bool mother=false;
};
struct SnakeTunnel {
    std::vector<int> rooms;
    Cell axis{},entry{},cache{},island{},tree{};
    int crossing_length=0;
};
struct RootMaze {
    int a=0,b=0,length=0;
    Cell origin{},axis{},side{},entry{},cache{},plug{};
    bool cross_link=false;
    std::vector<Cell> roots,ground;
    std::vector<RoofSpan> passages;
};
struct GiantTree {
    std::array<int,4> rooms{};
    RoofSpan canopy{};
    Cell cache{};
    bool spiders=false;
    std::vector<Cell> entrances,ground;
    std::vector<RoofSpan> passages;
};
struct TimberGrove {
    std::array<int,4> rooms{};
    Cell center{},entry{},cache{},spring{};
    int turns=0,offset=0;
    std::vector<Cell> ground,firebreaks,trees;
};
struct FloorPlan {
    GenerationReport report;
    std::vector<TimberGrove> timber_groves;
    std::vector<GiantTree> giant_trees;
    std::vector<RootMaze> root_mazes;
    std::vector<SnakeTunnel> snake_tunnels;
    std::vector<SpiderCave> spider_caves;
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
void populate_rooms(Game& game, const FloorPlan& plan, PopulationReport* report=nullptr,GenerationReport* decisions=nullptr);
void scatter_room_props(Game& game, const FloorPlan& plan);
const char* room_name(RoomRole role);
