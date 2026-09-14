#include "cold_remedies.hpp"

namespace {

constexpr RegionalItem wool{"Wool Wrap", "Clear chill; resist more for 8s. Fire burns the wrap away and leaves you burning. Cannot wrap while alight.",
    Sprite::WoolWrap, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 8, 3, true, 0, 0, 0, 0, 0, SoundId::WoolBind};
constexpr RegionalItem broth{"Hot Broth", "Clear chill and heal slowly. Taking damage ends this healing. Cannot stack active regeneration.",
    Sprite::HotBroth, {0, 0, 0, 0, 45, PatternEffect::Heal, false, 0, 12},
    ItemAction::Food, 9, 3, true, 0, 0, 0, 0, 0, SoundId::BrothSip};
constexpr RegionalItem poultice{"Ice Poultice", "Stop burning and heal slowly, but suffer 3s chill. Wool blocks that chill. Cannot stack active regeneration.",
    Sprite::IcePoultice, {0, 0, 0, 0, 45, PatternEffect::Heal, false, 0, 10},
    ItemAction::Food, 7, 4, true, 0, 0, 0, 0, 0, SoundId::PoulticePress};

} // namespace

const RegionalItem* cold_remedy_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::WoolWrap: return &wool;
    case ItemKind::HotBroth: return &broth;
    case ItemKind::IcePoultice: return &poultice;
    default: return nullptr;
    }
}

bool use_cold_remedy(Game& game, int slot) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item& item = *user.inventory.held();
    if (user.health <= 0) return false;
    if (item.kind == ItemKind::WoolWrap) {
        if (user.burn_ticks > 0 || user.scorch_ticks > 0 || user.vitals.chill_guard > 0) return false;
        user.freeze_ticks = 0;
        user.vitals.chill_guard = 480;
        return true;
    }
    // REMEDIES: Full-health use needs a symptom to cure; another active recovery is never replaced.
    if (item.kind == ItemKind::HotBroth) {
        if (user.health >= user.max_health && user.freeze_ticks == 0) return false;
        if (!begin_recovery(user, RecoveryKind::Broth, item_pattern(item).heal)) return false;
        user.freeze_ticks = 0;
        return true;
    }
    if (item.kind == ItemKind::IcePoultice) {
        if (user.health >= user.max_health && user.burn_ticks == 0 && user.scorch_ticks == 0) return false;
        if (!begin_recovery(user, RecoveryKind::Poultice, item_pattern(item).heal)) return false;
        user.burn_ticks = user.scorch_ticks = 0;
        apply_chill(user, 180);
        return true;
    }
    return false;
}
