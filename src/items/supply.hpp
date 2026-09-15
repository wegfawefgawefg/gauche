#pragma once
#include "../game.hpp"

enum class ItemOrigin { Shared, Forest, Ice, Industrial };
enum class LootSource { Reward, Shop, Weapon, Cache, Secret, Workshop, Salvage };
struct ItemSupply {
    ItemKind kind;
    ItemOrigin origin;
    int weight, stage, count;
    unsigned sources;
};

const ItemSupply& item_supply(ItemKind kind);
bool native_supply(ItemKind kind, Biome biome);
int supply_count(ItemKind kind);
Item supply_item(ItemKind kind);
ItemKind roll_item_supply(Game& game, LootSource source, bool allow_import = true,
                         ItemKind exclude = ItemKind::None);
