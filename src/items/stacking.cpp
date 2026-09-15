#include "../game.hpp"
#include "../props/candle.hpp"
#include "../item_attribute.hpp"

#include <algorithm>

bool item_stackable(const Item& item) {
    if (item.kind == ItemKind::GroundingSpike)
        return item.durability == item.max_durability && item.max_count > 1;
    if (item.kind == ItemKind::CandleStub)
        return !item.opened && item.loaded == candle_fuel_ticks && item.durability == item.max_durability && item.max_count > 1;
    return item.max_count > 1 && item.max_uses == 0 && item.max_durability == 0 && !item_is_gun(item.kind);
}

namespace {

bool compatible(const Item& a, const Item& b) {
    return item_stackable(a) && item_stackable(b) && a.kind == b.kind &&
        a.flight.slot < 0 && b.flight.slot < 0 && a.flame_ticks == b.flame_ticks && a.muffled_uses == b.muffled_uses && a.attribute == b.attribute && a.opened == b.opened && a.dig_power == b.dig_power &&
        a.max_count == b.max_count && a.consume_on_use == b.consume_on_use &&
        a.light.radius == b.light.radius && a.light.strength == b.light.strength &&
        a.light.color.red == b.light.color.red && a.light.color.green == b.light.color.green &&
        a.light.color.blue == b.light.color.blue && a.light.shape == b.light.shape;
}

} // namespace

int transfer_item(Inventory& inventory, Item& item) {
    if (item.kind == ItemKind::None || item.count <= 0 || item.max_count <= 0) return 0;
    const int original = item.count;
    // MERGE: Matching partial stacks get first refusal; never recharge used tools.
    if (item_stackable(item)) {
        for (Item& slot : inventory.slots) {
            if (!compatible(slot, item) || slot.count >= slot.max_count) continue;
            const int count = std::min(slot.max_count - slot.count, item.count);
            slot.count += count;
            slot.cooldown = std::max(slot.cooldown, item.cooldown);
            item.count -= count;
            if (item.count == 0) return original;
        }
    }
    for (Item& slot : inventory.slots) {
        if (slot.kind != ItemKind::None) continue;
        slot = item;
        slot.count = std::min(item.count, item_stackable(item) ? item.max_count : 1);
        item.count -= slot.count;
        if (item.count == 0) return original;
    }
    return original - item.count;
}

bool insert_item(Inventory& inventory, Item item) {
    const Inventory original = inventory;
    // TRANSACTION: Shops/rewards/crafting grant the entire item or leave everything alone.
    if (transfer_item(inventory, item) > 0 && item.count == 0) return true;
    inventory = original;
    return false;
}
