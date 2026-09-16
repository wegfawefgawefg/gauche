#include "light_towers.hpp"
#include "../props/light_tower.hpp"
#include "../props/interaction.hpp"
#include "../props/streetlamp.hpp"
#include "../surfaces/interaction.hpp"
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool workroom(RoomRole role) {
    switch (role) {
    case RoomRole::Workshop: case RoomRole::Workfront: case RoomRole::AssemblyLine:
    case RoomRole::RepairBay: case RoomRole::ScrapYard: case RoomRole::BoilerGallery:
    case RoomRole::HoistShaft: case RoomRole::CastingFloor: case RoomRole::FreightSiding:
    case RoomRole::CoolingWorks: case RoomRole::LampAlcove: return true;
    default:return false;
    }
}
bool quest(const Game& game,Cell cell,int range) {
    for (const auto& actor:game.entities) if (distance(actor.cell,cell)<range &&
        (actor.kind==EntityKind::Player || actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch ||
         actor.kind==EntityKind::Door || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate)) return true;
    return false;
}
}
void place_light_towers(Game& game,const FloorPlan& plan) {
    if (floor_biome(game.run.floor)!=Biome::Industrial || plan.rooms.empty()) return;
    const int budget=2+static_cast<int>(random_u32(game)%3);
    const auto first=random_u32(game)%plan.rooms.size();std::vector<Cell> placed;
    for (std::size_t i=0;i<plan.rooms.size() && static_cast<int>(placed.size())<budget;++i) {
        const auto& room=plan.rooms[(first+i)%plan.rooms.size()];if (!workroom(room.role)) continue;
        for (int attempt=0;attempt<48;++attempt) {
            const Cell cell=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_width*2-3))-room.half_width+2,
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_height*2-3))-room.half_height+2};
            const Tile* tile=game.stage.at(cell);
            if (!tile || !streetlamp_ground(tile->kind) || !walkable(*tile) || surface_wet(*tile) ||
                tile->surface.fire_ticks || tile->prop.kind!=PropKind::None || plan.protected_cell(cell) ||
                entity_at(game,cell,false)>=0 || quest(game,cell,6)) continue;
            bool close=false;for (Cell other:placed) if (distance(cell,other)<10) close=true;
            if (close) continue;
            // The heavy light rack leans over a real work lane; its orientation
            // is saved at placement, not chosen randomly during collapse.
            const int start=static_cast<int>(random_u32(game)%4);int direction=-1;
            for (int turn=0;turn<4 && direction<0;++turn) {
                const int index=(start+turn)%4;const Cell dir=directions[index];bool clear=true;
                for (int n=1;n<=tower_reach;++n) {
                    const Cell target=cell+Cell{dir.x*n,dir.y*n};const Tile& ground=game.stage.at_or_border(target);
                    if (!streetlamp_ground(ground.kind) || !walkable(ground) || surface_wet(ground) || quest(game,target,2)) clear=false;
                }
                if (clear) direction=index;
            }
            if (direction<0 || !place_prop(game.stage,cell,PropKind::LightTower,static_cast<std::uint8_t>(direction))) continue;
            if (!floor_reachable(game)) {game.stage.at(cell)->prop={};continue;}
            placed.push_back(cell);break;
        }
    }
}
