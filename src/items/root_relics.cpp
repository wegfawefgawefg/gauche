#include "root_relics.hpp"
#include "../projectiles/projectile.hpp"

namespace {

constexpr RegionalItem acorn{"Thunder Acorn", "Flies, then arcs through up to four bodies. Water or wire takes the rest as a six-step circuit. Friends too; a planted spike grounds it.",
    Sprite::ThunderAcorn, {1, 5, 1, 24, 60, PatternEffect::Damage, true, 0, 0, false, false, false, true},
    ItemAction::Material, 30, 3, true, 0, 0, 0, 0, 0, SoundId::ThunderThrow};

constexpr RegionalItem drill{"Root Drill", "Dig-power-3 auger bores a fixed line through walls and bodies. Unbreakable terrain stops it.",
    Sprite::RootDrill, {1, 5, 0, 24, 90, PatternEffect::Damage, true, 0, 0, true},
    ItemAction::Material, 38, 1, false, 3, 0, 0, 0, 3, SoundId::DrillStart};
constexpr RegionalItem seed{"Swap Seed", "Traveling seed swaps your positions. Needs a movable target and two clear landings. Hazards apply.",
    Sprite::SwapSeed, {1, 5, 0, 0, 60, PatternEffect::Utility, true},
    ItemAction::Material, 32, 3, true, 0, 0, 0, 0, 0, SoundId::SwapCast};

} // namespace

const RegionalItem* forest_root_relic(ItemKind kind) {
    switch (kind) {
    case ItemKind::ThunderAcorn: return &acorn;
    case ItemKind::RootDrill: return &drill;
    case ItemKind::SwapSeed: return &seed;
    default: return nullptr;
    }
}

bool launch_root_relic(Game& game, int owner_slot, const Item& item, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (shot == nullptr) return false;
    const bool drill = item.kind == ItemKind::RootDrill;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(drill ? ProjectileKind::Drill : ProjectileKind::Swap);
    shot->sprite = drill ? Sprite::DrillRoot : Sprite::SwapSeed;
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->counter_b = pattern.damage;
    shot->entity_a = {owner_slot, owner.generation};
    shot->point_a = owner.cell;
    shot->facing = direction;
    shot->ground_item = item;
    shot->ground_item.count = 1;
    shot->timer_a = drill ? 360 : pattern.maximum * 4 + 4;
    shot->timer_b = drill ? 12 : 4;
    shot->light = drill ? LightEmitter{2, 150, {180, 211, 108}} : LightEmitter{2, 250, {211, 164, 242}};
    return true;
}
