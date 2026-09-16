#include "supply.hpp"

#include <algorithm>
#include <array>

bool native_supply(ItemKind kind, Biome biome) {
    const auto origin=item_supply(kind).origin;
    return origin==ItemOrigin::Shared ||
        (origin==ItemOrigin::Forest && biome==Biome::Forest) ||
        (origin==ItemOrigin::Ice && biome==Biome::Ice) ||
        (origin==ItemOrigin::Industrial && biome==Biome::Industrial);
}

int supply_count(ItemKind kind) {
    return std::clamp(item_supply(kind).count,0,make_item(kind).max_count);
}

Item supply_item(ItemKind kind) { return make_item(kind,supply_count(kind)); }

int supply_weight(ItemKind kind,int floor,LootSource source,bool foreign,ItemKind exclude,SupplyNeed need) {
    if (source==LootSource::Reward && need==SupplyNeed::Any) need=SupplyNeed::Dependable;
    const bool salvage=source==LootSource::Salvage;
    const Biome biome=floor_biome(floor);
    const int stage=biome_stage(floor)+(source==LootSource::Secret || salvage ? 1 : 0);
    const unsigned mask=1U<<static_cast<unsigned>(salvage ? LootSource::Weapon : source);
    const auto& entry=item_supply(kind);
    if ((need==SupplyNeed::Dependable && !dependable_supply(kind)) ||
        (need==SupplyNeed::Equipment && entry.role!=ItemRole::Combat && entry.role!=ItemRole::Mobility)) return 0;
    if (kind==exclude || (entry.sources&mask)==0 || entry.weight<=0 ||
        native_supply(kind,biome)==foreign || (!foreign && entry.stage>stage)) return 0;
    return source==LootSource::Secret || salvage ? 12/entry.weight : entry.weight;
}

unsigned supply_bucket(ItemKind kind) {
    return dependable_supply(kind) ? 0U : item_supply(kind).role==ItemRole::Utility ? 1U : 2U;
}
std::array<unsigned,3> supply_shares(LootSource source) {
    return source==LootSource::Workshop ? std::array<unsigned,3>{40,55,5} :
        source==LootSource::Secret ? std::array<unsigned,3>{85,12,3} : std::array<unsigned,3>{70,25,5};
}

ItemKind roll_item_supply(Game& game, LootSource source, bool allow_import, ItemKind exclude, SupplyNeed need) {
    if (source==LootSource::Reward && need==SupplyNeed::Any) need=SupplyNeed::Dependable;
    // IMPORTS: One explicit chance per eligible offer/cache, not a shared fallback
    // full of stronger guns. Workshops always supply their own available tools.
    const bool salvage=source==LootSource::Salvage;
    const bool imported=allow_import && source!=LootSource::Workshop && !salvage &&
        random_u32(game)%(source==LootSource::Secret ? 500U : 2000U)==0;
    const auto weight=[&](ItemKind kind,bool foreign) {
        return supply_weight(kind,game.run.floor,source,foreign,exclude,need);
    };
    // FALLBACK: A source without eligible imports still produces native content.
    for (int attempt=0;attempt<(imported ? 2 : 1);++attempt) {
        const bool foreign=imported && attempt==0;
        // Choose usefulness before the item: adding twenty niche tools must not
        // drown out portable equipment. Scene-specific supplies remain explicit.
        std::array<unsigned,3> totals{};
        for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
            const auto kind=static_cast<ItemKind>(i);
            totals[supply_bucket(kind)]+=static_cast<unsigned>(weight(kind,foreign));
        }
        const auto shares=supply_shares(source);
        unsigned share_total=0,available=0,selected=0;
        for (unsigned i=0;i<totals.size();++i) if (totals[i]) {
            share_total+=shares[i];++available;selected=i;
        }
        if (!share_total) continue;
        if (available>1) {
            unsigned share=random_u32(game)%share_total;
            for (unsigned i=0;i<totals.size();++i) if (totals[i]) {
                if (share<shares[i]) {selected=i;break;}
                share-=shares[i];
            }
        }
        unsigned roll=random_u32(game)%totals[selected];
        for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
            const auto kind=static_cast<ItemKind>(i);
            const auto amount=supply_bucket(kind)==selected ? static_cast<unsigned>(weight(kind,foreign)) : 0U;
            if (roll<amount) return kind;
            roll-=amount;
        }
    }
    return ItemKind::None;
}

const char* item_role_name(ItemRole role) {
    constexpr const char* names[]{"none","combat","survival","mobility","utility","oddity"};
    const auto index=static_cast<unsigned>(role);
    return index<static_cast<unsigned>(ItemRole::Count) ? names[index] : "invalid";
}

bool dependable_supply(ItemKind kind) {
    const auto role=item_supply(kind).role;
    return role==ItemRole::Combat || role==ItemRole::Survival || role==ItemRole::Mobility;
}
