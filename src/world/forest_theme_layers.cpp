#include "forest_theme_layers.hpp"
#include "components.hpp"
#include "raster.hpp"
#include "terrain_material.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>

namespace {
constexpr auto feature=GenerationFeature::Themes;
bool room_allowed(const RoomPlan& room) {
    return room.role!=RoomRole::Entrance && room.role!=RoomRole::Exit && room.role!=RoomRole::Secret && room.role!=RoomRole::Shrine;
}
bool site_allowed(const Game& game,const FloorPlan& plan,Cell cell,GenerationTheme theme) {
    const bool soft=theme==GenerationTheme::Spiders || theme==GenerationTheme::Mushrooms || theme==GenerationTheme::Overgrowth;
    if (cell.x<=0 || cell.y<=0 || cell.x>=plan.width-1 || cell.y>=plan.height-1 ||
        (plan.protected_cell(cell) && !soft) || distance(cell,game.run.spawn)<8) return false;
    const auto& tile=game.stage.at_or_border(cell);
    if (tile.prop.kind!=PropKind::None || tile.contents!=ItemKind::None || wooden_terrain(tile)) return false;
    if (theme==GenerationTheme::WetWoods) for (const auto& roof:game.stage.roofs) if (roof_covers(roof,cell)) return false;
    // Keep objectives/lock envelopes intact. Nonblocking vegetation and silk
    // can overlap landmark reservations; terrain edits still respect their paths.
    for (const auto& room:plan.rooms) if (!room_allowed(room) &&
        std::abs(cell.x-room.center.x)<=room.half_width+1 && std::abs(cell.y-room.center.y)<=room.half_height+1) return false;
    return true;
}
bool exposed(const Game& game,Cell cell) {
    for (Cell side:{Cell{1,0},{0,1},{-1,0},{0,-1}}) if (walkable(game.stage.at_or_border(cell+side))) return true;
    return false;
}
PropKind theme_prop(Game& game,GenerationTheme theme) {
    const auto roll=random_u32(game)%6;
    switch(theme) {
    case GenerationTheme::WetWoods:return roll ? PropKind::Fern : PropKind::Puffball;
    case GenerationTheme::Timber:return roll<4 ? PropKind::Twigs : PropKind::Leaves;
    case GenerationTheme::Spiders:return roll ? PropKind::ForestWeb : PropKind::BonePile;
    case GenerationTheme::Mushrooms:return roll<4 ? PropKind::Puffball : roll==4 ? PropKind::LanternPlant : PropKind::Fern;
    case GenerationTheme::Ruins:return roll<4 ? PropKind::BonePile : PropKind::Leaves;
    case GenerationTheme::Ants:return roll ? PropKind::Twigs : PropKind::BonePile;
    case GenerationTheme::Overgrowth:return roll<3 ? PropKind::TallGrass : roll<5 ? PropKind::Fern : PropKind::Puffball;
    default:return PropKind::None;
    }
}
}
RoomRole themed_forest_role(Game& game,FloorPlan& plan,Cell center) {
    const bool wet=has_theme(plan.themes,GenerationTheme::WetWoods),wood=has_theme(plan.themes,GenerationTheme::Timber);
    const bool ruin=has_theme(plan.themes,GenerationTheme::Ruins),spider=has_theme(plan.themes,GenerationTheme::Spiders);
    const bool overgrowth=has_theme(plan.themes,GenerationTheme::Overgrowth);
    const WeightedComponent choices[]{
        {static_cast<int>(RoomRole::Clearing),"Clearing",2},
        {static_cast<int>(RoomRole::Thicket),"Thicket",wood || overgrowth ? 6U : 2U},
        {static_cast<int>(RoomRole::Brook),"Brook",wet ? 8U : 2U},
        {static_cast<int>(RoomRole::Ruins),"Ruins",ruin ? 8U : 2U},
        {static_cast<int>(RoomRole::Den),"Den",spider ? 5U : 2U},
        {static_cast<int>(RoomRole::Cache),"Cache",2},
        {static_cast<int>(RoomRole::Workshop),"Workshop",wood ? 4U : 2U},
        {static_cast<int>(RoomRole::Orchard),"Orchard",2},
    };
    const auto roll=roll_component(game,&plan.report,feature,plan.theme_component,"Base room role",center,choices);
    component_result(&plan.report,roll,"Base role selected; later objective, secret or landmark reservations take precedence",std::array{center});
    return static_cast<RoomRole>(roll.value);
}
void carve_forest_theme_patches(Game& game,FloorPlan& plan) {
    if (!forest_floor(game.run.floor)) return;
    std::vector<Cell> anchors;
    for (const auto& room:plan.rooms) if (room_allowed(room)) anchors.push_back(room.center);
    for (auto edge:plan.edges) {
        const auto& a=plan.rooms[static_cast<std::size_t>(edge.a)];const auto& b=plan.rooms[static_cast<std::size_t>(edge.b)];
        if (room_allowed(a) && room_allowed(b))
            anchors.push_back({(a.center.x+b.center.x)/2,(a.center.y+b.center.y)/2});
    }
    for (GenerationTheme theme:{plan.themes.major,plan.themes.minor}) {
        if (theme==GenerationTheme::None) continue;
        auto shuffled=anchors;
        for (std::size_t i=shuffled.size();i>1;--i) std::swap(shuffled[i-1],shuffled[random_u32(game)%i]);
        const int wanted=std::clamp(static_cast<int>(plan.rooms.size()/2),2,7);
        std::vector<bool> seen(game.stage.tiles.size(),false);
        const int parent=theme==plan.themes.major ? plan.theme_component : plan.minor_theme_component;
        for (std::size_t i=0;i<shuffled.size() && i<static_cast<std::size_t>(wanted);++i) {
            const Cell center=shuffled[i]+Cell{static_cast<int>(random_u32(game)%5)-2,static_cast<int>(random_u32(game)%5)-2};
            const WeightedComponent sizes[]{{5,"Small pocket",3},{9,"Broad patch",5},{14,"Spreading patch",biome_stage(game.run.floor)>=2 ? 3U : 1U}};
            const auto roll=roll_component(game,&plan.report,feature,parent,"Theme patch",center,sizes);
            const int rx=roll.value,ry=4+static_cast<int>(random_u32(game)%static_cast<unsigned>(rx));
            const std::array<Cell,6> polygon{{center+Cell{-rx,0},center+Cell{-rx/2,-ry},center+Cell{rx/2,-ry+2},
                center+Cell{rx,1},center+Cell{rx/2,ry},center+Cell{-rx/2,ry-1}}};
            const auto shape=raster_polygon(polygon,plan.width,plan.height);
            if (roll.record>=0) {auto& row=plan.report.components[static_cast<std::size_t>(roll.record)];row.guide.assign(polygon.begin(),polygon.end());row.guide_closed=true;}
            ThemePatch patch{theme,{},roll.record};std::vector<Cell> changed;
            for (Cell cell:shape.cells) {
                const auto index=static_cast<std::size_t>(cell.y*plan.width+cell.x);
                if (seen[index] || !site_allowed(game,plan,cell,theme)) continue;
                auto& tile=*game.stage.at(cell);
                if (tile.kind==TileKind::Wall) {
                    if (theme==GenerationTheme::Timber && tile.break_rule!=BreakRule::Unbreakable && !wooden_terrain(tile) && exposed(game,cell)) {
                        tile=wood_tile(random_u32(game)%3 ? TileMaterial::Tree : TileMaterial::Timber);changed.push_back(cell);seen[index]=true;
                    }
                    continue;
                }
                if (tile.kind!=TileKind::Grass && tile.kind!=TileKind::Ruin) continue;
                seen[index]=true;patch.ground.push_back(cell);
                if (theme==GenerationTheme::WetWoods) {tile={TileKind::ShallowWater};changed.push_back(cell);}
                if (theme==GenerationTheme::Ruins && tile.kind!=TileKind::Ruin) {tile={TileKind::Ruin};changed.push_back(cell);}
            }
            component_result(&plan.report,roll,patch.ground.empty() && changed.empty() ? "No suitable terrain; existing structures preserved" : "Terrain applied; scenery follows inhabitants",changed);
            plan.theme_patches.push_back(std::move(patch));
        }
    }
}
void dress_forest_theme_patches(Game& game,FloorPlan& plan) {
    if (!forest_floor(game.run.floor)) return;
    int props=0;
    for (const auto& patch:plan.theme_patches) {
        std::vector<Cell> placed;
        const unsigned chance=patch.theme==GenerationTheme::Spiders ? 38U : patch.theme==GenerationTheme::Overgrowth ? 55U : 32U;
        for (Cell cell:patch.ground) {
            auto* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->prop.kind!=PropKind::None || entity_at(game,cell,false)>=0 || random_u32(game)%100>=chance) continue;
            // Wet patches retain visible water; their dry margins get plants in
            // the river/ordinary scenery layers rather than underwater mushrooms.
            if (tile->kind==TileKind::ShallowWater) continue;
            if (place_prop(game.stage,cell,theme_prop(game,patch.theme),static_cast<std::uint8_t>(random_u32(game)%4))) placed.push_back(cell);
        }
        props+=static_cast<int>(placed.size());
        if (patch.component>=0) {
            auto& row=plan.report.components[static_cast<std::size_t>(patch.component)];
            const auto terrain=row.cells.size();row.cells.insert(row.cells.end(),placed.begin(),placed.end());
            row.placed=static_cast<int>(row.cells.size());
            row.result="Terrain changes "+std::to_string(terrain)+"; interactive ground props "+std::to_string(placed.size());
        }
    }
    for (auto& decision:plan.report.features) if (decision.feature==feature) {
        decision.outcome=GenerationOutcome::Built;
        decision.reason="Room/feature/encounter policy applied; "+std::to_string(plan.theme_patches.size())+" patch attempts, "+std::to_string(props)+" themed props. Existing structures, hazards and objectives preserved.";
    }
}
