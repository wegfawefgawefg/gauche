#include "timber_grove.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "../props/interaction.hpp"
#include "../props/tall_tree.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::TimberGrove;
bool bare(const TimberGrove& grove,Cell cell,int radius) {
    return std::any_of(grove.firebreaks.begin(),grove.firebreaks.end(),[&](Cell old){return distance(old,cell)<=radius;});
}
}

void dress_timber_grove(Game& game,FloorPlan& plan,TimberGrove& grove,GenerationTrace* trace) {
    for(const auto& glade:grove.glades) {
        const WeightedComponent stands[]{{0,"Brush clearing",2},{3,"Sparse trees",3},{6,"Tree stand",5},{9,"Dense stand",3}};
        const auto trees=roll_component(game,&plan.report,feature,glade.component,"Tree cluster",glade.center,stands);
        const GenerationStep step{trace,game,plan,"Grove fuel bed",feature,trees.record};
        auto sites=glade.ground;
        for(std::size_t n=sites.size();n>1;--n)std::swap(sites[n-1],sites[random_u32(game)%n]);
        std::vector<Cell> planted;
        for(Cell cell:sites) {
            if(static_cast<int>(planted.size())>=trees.value)break;
            auto* tile=game.stage.at(cell);
            if(!tile || tile->kind!=TileKind::Grass || tile->prop.kind!=PropKind::None || bare(grove,cell,tree_reach))continue;
            if(std::any_of(grove.trees.begin(),grove.trees.end(),[&](Cell old){return distance(cell,old)<4;}))continue;
            if(place_prop(game.stage,cell,PropKind::TallTree,static_cast<std::uint8_t>(random_u32(game)%4))) {
                planted.push_back(cell);grove.trees.push_back(cell);
            }
        }
        component_result(&plan.report,trees,!trees.value ? "No trees in this clearing" : planted.size()==static_cast<std::size_t>(trees.value) ? "Tree stand placed" : "Reduced to preserve fall clearance and neighboring stands",planted);
        const WeightedComponent brush[]{{0,"Low leaf carpet",5},{1,"Tall grass bed",4},{2,"Fern bed",3},{3,"Fungal undergrowth",2}};
        const auto plants=roll_component(game,&plan.report,feature,glade.component,"Understory",glade.center,brush);
        const WeightedComponent density[]{{55,"Broken fuel",2},{80,"Close fuel",5},{95,"Continuous fuel",3}};
        const auto amount=roll_component(game,&plan.report,feature,plants.record,"Fuel coverage",glade.center,density);
        std::vector<Cell> covered;
        for(Cell cell:sites) {
            auto* tile=game.stage.at(cell);
            if(!tile || tile->kind!=TileKind::Grass || tile->prop.kind!=PropKind::None || plan.protected_cell(cell) || bare(grove,cell,0) || random_u32(game)%100>=static_cast<unsigned>(amount.value))continue;
            const bool low=random_u32(game)%3==0;
            const auto kind=low || !plants.value ? (random_u32(game)%4 ? PropKind::Leaves : PropKind::Twigs) :
                plants.value==1 ? PropKind::TallGrass : plants.value==2 ? PropKind::Fern : PropKind::Puffball;
            if(place_prop(game.stage,cell,kind,static_cast<std::uint8_t>(random_u32(game)%3)))covered.push_back(cell);
        }
        component_result(&plan.report,plants,"Patch fitted to remaining ground; mineral breaks stay bare",covered);
        component_result(&plan.report,amount,"Independent per-cell fuel roll",covered);
    }
    // Connective trails have low fuel too, without repainting the glade choices.
    for(Cell cell:grove.ground) {
        auto& tile=*game.stage.at(cell);
        if(tile.kind!=TileKind::Grass || tile.prop.kind!=PropKind::None || plan.protected_cell(cell) || random_u32(game)%3==0)continue;
        place_prop(game.stage,cell,PropKind::Leaves,static_cast<std::uint8_t>(random_u32(game)%3));
    }
}
