#include "forest_den.hpp"
#include "feature_roll.hpp"
#include "terrain_material.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
Cell scale(Cell cell,int n) {return {cell.x*n,cell.y*n};}
bool eligible(const FloorPlan& plan,int index) {
    return index!=0 && index!=plan.exit_room && index!=plan.secret_room && index!=plan.objective_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(index)]);
}
void reserve(FloorPlan& plan,Cell cell) {
    plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
}
void paint(Game& game,FloorPlan& plan,Cell cell,TileKind kind) {
    if (auto* tile=game.stage.at(cell)) {
        *tile={kind};reserve(plan,cell);
    }
}
Cell site(const FloorPlan& plan,const ForestDen& den,int room,int along,int across) {
    return plan.rooms[static_cast<std::size_t>(room)].center+
        scale(den.along,along)+scale(den.across,across);
}
}

void plan_forest_den(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::BearDen)) return;
    std::vector<RouteEdge> choices;
    for (const auto edge:plan.edges)
        if (eligible(plan,edge.a) && eligible(plan,edge.b)) choices.push_back(edge);
    if (choices.empty()) { feature_failed(plan,"No adjacent eligible room pair after objective and earlier habitat reservations"); return; }
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    auto edge=choices[random_u32(game)%choices.size()];
    // Put the sleeping hollow deeper in the route; the broad stream bank is its
    // approach. Existing corridors remain usable even if this is a through area.
    if (plan.rooms[static_cast<std::size_t>(edge.a)].depth>plan.rooms[static_cast<std::size_t>(edge.b)].depth)
        std::swap(edge.a,edge.b);
    ForestDen den;den.a=edge.a;den.b=edge.b;
    den.along=plan.rooms[static_cast<std::size_t>(edge.b)].grid-plan.rooms[static_cast<std::size_t>(edge.a)].grid;
    den.across={-den.along.y,den.along.x};
    if (random_u32(game)%2) den.across=scale(den.across,-1);
    for (int index:{den.a,den.b}) {
        auto& room=plan.rooms[static_cast<std::size_t>(index)];
        room.shape=RoomShape::BearHollow;room.role=RoomRole::Den;
        room.half_width=room.half_height=9;
    }
    plan.forest_dens.push_back(den);
    feature_reserved(plan,std::array{den.a,den.b},"Stream approach / sleeping hollow");
}

void carve_forest_den(Game& game,FloorPlan& plan) {
    for (auto& den:plan.forest_dens) {
        const auto a=plan.rooms[static_cast<std::size_t>(den.a)].center;
        const auto b=plan.rooms[static_cast<std::size_t>(den.b)].center;
        const int length=std::abs((b.x-a.x)*den.along.x+(b.y-a.y)*den.along.y);
        // Widen the former connecting corridor into an uneven shared bank. Its
        // center is dry and uninterrupted; stream and sleeping bays are lateral.
        for (int i=0;i<=length;++i) {
            const Cell center{a.x+(b.x-a.x)*i/length,a.y+(b.y-a.y)*i/length};
            const int width=4+(i<6 || i>length-6 ? 2 : (i/4)%2);
            for (int side=-width;side<=width;++side)
                paint(game,plan,center+scale(den.across,side),std::abs(side)<2 ? TileKind::Empty : TileKind::Grass);
            if (i>1 && i<length-1)
                paint(game,plan,center+scale(den.across,3+(i/5)%2),TileKind::ShallowWater);
        }
        den.spring=site(plan,den,den.a,-4,4);
        for (int along=-4;along<=3;++along) for (int across=3;across<=5;++across)
            paint(game,plan,site(plan,den,den.a,along,across),TileKind::ShallowWater);
        paint(game,plan,den.spring,TileKind::Spring);
        // Dry circular beds on the sheltered side, separated by cuttable roots.
        std::vector<Cell> bays{{-6,-4},{-3,-6},{1,-6},{5,-5},{-6,0},{6,0},{-3,-2},{2,-2}};
        for (std::size_t i=bays.size();i>1;--i) std::swap(bays[i-1],bays[random_u32(game)%i]);
        const std::size_t count=game.run.floor==1 ? 3+random_u32(game)%2 : 4+random_u32(game)%5;
        for (std::size_t i=0;i<count;++i) {
            const Cell bay=bays[i];
            const Cell bed=site(plan,den,den.b,bay.x,bay.y);den.beds.push_back(bed);
            for (int y=-1;y<=1;++y) for (int x=-1;x<=1;++x)
                paint(game,plan,bed+Cell{x,y},TileKind::Grass);
        }
        den.cache=site(plan,den,den.b,random_u32(game)%2 ? 5 : -5,-7);
        paint(game,plan,den.cache,TileKind::Grass);
        for (int along:{-2,2}) {
            const Cell cell=site(plan,den,den.b,along,-6);
            if (!plan.protected_cell(cell)) *game.stage.at(cell)=wood_tile(TileMaterial::Tree);
        }
        // Reserve the whole authored footprint, including walls. Later scenery
        // cannot plant pillars, scatter junk over beds or hollow out another roof.
        for (int index:{den.a,den.b}) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            for (int y=-9;y<=9;++y) for (int x=-9;x<=9;++x) {
                const Cell cell=room.center+Cell{x,y};reserve(plan,cell);
                auto& tile=*game.stage.at(cell);
                if (tile.kind==TileKind::Wall && (x+y)%3!=0) tile=wood_tile(TileMaterial::Tree);
            }
        }
    }
}
