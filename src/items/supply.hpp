#pragma once
#include "../game.hpp"

enum class ItemOrigin { Shared, Forest, Ice, Industrial };
enum class LootSource { Reward, Shop, Weapon, Cache, Secret, Workshop, Salvage };
enum class ItemRole { None, Combat, Survival, Mobility, Utility, Oddity, Count };
enum class SupplyNeed { Any, Dependable, Equipment };
struct ItemSupply {
    ItemKind kind;
    ItemOrigin origin;
    int weight, stage, count;
    unsigned sources;
    ItemRole role;
};

const ItemSupply& item_supply(ItemKind kind);
bool native_supply(ItemKind kind, Biome biome);
int supply_count(ItemKind kind);
Item supply_item(ItemKind kind);
ItemKind roll_item_supply(Game& game, LootSource source, bool allow_import = true,
                         ItemKind exclude = ItemKind::None, SupplyNeed need = SupplyNeed::Any);

const char* item_role_name(ItemRole role);
bool dependable_supply(ItemKind kind);

// Shared eligibility/weights for actual selection and generation diagnostics.
int supply_weight(ItemKind kind,int floor,LootSource source,bool foreign=false,
                  ItemKind exclude=ItemKind::None,SupplyNeed need=SupplyNeed::Any);
unsigned supply_bucket(ItemKind kind);
std::array<unsigned,3> supply_shares(LootSource source);
