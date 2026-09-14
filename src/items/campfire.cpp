#include "campfire.hpp"

namespace {

ItemKind cooked_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::RawMeat: return ItemKind::CookedMeat;
    case ItemKind::Egg: return ItemKind::FriedEgg;
    default: return ItemKind::None;
    }
}

// PORTION: Prepare a copy first; a full pack must not destroy the ingredient.
bool cook_portion(Game& game, Entity& player, Cell fire, int slot) {
    Inventory cooked = player.inventory;
    Item& ingredient = cooked.slots[static_cast<std::size_t>(slot)];
    const ItemKind result = cooked_item(ingredient.kind);
    if (result == ItemKind::None || ingredient.count <= 0) return false;
    const Item meal = make_item(result, 1, ingredient.attribute);
    if (--ingredient.count == 0) ingredient = {};
    if (!insert_item(cooked, meal)) return false;
    player.inventory = cooked;
    player.timer_b = 45;
    emit_sound(game, SoundId::CookingSizzle, fire);
    return true;
}

} // namespace

bool use_campfire(Game& game, Entity& player, const Entity& fire) {
    if (fire.fire_tramples >= 5) return false;
    if (player.timer_b > 0) return true;
    const int selected = player.inventory.selected;
    if (cooked_item(player.inventory.held()->kind) != ItemKind::None)
        return cook_portion(game, player, fire.cell, selected);
    for (int slot = 0; slot < quick_slots; ++slot)
        if (cook_portion(game, player, fire.cell, slot)) return true;
    return false;
}
