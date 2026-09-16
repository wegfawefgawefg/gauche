#include "tall_trees.hpp"
#include "../props/tall_tree.hpp"
#include "../props/interaction.hpp"
#include "../items/folded_bridge.hpp"
#include "../surfaces/interaction.hpp"
#include <array>
#include <vector>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool space(const Game& game,const FloorPlan& plan,Cell cell,const std::vector<Cell>& placed) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !bridge_bank(*tile) || surface_wet(*tile) || tile->prop.kind!=PropKind::None ||
        entity_at(game,cell,false)>=0 || plan.protected_cell(cell)) return false;
    for (Cell old:placed) if (distance(old,cell)<8) return false;
    for (const Entity& actor:game.entities) if (distance(actor.cell,cell)<5 &&
        (actor.kind==EntityKind::Player || actor.kind==EntityKind::Door || actor.kind==EntityKind::Key ||
         actor.kind==EntityKind::Switch || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate)) return false;
    return true;
}
bool place(Game& game,const FloorPlan& plan,Cell cell,int facing,std::vector<Cell>& placed) {
    if (!space(game,plan,cell,placed) || !place_prop(game.stage,cell,PropKind::TallTree,static_cast<std::uint8_t>(facing))) return false;
    bool valid=floor_reachable(game);
    // Potential future crossings must respect the same exit lock as the floor.
    // Test every cut direction; a player's blow can override the planted lean.
    for (Cell dir:directions) {
        const int length=tree_bridge_length(game,cell,dir);if (!length) continue;
        std::array<Tile,3> saved{};
        for (int i=0;i<length;++i) saved[static_cast<std::size_t>(i)]=*game.stage.at(cell+Cell{dir.x*(i+1),dir.y*(i+1)});
        lay_tree_bridge(game,cell,dir);
        const auto tree=game.stage.at(cell)->prop;game.stage.at(cell)->prop={};
        valid=valid && floor_lock_required(game);
        game.stage.at(cell)->prop=tree;
        for (int i=0;i<length;++i) *game.stage.at(cell+Cell{dir.x*(i+1),dir.y*(i+1)})=saved[static_cast<std::size_t>(i)];
    }
    if (!valid) {game.stage.at(cell)->prop={};return false;}
    placed.push_back(cell);return true;
}
}
void place_tall_trees(Game& game,const FloorPlan& plan) {
    if (!forest_floor(game.run.floor) || plan.rooms.empty()) return;
    std::vector<Cell> placed;
    const int budget=2+static_cast<int>(random_u32(game)%3);
    // Prefer one actual bank-to-bank crossing. Nothing is relocated or carved
    // to force it; ordinary water/gap geometry supplies the opportunity.
    const int size=game.stage.width*game.stage.height;
    const int first=static_cast<int>(random_u32(game)%static_cast<unsigned int>(size));
    for (int i=0;i<size && placed.empty();++i) {
        const int index=(first+i)%size;const Cell cell{index%game.stage.width,index/game.stage.width};
        if (!space(game,plan,cell,placed)) continue;
        for (int facing=0;facing<4;++facing) {
            const Cell behind=cell-directions[facing];
            if (!walkable(game.stage.at_or_border(behind)) || entity_at(game,behind,false)>=0 ||
                tree_bridge_length(game,cell,directions[facing])==0) continue;
            if (place(game,plan,cell,facing,placed)) break;
        }
    }
    const auto start=random_u32(game)%plan.rooms.size();
    for (std::size_t i=0;i<plan.rooms.size() && static_cast<int>(placed.size())<budget;++i) {
        const auto& room=plan.rooms[(start+i)%plan.rooms.size()];
        if (room.role!=RoomRole::Clearing && room.role!=RoomRole::Thicket && room.role!=RoomRole::Brook) continue;
        for (int attempt=0;attempt<32;++attempt) {
            const Cell cell=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_width*2-3))-room.half_width+2,
                static_cast<int>(random_u32(game)%static_cast<unsigned int>(room.half_height*2-3))-room.half_height+2};
            if (place(game,plan,cell,static_cast<int>(random_u32(game)%4),placed)) break;
        }
    }
}
