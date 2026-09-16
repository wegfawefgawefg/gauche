#include "loose_finds.hpp"
#include "feature_roll.hpp"
#include "generation_trace.hpp"
#include "supply_rolls.hpp"
#include "population_report.hpp"
#include "../scenery/roof.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::ForestFinds;
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
std::size_t index(const Stage& stage,Cell c) {return static_cast<std::size_t>(c.y*stage.width+c.x);}
std::vector<bool> reachable(const Game& game) {
    std::vector<bool> seen(game.stage.tiles.size());std::vector<Cell> queue{game.run.spawn};
    for (std::size_t next=0;next<queue.size();++next) {
        const Cell cell=queue[next];const auto* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || seen[index(game.stage,cell)]) continue;
        seen[index(game.stage,cell)]=true;
        for (Cell d:sides) queue.push_back(cell+d);
    }
    return seen;
}
bool plain_ground(const Tile& tile) {
    return (tile.kind==TileKind::Grass || tile.kind==TileKind::Empty || tile.kind==TileKind::Ruin || tile.kind==TileKind::Snow) &&
        (tile.prop.kind==PropKind::None || tile.prop.kind==PropKind::Leaves || tile.prop.kind==PropKind::Twigs ||
         tile.prop.kind==PropKind::Fern || tile.prop.kind==PropKind::TallGrass) &&
        tile.surface.liquid==LiquidKind::None && !tile.surface.fire_ticks;
}
}

