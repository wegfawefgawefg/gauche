#include "giant_tree.hpp"
#include "landmark_supplies.hpp"
#include "tree_hollow.hpp"
#include "components.hpp"
#include "../entities/forest_spider.hpp"
#include "../entities/attacks.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::GiantTree;
void shuffle(Game& game,std::vector<Cell>& cells) {
    for(std::size_t i=cells.size();i>1;--i)std::swap(cells[i-1],cells[random_u32(game)%i]);
}
bool vacant(const Game& game,Cell cell) {
    const auto* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->prop.kind==PropKind::None && entity_at(game,cell,false)<0;
}
bool mouth(const GiantTree& tree,Cell cell,int radius) {
    return std::any_of(tree.entrances.begin(),tree.entrances.end(),[&](Cell e){return distance(e,cell)<=radius;});
}
void group(Game& game,const GiantTree& tree,Cell anchor,std::span<const Cell> ground,
    GenerationReport* report,std::vector<Cell>& occupied) {
    const int stage=biome_stage(game.run.floor);
    const WeightedComponent families[]{{0,"Winged hollow",4},{1,"Restless dead",3},
        {2,"Hunting pack",stage>=2 ? 3U : 0U},{3,"Spider brood",tree.spiders ? 9U : stage>=2 ? 3U : 1U},
        {4,"Guarded root garden",stage>=2 ? 3U : 0U}};
    const auto family=roll_component(game,report,feature,tree.hollow_component,"Hollow combat pocket",anchor,families);
    const WeightedComponent sizes[]{{3,"Three fighters",4},{4,"Four fighters",4},{5,"Five fighters",stage>=2 ? 3U : 1U}};
    const auto size=roll_component(game,report,feature,family.record,"Pocket population",anchor,sizes);
    std::vector<Cell> sites;for(Cell cell:ground)if(distance(cell,anchor)<=5 && distance(cell,tree.cache)>2 && !mouth(tree,cell,3) && clear_attack_sight(game,anchor,cell))sites.push_back(cell);
    std::vector<Cell> placed;Handle ward{};
    for(int member=0;member<size.value;++member) {
        std::vector<WeightedComponent> species;
        const auto add=[&](EntityKind kind,const char* name,unsigned weight){species.push_back({static_cast<int>(kind),name,weight});};
        if(family.value==0){add(EntityKind::Bat,"Bat",5);add(EntityKind::Mosquito,"Mosquito",tree.hollow==WetHollow ? 5U : 2U);}
        else if(family.value==1){add(EntityKind::Zombie,"Zombie",7);add(EntityKind::Bat,"Bat",2);}
        else if(family.value==2){add(EntityKind::Wolf,"Wolf",6);add(EntityKind::Boar,"Boar",2);}
        else if(family.value==3){add(EntityKind::ForestSpider,"Forest spider",8);add(EntityKind::Bat,"Bat",1);}
        else {add(EntityKind::RootTurret,"Root turret",ward.slot<0 ? 1U : 0U);add(EntityKind::BrambleGuard,"Linked bramble guard",ward.slot>=0 ? 1U : 0U);}
        const auto choice=roll_component(game,report,feature,size.record,"Hollow fighter",anchor,species);
        auto cell=std::find_if(sites.begin(),sites.end(),[&](Cell c){return vacant(game,c) && std::none_of(occupied.begin(),occupied.end(),[&](Cell old){return distance(old,c)<2;});});
        if(cell==sites.end()){component_result(report,choice,"No spaced ground left in this pocket");continue;}
        const auto kind=static_cast<EntityKind>(choice.value);const auto handle=spawn_entity(game,kind,*cell);auto* actor=get_entity(game,handle);
        if(!actor){component_result(report,choice,"Entity capacity exhausted");continue;}
        if(kind==EntityKind::RootTurret)ward=handle;
        if(kind==EntityKind::BrambleGuard)actor->entity_a=ward;
        if(kind==EntityKind::ForestSpider)set_forest_spider_role(*actor,member==0 && stage>=3 && random_u32(game)%5==0 ? SpiderMother : random_u32(game)%3==0 ? SpiderYoung : SpiderAdult);
        placed.push_back(*cell);occupied.push_back(*cell);component_result(report,choice,"Fighter placed",std::array{*cell});
    }
    component_result(report,size,placed.size()==static_cast<std::size_t>(size.value) ? "Requested fighters placed" : "Reduced by actual pocket space/capacity",placed);
    component_result(report,family,"Combat population; quiet inhabitants roll separately",placed);
    const WeightedComponent visitors[]{{0,"No quiet visitor",6},{1,"Forager",2},{2,"Lantern moth",2}};
    const auto visitor=roll_component(game,report,feature,family.record,"Quiet inhabitant",anchor,visitors);
    auto free=std::find_if(sites.begin(),sites.end(),[&](Cell c){return vacant(game,c);});
    if(!visitor.value)component_result(report,visitor,"Empty slot");
    else if(free==sites.end())component_result(report,visitor,"No vacant ground");
    else if(get_entity(game,spawn_entity(game,visitor.value==1 ? EntityKind::ForagerGoblin : EntityKind::LanternMoth,*free)))
        component_result(report,visitor,"Additional inhabitant",std::array{*free});
    else component_result(report,visitor,"Entity capacity exhausted");
    // Real breakable lights replace the same three invisible lights in every tree.
    const WeightedComponent lamps[]{{0,"Dark pocket",2},{1,"Lantern growth",5}};
    const auto lamp=roll_component(game,report,feature,family.record,"Pocket light",anchor,lamps);
    free=std::find_if(sites.begin(),sites.end(),[&](Cell c){return vacant(game,c);});
    if(lamp.value && free!=sites.end() && place_prop(game.stage,*free,PropKind::LanternPlant))component_result(report,lamp,"Living light; breaking it darkens this pocket",std::array{*free});
    else component_result(report,lamp,lamp.value ? "No vacant light site" : "No added light");
}
}

