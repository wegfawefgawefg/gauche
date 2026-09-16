#include "open_sectors.hpp"
#include "generation_trace.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "raster.hpp"
#include "growth_carving.hpp"
#include "population_report.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
Cell scale(Cell cell,int n) {return {cell.x*n,cell.y*n};}
bool candidate(const RoomPlan& room) {
    return room.role!=RoomRole::Entrance && room.role!=RoomRole::Exit && room.role!=RoomRole::Shrine && room.role!=RoomRole::Secret;
}

}
void carve_open_sectors(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    if (!roll_generation_feature(game,plan,GenerationFeature::OpenSectors)) return;
    std::vector<RouteEdge> choices;
    for (auto edge:plan.edges) if (candidate(plan.rooms[static_cast<std::size_t>(edge.a)]) &&
        candidate(plan.rooms[static_cast<std::size_t>(edge.b)])) choices.push_back(edge);
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    if (choices.empty()) {feature_failed(plan,"No room connection clear of entrance, exit, objective and secret roles");return;}
    for (std::size_t i=choices.size();i>1;--i) std::swap(choices[i-1],choices[random_u32(game)%i]);
    const int stage=biome_stage(game.run.floor);
    const int wanted=1+static_cast<int>(random_u32(game)%static_cast<unsigned>(stage==1 ? 2 : 3));
    const WeightedComponent modes[]{{0,"Irregular meadow neck",4},{1,"Bent clearing",3},{2,"Broad basin",stage==1 ? 1U : 4U}};
    for (const auto edge:choices) {
        if (static_cast<int>(plan.open_sectors.size())>=wanted) break;
        const auto& ra=plan.rooms[static_cast<std::size_t>(edge.a)];const auto& rb=plan.rooms[static_cast<std::size_t>(edge.b)];
        const Cell a=ra.center,b=rb.center,axis=rb.grid-ra.grid,side{-axis.y,axis.x};
        const Cell middle{(a.x+b.x)/2,(a.y+b.y)/2};
        const auto roll=roll_component(game,&plan.report,GenerationFeature::OpenSectors,-1,"Sector polygon",middle,modes);
        const GenerationStep step{trace,game,plan,"Sector attempt",GenerationFeature::OpenSectors,roll.record};
        const int left=4+static_cast<int>(random_u32(game)%5),right=4+static_cast<int>(random_u32(game)%5);
        const int waist=(roll.value==2 ? 11 : 5)+static_cast<int>(random_u32(game)%5);
        const int bend=roll.value==1 ? static_cast<int>(random_u32(game)%11)-5 : 0;
        const std::array polygon{a-scale(axis,4)-scale(side,left),middle+scale(side,bend-waist),b+scale(axis,4)-scale(side,right),
            b+scale(axis,4)+scale(side,right),middle+scale(side,bend+waist),a-scale(axis,4)+scale(side,left)};
        if (roll.record>=0) {auto& child=plan.report.components[static_cast<std::size_t>(roll.record)];child.guide.assign(polygon.begin(),polygon.end());child.guide_closed=true;}
        auto shape=raster_polygon(polygon,plan.width,plan.height);
        const auto spine=raster_line(a,b,2,plan.width,plan.height);
        if (shape.truncated || spine.truncated) {component_result(&plan.report,roll,"Raster budget exceeded");continue;}
        shape.cells.insert(shape.cells.end(),spine.cells.begin(),spine.cells.end());
        struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;
        OpenSector sector;sector.component=roll.record;
        for (Cell cell:shape.cells) {
            auto* tile=game.stage.at(cell);
            if (!tile || cell.x<=0 || cell.y<=0 || cell.x>=plan.width-1 || cell.y>=plan.height-1 ||
                tile->kind!=TileKind::Wall || tile->break_rule==BreakRule::Unbreakable ||
                tile->material==TileMaterial::Root || tile->contents!=ItemKind::None || tile->prop.kind!=PropKind::None) continue;
            saved.push_back({cell,*tile});*tile={TileKind::Grass};sector.ground.push_back(cell);
        }
        const bool valid=sector.ground.size()>=32 && generation_lock_intact(game,plan);
        if (!valid) {
            for (const auto& old:saved) *game.stage.at(old.cell)=old.tile;
            component_result(&plan.report,roll,sector.ground.size()<32 ? "Too little new ground" : "Would bypass exit lock or disconnect objective");
            continue;
        }
        component_result(&plan.report,roll,"Walls opened; population pending",sector.ground);
        auto& feature=plan.report.features.back();
        Cell low{plan.width,plan.height},high{};
        for (Cell cell:sector.ground) {low.x=std::min(low.x,cell.x);low.y=std::min(low.y,cell.y);high.x=std::max(high.x,cell.x+1);high.y=std::max(high.y,cell.y+1);}
        feature.regions.push_back({low,high});plan.open_sectors.push_back(std::move(sector));
    }
    auto& feature=plan.report.features.back();
    feature.outcome=plan.open_sectors.empty() ? GenerationOutcome::Failed : GenerationOutcome::Built;
    feature.reason=plan.open_sectors.empty() ? "No candidate opened enough terrain while retaining the lock" : "Ordinary dividing walls opened; structural root walls and hazards retained";
    feature.variant=std::to_string(plan.open_sectors.size())+" accepted sectors";
}

