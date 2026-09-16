#include "landmark_supplies.hpp"
#include "../items/supply.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <optional>

namespace {
constexpr WeightedComponent rewards[]{{0,"Native weapon",5},{1,"Portable equipment",3},{2,"Dependable cache",2}};
constexpr WeightedComponent pocket_types[]{{0,"Undisturbed ground",3},{1,"Forager leftovers",3},{2,"Bone scatter",2},{3,"Fungal patch",2}};
constexpr WeightedComponent supplies[]{{0,"No loose supply",5},{1,"Cache supply",3},{2,"Workshop tool",2}};
constexpr WeightedComponent savings[]{{0,"No coins",2},{1,"Scattered savings",3}};
constexpr WeightedComponent scatter[]{{0,"No prop",2},{1,"Local prop",5}};

bool usable(const Game& game,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || prop_blocks(tile->prop) || entity_at(game,cell,false)>=0) return false;
    if (tile->kind==TileKind::Lava || tile->kind==TileKind::Water || tile->kind==TileKind::Chasm) return false;
    return tile->prop.kind==PropKind::None || tile->prop.kind==PropKind::Leaves || tile->prop.kind==PropKind::Twigs ||
        tile->prop.kind==PropKind::Fern || tile->prop.kind==PropKind::TallGrass;
}
std::optional<Cell> site(Game& game,std::span<const Cell> ground,Cell anchor,int radius) {
    std::vector<Cell> candidates;
    for (Cell cell:ground) if (distance(cell,anchor)<=radius && usable(game,cell)) candidates.push_back(cell);
    if (candidates.empty()) return std::nullopt;
    return candidates[random_u32(game)%candidates.size()];
}
void item_at(Game& game,GenerationReport* report,ComponentRoll roll,std::span<const Cell> ground,
             Cell anchor,int radius,LootSource source,SupplyNeed need) {
    const auto cell=site(game,ground,anchor,radius);
    if (!cell) {component_result(report,roll,"No vacant safe ground in this component");return;}
    const auto kind=roll_item_supply(game,source,false,ItemKind::None,need);
    if (kind==ItemKind::None) {component_result(report,roll,"No eligible native item in source table");return;}
    auto* item=get_entity(game,spawn_entity(game,EntityKind::GroundItem,*cell));
    if (!item) {component_result(report,roll,"Entity capacity exhausted");return;}
    // Exact placement: an island prize must never migrate to the mainland.
    item->ground_item=supply_item(kind);item->sprite=item_sprite(kind);
    game.stage.at(*cell)->prop={};
    component_result(report,roll,item_name(kind),std::array{*cell});
}
void loose_coins(Game& game,GenerationReport* report,GenerationFeature feature,int parent,
                 std::span<const Cell> ground,Cell anchor) {
    const auto roll=roll_component(game,report,feature,parent,"Savings",anchor,savings);
    if (!roll.value) {component_result(report,roll,"Empty slot");return;}
    const auto cell=site(game,ground,anchor,3);
    if (!cell) {component_result(report,roll,"No vacant safe ground");return;}
    auto* coins=get_entity(game,spawn_entity(game,EntityKind::Coins,*cell));
    if (!coins) {component_result(report,roll,"Entity capacity exhausted");return;}
    coins->counter_a=18+static_cast<int>(random_u32(game)%28);
    component_result(report,roll,"Coin pile",std::array{*cell});
}
}

void compose_landmark_supplies(Game& game,GenerationReport* report,GenerationFeature feature,
    std::span<const Cell> ground,Cell cache,bool optional_reward) {
    const auto reward=roll_component(game,report,feature,-1,optional_reward ? "Optional island prize" : "Landmark reward",cache,rewards);
    const auto source=reward.value==0 ? LootSource::Weapon : optional_reward ? LootSource::Secret : LootSource::Cache;
    const auto need=reward.value==1 ? SupplyNeed::Equipment : SupplyNeed::Dependable;
    item_at(game,report,reward,ground,cache,optional_reward ? 0 : 4,source,need);
    if (optional_reward) return;
    loose_coins(game,report,feature,reward.record,ground,cache);
    std::vector<Cell> anchors;
    const int wanted=2+static_cast<int>(random_u32(game)%3);
    std::vector<Cell> candidates(ground.begin(),ground.end());
    for (std::size_t i=candidates.size();i>1;--i) std::swap(candidates[i-1],candidates[random_u32(game)%i]);
    for (Cell anchor:candidates) {
        if (static_cast<int>(anchors.size())>=wanted) break;
        if (!usable(game,anchor) || distance(anchor,cache)<6) continue;
        bool near=false;for (Cell old:anchors) if (distance(old,anchor)<7) near=true;
        if (near) continue;
        anchors.push_back(anchor);
        const auto pocket=roll_component(game,report,feature,-1,"Interior pocket",anchor,pocket_types);
        if (!pocket.value) {component_result(report,pocket,"Undisturbed ground");continue;}
        component_result(report,pocket,"Pocket anchored",std::array{anchor});
        const auto supply=roll_component(game,report,feature,pocket.record,"Loose supply",anchor,supplies);
        if (supply.value) item_at(game,report,supply,ground,anchor,2,
            supply.value==2 ? LootSource::Workshop : LootSource::Cache,SupplyNeed::Any);
        else component_result(report,supply,"Empty slot");
        const int pieces=2+static_cast<int>(random_u32(game)%4);
        for (int i=0;i<pieces;++i) {
            const auto decoration=roll_component(game,report,feature,pocket.record,"Pocket prop",anchor,scatter);
            if (!decoration.value) {component_result(report,decoration,"Empty slot");continue;}
            const auto cell=site(game,ground,anchor,3);
            if (!cell) {component_result(report,decoration,"No vacant safe ground");continue;}
            const auto kind=pocket.value==1 ? PropKind::Nest : pocket.value==2 ? PropKind::BonePile : PropKind::Puffball;
            game.stage.at(*cell)->prop={};
            if (place_prop(game.stage,*cell,kind,static_cast<std::uint8_t>(random_u32(game)%4)))
                component_result(report,decoration,"Prop placed",std::array{*cell});
            else component_result(report,decoration,"Prop placement rejected");
        }
    }
}
