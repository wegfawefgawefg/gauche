#include "ice_pillars.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <cstdlib>
#include <vector>

void place_ice_pillars(Game& game,const FloorPlan& plan) {
    if (!ice_floor(game.run.floor) || plan.rooms.empty()) return;
    const int budget=3+static_cast<int>(random_u32(game)%3);
    const auto first=random_u32(game)%plan.rooms.size();
    std::vector<Cell> placed;
    for (std::size_t index=0;index<plan.rooms.size() && static_cast<int>(placed.size())<budget;++index) {
        const RoomPlan& room=plan.rooms[(first+index)%plan.rooms.size()];
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Shrine ||
            room.role==RoomRole::Chapel || room.role==RoomRole::Shelter || room.role==RoomRole::FishingHut || room.role==RoomRole::Bathhouse) continue;
        for (int attempt=0;attempt<48;++attempt) {
            const Cell cell=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_width*2-3))-room.half_width+2,
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_height*2-3))-room.half_height+2};
            bool okay=true;
            for (Cell previous:placed) if (distance(previous,cell)<7) okay=false;
            for (int y=-1;y<=1 && okay;++y) for (int x=-1;x<=1 && okay;++x) {
                const Cell p=cell+Cell{x,y};const Tile* tile=game.stage.at(p);
                if (!tile || !walkable(*tile) || surface_wet(*tile) || tile->kind==TileKind::Lava ||
                    tile->prop.kind!=PropKind::None || entity_at(game,p,false)>=0 || plan.protected_cell(p)) okay=false;
            }
            for (const Entity& e:game.entities) if (distance(e.cell,cell)<4 &&
                (e.kind==EntityKind::Player || e.kind==EntityKind::Key || e.kind==EntityKind::Switch ||
                 e.kind==EntityKind::Door || e.kind==EntityKind::Exit || e.kind==EntityKind::EncounterGate)) okay=false;
            if (!okay) continue;
            place_prop(game.stage,cell,PropKind::IcePillar);
            if (!floor_reachable(game)) {game.stage.at(cell)->prop={};continue;}
            placed.push_back(cell);break;
        }
    }
}
