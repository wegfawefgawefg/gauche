#include "fissures.hpp"
#include "route.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../scenery/roof.hpp"
#include <vector>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
Cell offset(Cell cell,Cell direction,int n) {return cell+Cell{direction.x*n,direction.y*n};}
bool dry(const Tile& tile) {
    return (tile.kind==TileKind::Empty || tile.kind==TileKind::Grass || tile.kind==TileKind::Ruin) &&
        tile.prop.kind==PropKind::None && !surface_wet(tile) && !tile.surface.fire_ticks;
}
bool near_quest(const Game& game,Cell cell) {
    if (distance(cell,game.run.spawn)<9) return true;
    for (const auto& actor:game.entities) if (distance(actor.cell,cell)<6 &&
        (actor.kind==EntityKind::Player || actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch ||
         actor.kind==EntityKind::Door || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate)) return true;
    return false;
}
bool near_fissure(const Game& game,Cell cell) {
    for (const auto& f:game.fissures) if (distance(f.center,cell)<10) return true;
    return false;
}
bool occupied(const Game& game,Cell cell) {
    if (entity_at(game,cell,false)>=0) return true;
    for (const auto& roof:game.stage.roofs) for (int n=0;n<roof.length;++n)
        for (int w=0;w<3;++w) if (roof_cell(roof,n,w)==cell) return true;
    return false;
}
Fissure make(Game& game,Cell center,Cell axis,FissureKind kind) {
    Fissure f;f.center=center;f.axis=axis;f.kind=kind;
    f.ticks=static_cast<std::uint16_t>(180+random_u32(game)%241);return f;
}
struct Recess {Cell approach{},outward{};};
bool recess_fits(const Game& game,const FloorPlan& plan,const Recess& site) {
    const Cell across{-site.outward.y,site.outward.x};
    // A sealed 3x3 pocket beyond an existing bank cannot connect two rooms or
    // bypass an exit lock. Leave both flanks and its far wall intact.
    for (int depth=1;depth<=4;++depth) for (int width=-2;width<=2;++width) {
        const Cell cell=offset(offset(site.approach,site.outward,depth),across,width);
        const Tile* tile=game.stage.at(cell);
        if (!tile || cell.x<2 || cell.y<2 || cell.x>=game.stage.width-2 || cell.y>=game.stage.height-2 ||
            tile->kind!=TileKind::Wall || tile->break_rule==BreakRule::Unbreakable ||
            tile->contents!=ItemKind::None || tile->prop.kind!=PropKind::None || plan.protected_cell(cell)) return false;
    }
    for (int width=-1;width<=1;++width) {
        const Cell cell=offset(site.approach,across,width);
        if (!dry(game.stage.at_or_border(cell)) || occupied(game,cell)) return false;
    }
    for (int depth=1;depth<=3;++depth) for (int width=-1;width<=1;++width)
        if (occupied(game,offset(offset(site.approach,site.outward,depth),across,width))) return false;
    return !near_quest(game,site.approach);
}
bool make_recess(Game& game,const FloorPlan& plan) {
    int free=0;for (const auto& e:game.entities) free+=e.kind==EntityKind::None;
    if (free<2) return false;
    // An optional cache must still have a walking approach in the real map.
    // Mobile actors and objective doors can be dealt with during the run.
    std::vector<bool> reached(game.stage.tiles.size(),false);std::vector<Cell> pending{game.run.spawn};
    for (std::size_t i=0;i<pending.size();++i) {
        const Cell cell=pending[i];const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile)) continue;
        const auto index=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if (reached[index]) continue;
        reached[index]=true;for (Cell side:sides) pending.push_back(cell+side);
    }
    std::vector<Recess> choices;
    for (int y=3;y<game.stage.height-3;++y) for (int x=3;x<game.stage.width-3;++x) {
        const Cell cell{x,y};
        if (!reached[static_cast<std::size_t>(y*game.stage.width+x)] || !dry(game.stage.at_or_border(cell))) continue;
        for (Cell side:sides) if (recess_fits(game,plan,{cell,side})) choices.push_back({cell,side});
    }
    if (choices.empty()) return false;
    const auto site=choices[random_u32(game)%choices.size()];
    const Cell across{-site.outward.y,site.outward.x};
    std::vector<Tile> before;
    for (int depth=1;depth<=3;++depth) for (int width=-1;width<=1;++width) {
        auto& tile=*game.stage.at(offset(offset(site.approach,site.outward,depth),across,width));
        before.push_back(tile);tile={TileKind::Ruin};
    }
    if (!floor_reachable(game) || !floor_lock_required(game)) {
        std::size_t i=0;for (int d=1;d<=3;++d) for (int w=-1;w<=1;++w)
            *game.stage.at(offset(offset(site.approach,site.outward,d),across,w))=before[i++];
        return false;
    }
    const Cell axis{across.x<0 ? -across.x : across.x,across.y<0 ? -across.y : across.y};
    game.fissures.push_back(make(game,offset(site.approach,site.outward,1),axis,
        random_u32(game)%2 ? FissureKind::Steam : FissureKind::Lava));
    constexpr ItemKind rewards[]{ItemKind::PressHammer,ItemKind::ArcTorch,ItemKind::RivetGun,ItemKind::HeatSiphon};
    place_ground_item(game,offset(site.approach,site.outward,3),rewards[random_u32(game)%4]);
    place_ground_item(game,offset(site.approach,across,-1),ItemKind::PocketPump);
    place_prop(game.stage,offset(offset(site.approach,site.outward,2),across,1),PropKind::ScrapBin);
    place_prop(game.stage,offset(offset(site.approach,site.outward,3),across,-1),PropKind::Crate);
    return true;
}
}

void place_fissures(Game& game,const FloorPlan& plan) {
    if (!industrial_floor(game.run.floor) || plan.rooms.empty()) return;
    make_recess(game,plan);
    const int budget=4+biome_stage(game.run.floor);
    const auto first=random_u32(game)%plan.rooms.size();
    for (std::size_t i=0;i<plan.rooms.size() && static_cast<int>(game.fissures.size())<budget;++i) {
        const auto& room=plan.rooms[(i+first)%plan.rooms.size()];
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Shrine) continue;
        for (int attempt=0;attempt<40;++attempt) {
            const Cell center=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_width*2-3))-room.half_width+2,
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_height*2-3))-room.half_height+2};
            if (near_fissure(game,center) || near_quest(game,center)) continue;
            const Cell axis=random_u32(game)%2 ? Cell{1,0} : Cell{0,1};const Cell across{-axis.y,axis.x};
            bool clear=true;
            for (int n=-1;n<=1 && clear;++n) for (int w=-1;w<=1 && clear;++w) {
                const Cell cell=offset(offset(center,axis,n),across,w);
                if (!dry(game.stage.at_or_border(cell)) || plan.protected_cell(cell) || occupied(game,cell)) clear=false;
            }
            if (!clear) continue;
            const auto kind=game.fissures.size()%2 ? FissureKind::Steam : FissureKind::Lava;
            game.fissures.push_back(make(game,center,axis,kind));break;
        }
    }
}
