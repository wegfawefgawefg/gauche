#include "industrial.hpp"
#include "interaction.hpp"
#include <array>

namespace {
bool free_space(const Game& game,const FloorPlan& plan,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || tile->prop.kind!=PropKind::None ||
        tile->kind==TileKind::Lava || tile->kind==TileKind::Water ||
        tile->kind==TileKind::ShallowWater || tile->kind==TileKind::Spring || plan.protected_cell(cell)) return false;
    for (const Entity& actor:game.entities) {
        if (actor.kind==EntityKind::None) continue;
        if (distance(actor.cell,cell)<=1) return false;
        if ((actor.kind==EntityKind::GroundItem || actor.kind==EntityKind::Player ||
            actor.kind==EntityKind::Door || actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch ||
            actor.kind==EntityKind::Exit) && distance(actor.cell,cell)<=2) return false;
    }
    return true;
}
void partition(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    struct Strip { Cell start,step; };
    constexpr std::array strips{Strip{{5,2},{0,1}},Strip{{-5,-4},{0,1}},
        Strip{{2,4},{1,0}},Strip{{-4,-4},{1,0}}};
    const auto first=random_u32(game)%strips.size();
    for (std::size_t offset=0;offset<strips.size();++offset) {
        const Strip strip=strips[(first+offset)%strips.size()];
        std::array<Cell,3> cells{}; bool okay=true;
        for (int i=0;i<3;++i) {
            const Cell cell=room.center+strip.start+Cell{strip.step.x*i,strip.step.y*i};
            cells[static_cast<std::size_t>(i)]=cell;
            if (!free_space(game,plan,cell)) { okay=false; break; }
        }
        if (!okay) continue;
        for (Cell cell:cells) place_prop(game.stage,cell,PropKind::Grate,strip.step.x==0 ? 1 : 0);
        return;
    }
}
}

void scatter_industrial_props(Game& game,const FloorPlan& plan) {
    // All placement happens after actors/loot and outside protected paths. Short
    // partitions have open ends; they are cover, never an extra mandatory lock.
    for (const RoomPlan& room:plan.rooms) {
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Shrine) continue;
        if (room.role==RoomRole::Workfront || room.role==RoomRole::BlastingAlcove) partition(game,plan,room);
        for (int bin=0;bin<2;++bin) {
            for (int attempt=0;attempt<12;++attempt) {
                const Cell cell=room.center+Cell{
                    static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_width*2))-room.half_width,
                    static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_height*2))-room.half_height};
                if (!free_space(game,plan,cell)) continue;
                bool crowded=false;
                for (Cell side:{Cell{1,0},{-1,0},{0,1},{0,-1}})
                    if (prop_blocks(game.stage.at_or_border(cell+side).prop)) crowded=true;
                if (crowded) continue;
                place_prop(game.stage,cell,bin==0 ? PropKind::ScrapBin : PropKind::OreBin);
                break;
            }
        }
    }
}