// Late finds add independent sockets, never refill a landmark's reward or spend
// capacity reserved for fighters. Wall contents keep their ordinary extraction.
void place_forest_loose_finds(Game& game,FloorPlan& plan,PopulationReport* report,GenerationTrace* trace) {
    if (!roll_generation_feature(game,plan,feature)) return;
    auto& decision=plan.report.features.back();
    const auto reached=reachable(game);
    std::vector<bool> forbidden(game.stage.tiles.size());
    const auto exclude=[&](Cell center,int radius) {
        for (int y=center.y-radius;y<=center.y+radius;++y) for (int x=center.x-radius;x<=center.x+radius;++x)
            if (game.stage.in_bounds({x,y}) && distance(center,{x,y})<=radius) forbidden[index(game.stage,{x,y})]=true;
    };
    for (const auto& room:plan.rooms) if (reserved_habitat(room) || room.role==RoomRole::Entrance ||
        room.role==RoomRole::Exit || room.role==RoomRole::Shrine || room.role==RoomRole::Secret)
        for (int y=room.center.y-room.half_height-2;y<=room.center.y+room.half_height+2;++y)
            for (int x=room.center.x-room.half_width-2;x<=room.center.x+room.half_width+2;++x)
                if (game.stage.in_bounds({x,y})) forbidden[index(game.stage,{x,y})]=true;
    for (const auto& actor:game.entities) if (actor.kind!=EntityKind::None) {
        exclude(actor.cell,actor.kind==EntityKind::GroundItem || actor.kind==EntityKind::Coins ? 6 : 0);
        if (actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch || actor.kind==EntityKind::Door) exclude(actor.cell,5);
        if (actor.kind==EntityKind::OldGrowthBear) exclude(actor.cell,10);
    }
    exclude(game.run.spawn,10);exclude(game.run.exit,6);
    std::array<std::vector<Cell>,2> sites;
    for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
        const Cell cell{x,y};const auto i=index(game.stage,cell);const auto& tile=game.stage.tiles[i];
        if (forbidden[i] || plan.protected_cell(cell) || tile.contents!=ItemKind::None) continue;
        bool covered=false;for (const auto& roof:game.stage.roofs) if (roof_covers(roof,cell)) {covered=true;break;}
        if (covered) continue;
        if (reached[i] && plain_ground(tile)) sites[0].push_back(cell);
        else if (tile.kind==TileKind::Wall && tile.hp>0 && tile.break_rule!=BreakRule::Unbreakable &&
            tile.material!=TileMaterial::Root && tile.prop.kind==PropKind::None) {
            for (Cell d:sides) if (const Cell edge=cell+d;game.stage.in_bounds(edge) && reached[index(game.stage,edge)] && plain_ground(game.stage.at_or_border(edge))) {
                sites[1].push_back(cell);break;
            }
        }
    }
    decision.candidate_count=static_cast<int>(sites[0].size()+sites[1].size());
    if (!decision.candidate_count) {feature_failed(plan,"No vacant ground or accessible breakable wall outside reserved scenes");return;}
    int placed=0,attempted=0,empties=0;
    for (int buried=0;buried<2;++buried) {
        auto& candidates=sites[static_cast<std::size_t>(buried)];
        for (std::size_t i=candidates.size();i>1;--i) std::swap(candidates[i-1],candidates[random_u32(game)%i]);
        const WeightedComponent counts[]{{buried ? 2 : 4,"Few sockets",5},{buried ? 3 : 6,"Several sockets",3},
            {buried ? 4 : 8,"More sockets",biome_stage(game.run.floor)<3 ? 0U : 2U}};
        const auto count=roll_component(game,&plan.report,feature,-1,buried ? "Wall search" : "Ground search",game.run.spawn,counts);
        std::vector<Cell> sockets;
        const WeightedComponent contents[]{{0,"Empty",6},{1,"Native cache",buried ? 4U : 3U},{2,"Lost workshop tool",buried ? 0U : 1U}};
        for (Cell cell:candidates) {
            if (static_cast<int>(sockets.size())>=count.value) break;
            if (forbidden[index(game.stage,cell)]) continue;
            sockets.push_back(cell);exclude(cell,8);++attempted;
            const auto choice=roll_component(game,&plan.report,feature,count.record,"Find socket",cell,contents);
            const GenerationStep checkpoint{trace,game,plan,"Late find socket",feature,choice.record};
            if (!choice.value) {++empties;component_result(&plan.report,choice,"Empty socket");continue;}
            const auto supply=roll_native_supply(game,&plan.report,feature,choice.record,cell,
                choice.value==2 ? LootSource::Workshop : LootSource::Cache,
                choice.value==2 ? SupplyNeed::Any : SupplyNeed::Dependable);
            if (supply.value<0) {component_result(&plan.report,choice,"No eligible source-table item");continue;}
            const auto kind=static_cast<ItemKind>(supply.value);const auto item=supply_item(kind);
            auto* tally=report ? &report->supplies[static_cast<std::size_t>(kind)] : nullptr;
            if (tally) ++tally->attempted;
            if (buried) {
                auto& tile=*game.stage.at(cell);tile.contents=kind;tile.content_count=static_cast<std::uint8_t>(item.count);
            } else {
                auto* loose=get_entity(game,spawn_entity(game,EntityKind::GroundItem,cell));
                if (!loose) {
                    if (tally) ++tally->rejected;
                    component_result(&plan.report,choice,"Entity capacity exhausted");
                    component_result(&plan.report,supply,"Not placed: entity capacity exhausted");continue;
                }
                loose->ground_item=item;loose->sprite=item_sprite(item);
            }
            if (tally) ++tally->placed;
            ++placed;
            component_result(&plan.report,choice,buried ? "Stored in breakable wall" : "Loose find placed",std::array{cell});
            component_result(&plan.report,supply,buried ? "Released when its wall opens" : "Actual item placed",std::array{cell});
            decision.regions.push_back({cell,cell+Cell{1,1}});
        }
        component_result(&plan.report,count,static_cast<int>(sockets.size())==count.value ? "Socket budget resolved" :
            "Partial sockets: eligible ground or spacing exhausted",sockets);
    }
    decision.outcome=GenerationOutcome::Built;
    decision.variant=std::to_string(placed)+" finds / "+std::to_string(attempted)+" sockets";
    decision.reason=std::to_string(empties)+" empty rolls; native/shared master supplies only; existing scenes and finds excluded";
}
