#include "remedies.hpp"
#include "action.hpp"

#include <algorithm>

namespace {

// REMEDIES: Recovery, prevention and risky food, rather than interchangeable heals.
constexpr RegionalItem herbs{"Herb Bag", "Chew healing herbs. Recover 1 HP each third of a second. Cannot stack active regeneration.",
    Sprite::HerbBag, {0, 0, 0, 0, 45, PatternEffect::Heal, false, 0, 18},
    ItemAction::Food, 7, 1, false, 4, 0, 0, 0, 0, SoundId::HerbsChew};
constexpr RegionalItem splint{"Splint", "Remove stun; resist more stun for 3s. Can be used while stunned. Two uses.",
    Sprite::Splint, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 6, 1, false, 2, 0, 0, 0, 0, SoundId::SplintBind};
constexpr RegionalItem root{"Bitter Root", "Lose 3 HP, wake and resist sleep for 10s. Can be chewed while asleep.",
    Sprite::BitterRoot, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 5, 5, true, 0, 0, 0, 0, 0, SoundId::RootChew};
constexpr RegionalItem chili{"Chili", "Double step recovery for 4s, then burn for 6 damage over 1.5s. Water helps.",
    Sprite::Chili, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 7, 3, true, 0, 0, 0, 0, 0, SoundId::ChiliBite};
constexpr RegionalItem bread{"Fungal Bread", "Bread... probably.",
    Sprite::FungalBread, {0, 0, 0, 0, 60, PatternEffect::Heal, false, 0, 25},
    ItemAction::Food, 9, 4, true, 0, 0, 0, 0, 0, SoundId::BreadMunch};

} // namespace

const RegionalItem* forest_remedy(ItemKind kind) {
    switch (kind) {
    case ItemKind::HerbBag: return &herbs;
    case ItemKind::Splint: return &splint;
    case ItemKind::BitterRoot: return &root;
    case ItemKind::Chili: return &chili;
    case ItemKind::FungalBread: return &bread;
    default: return nullptr;
    }
}

bool use_remedy(Game& game, int slot) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item& item = *user.inventory.held();
    VitalEffects& effects = user.vitals;
    if (user.health <= 0) return false;
    switch (item.kind) {
    case ItemKind::HerbBag:
        if (user.health >= user.max_health || effects.healing_left > 0) return false;
        begin_recovery(user, RecoveryKind::Herbs, item_pattern(item).heal);
        break;
    case ItemKind::Splint:
        if (user.stun_ticks == 0 && effects.stun_guard > 0) return false;
        user.stun_ticks = 0;
        effects.stun_guard = 180;
        break;
    case ItemKind::BitterRoot:
        if (user.health <= 3 || (user.sleep_ticks == 0 && effects.sleep_guard > 0)) return false;
        user.health -= 3;
        user.sleep_ticks = 0;
        effects.sleep_guard = 600;
        break;
    case ItemKind::Chili:
        if (effects.haste > 0) return false;
        effects.haste = 240;
        break;
    case ItemKind::FungalBread:
        if (user.health >= user.max_health) return false;
        user.health = std::min(user.max_health, user.health + item_pattern(item).heal);
        apply_sleep(user, fungal_bread_sleep_ticks);
        break;
    default: return false;
    }
    return true;
}

// DISABLED INPUT: Only the two cures work; waking cannot also move or attack this tick.
void use_disabled_remedy(Game& game, int slot, const Input& input) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    cancel_item_action(user);
    if (input.select >= 0 && input.select < quick_slots) user.inventory.selected = input.select;
    if (!input.use || input.cancel_use) return;
    const ItemKind kind = user.inventory.held()->kind;
    if ((kind == ItemKind::BitterRoot && user.sleep_ticks > 0) ||
        (kind == ItemKind::Splint && user.stun_ticks > 0))
        use_held_item(game, slot, user.cell);
}
