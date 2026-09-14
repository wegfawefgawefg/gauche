#include "fish.hpp"

namespace {

constexpr RegionalItem smoked_fish{"Smoked Fish", "Eat for 14 HP, or drop as bait. Eels follow fish through water. Feeding buys a pause, not a canceled shock.",
    Sprite::SmokedFish, {0,0,0,0,90,PatternEffect::Heal,false,0,14},
    ItemAction::Food,6,6,true,0,0,0,0,0,SoundId::FishEat};

constexpr RegionalItem salted_kelp{"Salted Kelp", "Cure nausea for 3 HP. Can kill if too weak. Drop as bait for seals and eels. No healing.",
    Sprite::SaltedKelp, {0,0,0,0,60,PatternEffect::Utility},
    ItemAction::Food,4,5,true,0,0,0,0,0,SoundId::KelpChew};

} // namespace

const RegionalItem* fish_item(ItemKind kind) {
    if (kind == ItemKind::SmokedFish) return &smoked_fish;
    return kind == ItemKind::SaltedKelp ? &salted_kelp : nullptr;
}

// MEAL: Consume first; lethal salt damage must not drop a second copy of the meal.
void apply_kelp_meal(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    actor.vitals.nausea = actor.vitals.nausea_wait = 0;
    emit_sound(game, SoundId::KelpChew, actor.cell);
    damage_entity(game, slot, kelp_health_cost, actor.cell, false);
}

bool eat_held_kelp(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *actor.inventory.held();
    if (actor.health <= 0 || actor.vitals.nausea == 0 ||
        item.kind != ItemKind::SaltedKelp || item.count <= 0 || item.cooldown > 0) return false;
    item.cooldown = item_pattern(item).cooldown;
    if (--item.count == 0) item = {};
    apply_kelp_meal(game, slot);
    return true;
}
