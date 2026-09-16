#include "supply_rolls.hpp"

ComponentRoll roll_native_supply(Game& game,GenerationReport* report,GenerationFeature feature,
    int parent,Cell cell,LootSource source,SupplyNeed need) {
    std::array<std::vector<WeightedComponent>,3> candidates;
    for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
        const auto kind=static_cast<ItemKind>(i);
        const int weight=supply_weight(kind,game.run.floor,source,false,ItemKind::None,need);
        if (weight>0) candidates[supply_bucket(kind)].push_back({static_cast<int>(kind),item_name(kind),static_cast<unsigned>(weight)});
    }
    const auto shares=supply_shares(source);
    const WeightedComponent buckets[]{{0,"Combat / survival / mobility",candidates[0].empty() ? 0U : shares[0]},
        {1,"Contextual utility",candidates[1].empty() ? 0U : shares[1]},
        {2,"Oddity",candidates[2].empty() ? 0U : shares[2]}};
    const auto bucket=roll_component(game,report,feature,parent,"Supply role",cell,buckets);
    if (bucket.value<0) return {};
    component_result(report,bucket,"Native/shared source table; stage and practical-use filters applied");
    const auto item=roll_component(game,report,feature,bucket.record,"Supply item",cell,candidates[static_cast<std::size_t>(bucket.value)]);
    component_result(report,item,"Selected; placement pending");
    return item;
}