void populate_giant_tree(Game& game,const FloorPlan& plan,GenerationReport* report) {
    for(const auto& tree:plan.giant_trees) {
        auto ground=tree.ground;shuffle(game,ground);std::vector<Cell> occupied,anchors;
        const WeightedComponent counts[]{{4,"Four combat pockets",3},{5,"Five combat pockets",5},{6,"Six combat pockets",3}};
        const Cell center=tree.canopy.start+Cell{tree.canopy.length/2,tree.canopy.width/2};
        const auto count=roll_component(game,report,feature,tree.hollow_component,"Hollow population spread",center,counts);
        for(Cell cell:ground) {
            if(static_cast<int>(anchors.size())>=count.value)break;
            if(!vacant(game,cell) || distance(cell,tree.cache)<4 || mouth(tree,cell,4))continue;
            bool near=false;for(Cell old:anchors)if(distance(old,cell)<8)near=true;
            if(near)continue;
            int space=0;for(Cell other:ground)if(distance(cell,other)<=4 && vacant(game,other) && clear_attack_sight(game,cell,other))++space;
            if(space<12)continue;
            anchors.push_back(cell);group(game,tree,cell,ground,report,occupied);
        }
        component_result(report,count,"Independent combat pockets on surviving ground",anchors);
        // Vegetation follows its own pocket rolls, not a fixed rim or furnishing kit.
        const WeightedComponent flora[]{{0,"Leaf litter",4},{1,"Fungal bed",3},{2,"Webbed remains",tree.spiders ? 5U : 1U}};
        for(int n=0;n<8 && !ground.empty();++n) {
            const Cell anchor=ground[random_u32(game)%ground.size()];
            const auto patch=roll_component(game,report,feature,tree.hollow_component,"Hollow undergrowth",anchor,flora);
            std::vector<Cell> placed;
            for(Cell cell:ground) {
                if(distance(cell,anchor)>3 || !vacant(game,cell) || mouth(tree,cell,2) || distance(cell,tree.cache)<3 || random_u32(game)%3==0)continue;
                const auto prop=patch.value==2 ? (random_u32(game)%3 ? PropKind::ForestWeb : PropKind::BonePile) :
                    patch.value==1 ? PropKind::Puffball : random_u32(game)%2 ? PropKind::Fern : PropKind::Leaves;
                if(place_prop(game.stage,cell,prop,static_cast<std::uint8_t>(random_u32(game)%3)))placed.push_back(cell);
            }
            component_result(report,patch,"Undergrowth fitted around inhabitants and approaches",placed);
        }
        compose_landmark_supplies(game,report,feature,tree.ground,tree.cache);
    }
}