void populate_open_sectors(Game& game,FloorPlan& plan,PopulationReport* report) {
    for (const auto& sector:plan.open_sectors) {
        auto sites=sector.ground;
        for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
        const unsigned late=game.run.floor==1 ? 0U : 2U;
        const WeightedComponent fighters[]{{static_cast<int>(EntityKind::Bat),"Bat",3},{static_cast<int>(EntityKind::Mosquito),"Mosquito",2},
            {static_cast<int>(EntityKind::Zombie),"Zombie",2},{static_cast<int>(EntityKind::Wolf),"Wolf",late},{static_cast<int>(EntityKind::Boar),"Boar",late}};
        const int wanted=std::clamp(static_cast<int>(sites.size()/50),2,8);
        std::vector<Cell> occupied;
        for (Cell cell:sites) {
            const auto* tile=game.stage.at(cell);
            if (static_cast<int>(occupied.size())>=wanted) break;
            if (!tile || !walkable(*tile) || prop_blocks(tile->prop) || entity_at(game,cell,false)>=0 || distance(cell,game.run.spawn)<8) continue;
            bool near=false;for (Cell old:occupied) if (distance(old,cell)<4) near=true;
            if (near) continue;
            const auto choice=roll_component(game,&plan.report,GenerationFeature::OpenSectors,sector.component,"Sector fighter",cell,fighters);
            auto* count=report ? &report->enemies[static_cast<std::size_t>(choice.value)] : nullptr;
            if (count) ++count->attempted;
            if (get_entity(game,spawn_entity(game,static_cast<EntityKind>(choice.value),cell))) {
                if (count) ++count->placed;
                occupied.push_back(cell);component_result(&plan.report,choice,"Fighter placed",std::array{cell});
            } else {
                if (count) ++count->rejected;
                component_result(&plan.report,choice,"Entity capacity exhausted");
            }
        }
        int scenery=0;
        for (Cell cell:sites) {
            auto* tile=game.stage.at(cell);
            if (!tile || tile->kind!=TileKind::Grass || tile->prop.kind!=PropKind::None || entity_at(game,cell,false)>=0) continue;
            if (random_u32(game)%100>=42) continue;
            const auto kind=random_u32(game)%4==0 ? PropKind::Puffball : random_u32(game)%2 ? PropKind::Fern : PropKind::Twigs;
            place_prop(game.stage,cell,kind,static_cast<std::uint8_t>(random_u32(game)%4));
            ++scenery;
        }
        if (sector.component>=0) plan.report.components[static_cast<std::size_t>(sector.component)].result=
            "Walls opened; fighters "+std::to_string(occupied.size())+"/"+std::to_string(wanted)+
            "; ground props "+std::to_string(scenery);
    }
}
