#include "forest_border.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "growth_carving.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

void place_forest_border(Game& game,FloorPlan& plan) {
    constexpr auto feature=GenerationFeature::ForestBorder;
    if(!roll_generation_feature(game,plan,feature))return;
    const WeightedComponent reaches[]{{30,"Sheltered snowy approach",3},{42,"Spreading frost tongues",5},{54,"Deep winter border",2}};
    const auto climate=roll_component(game,&plan.report,feature,-1,"Cold reach from exit",game.run.exit,reaches);
    const auto routes=generation_walking_routes(game,plan);
    const int width=game.stage.width;
    std::vector<int> distances(game.stage.tiles.size(),-1);
    std::vector<Cell> queue{game.run.exit};
    distances[static_cast<std::size_t>(game.run.exit.y*width+game.run.exit.x)]=0;
    // Follow actual connected ground. Walls shelter nearby rooms instead of a
    // rectangular blue stamp cutting through unrelated landmarks.
    for(std::size_t i=0;i<queue.size();++i) {
        const Cell cell=queue[i];const int dist=distances[static_cast<std::size_t>(cell.y*width+cell.x)];
        if(dist>=climate.value)continue;
        for(Cell d:{Cell{1,0},{-1,0},{0,1},{0,-1}}) {
            const Cell next=cell+d;const auto* tile=game.stage.at(next);
            if(!tile || !walkable(*tile))continue;
            auto& value=distances[static_cast<std::size_t>(next.y*width+next.x)];
            if(value<0){value=dist+1;queue.push_back(next);}
        }
    }
    const int salt=static_cast<int>(random_u32(game)%97);std::vector<Cell> changed;
    int snow=0,ice=0,piles=0;
    for(Cell cell:queue) {
        auto& tile=*game.stage.at(cell);const auto index=static_cast<std::size_t>(cell.y*width+cell.x);
        const int dist=distances[index];
        // Coarse patches, with a softer edge towards the warm part of the floor.
        const int patch=(cell.x/4*17+cell.y/3*31+salt)%11;
        if(distance(cell,game.run.spawn)<16 || dist+patch>climate.value || tile.material!=TileMaterial::Stone || tile.current || tile.freeze_ticks)continue;
        bool covered=false;for(const auto& roof:game.stage.roofs)if(roof_covers(roof,cell))covered=true;
        if(covered)continue;
        if(tile.kind==TileKind::Grass || tile.kind==TileKind::Empty || tile.kind==TileKind::Ruin) {
            if(patch==0 && dist>6)continue;
            tile.kind=TileKind::Snow;++snow;changed.push_back(cell);
            if(!plan.protected_cell(cell) && !routes[index] && tile.prop.kind==PropKind::None && entity_at(game,cell,false)<0 && random_u32(game)%12==0 &&
                place_prop(game.stage,cell,PropKind::SnowPile,static_cast<std::uint8_t>(random_u32(game)%3)))++piles;
        } else if(tile.kind==TileKind::ShallowWater && !routes[index] && !plan.protected_cell(cell) && tile.prop.kind==PropKind::None && entity_at(game,cell,false)<0) {
            tile.kind=TileKind::Ice;++ice;changed.push_back(cell);
        }
    }
    component_result(&plan.report,climate,"Connected cold approach; required walking lanes remain free of new slippery ground",changed);
    auto& decision=plan.report.features.back();decision.candidate_count=static_cast<int>(queue.size());
    decision.outcome=changed.empty() ? GenerationOutcome::Failed : GenerationOutcome::Built;
    decision.reason="Local Forest-to-Ice scenery; Forest population and loot tables stay native. Snow can be cleared and frozen pools thawed.";
    decision.variant=std::to_string(snow)+" snowy cells, "+std::to_string(ice)+" frozen pool cells, "+std::to_string(piles)+" snow piles";
    Cell low{width,game.stage.height},high{};for(Cell cell:changed){low.x=std::min(low.x,cell.x);low.y=std::min(low.y,cell.y);high.x=std::max(high.x,cell.x+1);high.y=std::max(high.y,cell.y+1);}if(!changed.empty())decision.regions.push_back({low,high});
}
