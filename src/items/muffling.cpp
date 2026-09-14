#include "muffling.hpp"
#include "../item_attribute.hpp"

namespace {

constexpr RegionalItem felt{"Muffling Felt", "Wrap next unwrapped weapon in slot order. Six quiet uses; loud impacts/explosions. Two wraps.",
    Sprite::MufflingFelt, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 14, 1, false, 2, 0, 0, 0, 0, SoundId::MuffleWrap};

} // namespace

const RegionalItem* muffling_item(ItemKind kind) {
    return kind == ItemKind::MufflingFelt ? &felt : nullptr;
}

bool muffleable_item(const Item& item) {
    return item.kind != ItemKind::None && item.count > 0 && !item.consume_on_use &&
        (item_is_melee(item.kind) || item_is_gun(item.kind));
}

int muffling_target_slot(const Inventory& inventory) {
    // ORDER: Start after the held wrap, cycle once. Never overwrite remaining charges.
    for (int offset = 1; offset < quick_slots; ++offset) {
        const int slot = (inventory.selected + offset) % quick_slots;
        const Item& item = inventory.slots[static_cast<std::size_t>(slot)];
        if (muffleable_item(item) && item.muffled_uses == 0 && item.flight.slot < 0) return slot;
    }
    return -1;
}

bool apply_muffling(Game& game, int slot) {
    Inventory& inventory = game.entities[static_cast<std::size_t>(slot)].inventory;
    const int target = muffling_target_slot(inventory);
    if (target < 0) return false;
    inventory.slots[static_cast<std::size_t>(target)].muffled_uses = 6;
    return true;
}

void emit_weapon_sound(Game& game, const Item& item, SoundId sound, Cell cell) {
    emit_sound(game, sound, cell, true, item.muffled_uses > 0);
}

void finish_muffled_use(Game& game, Item& item, Cell cell) {
    // ONE ACTION: A shotgun fan spends one charge, not one charge per pellet or cue.
    if (item.muffled_uses == 0) return;
    if (--item.muffled_uses == 0) emit_sound(game, SoundId::MuffleUnwind, cell);
}
