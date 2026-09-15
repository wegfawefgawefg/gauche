#include "supply.hpp"

#include <algorithm>

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

ItemKind roll_item_supply(Game& game, LootSource source, bool allow_import, ItemKind exclude) {
    // IMPORTS: One explicit chance per eligible offer/cache, not a shared fallback
    // full of stronger guns. Workshops always supply their own available tools.
    const bool salvage=source==LootSource::Salvage;
    const bool imported=allow_import && source!=LootSource::Workshop && !salvage &&
        random_u32(game)%(source==LootSource::Secret ? 500U : 2000U)==0;
    const Biome biome=floor_biome(game.run.floor);
    const int stage=biome_stage(game.run.floor)+(source==LootSource::Secret || salvage ? 1 : 0);
    // SALVAGE: Dangerous optional caches draw from the master weapon membership,
    // with a one-stage preview and rarity weighting. Never a duplicate loot list.
    const unsigned mask=1U<<static_cast<unsigned>(salvage ? LootSource::Weapon : source);
    const auto weight=[&](ItemKind kind, bool foreign) {
        const auto& entry=item_supply(kind);
        if (kind==exclude || (entry.sources&mask)==0 || entry.weight<=0 ||
            native_supply(kind,biome)==foreign || (!foreign && entry.stage>stage)) return 0;
        return source==LootSource::Secret || salvage ? 12/entry.weight : entry.weight;
    };
    // FALLBACK: A source without eligible imports still produces native content.
    for (int attempt=0;attempt<(imported ? 2 : 1);++attempt) {
        const bool foreign=imported && attempt==0;
        unsigned total=0;
        for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i)
            total+=static_cast<unsigned>(weight(static_cast<ItemKind>(i),foreign));
        if (total==0) continue;
        unsigned roll=random_u32(game)%total;
        for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
            const auto kind=static_cast<ItemKind>(i);
            const auto amount=static_cast<unsigned>(weight(kind,foreign));
            if (roll<amount) return kind;
            roll-=amount;
        }
    }
    return ItemKind::None;
}
