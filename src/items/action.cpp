#include "action.hpp"
#include "catalog.hpp"
#include "muffling.hpp"
#include "../item_attribute.hpp"

#include <algorithm>

int item_windup(const Item& item) {
    int ticks = 0;
    switch (item.kind) {
    case ItemKind::SkateBlade: ticks = 4; break;
    case ItemKind::Chisel: ticks = 3; break;
    case ItemKind::DiggingClaws: ticks = 3; break;
    case ItemKind::Torch: ticks = 6; break;
    case ItemKind::Fist: ticks = 3; break;
    case ItemKind::Stick: ticks = 6; break;
    case ItemKind::Pickaxe: ticks = 10; break;
    case ItemKind::Hatchet: ticks = 8; break;
    case ItemKind::HuntingSpear: ticks = 5; break;
    case ItemKind::WoodenMaul: ticks = 14; break;
    case ItemKind::Rake: ticks = 6; break;
    case ItemKind::FlintKnife: ticks = 2; break;
    default: return 0;
    }
    if (item.attribute == ItemAttribute::Heavy) ticks = (ticks * 3 + 1) / 2;
    if (item.attribute == ItemAttribute::Agile) ticks = std::max(1, (ticks * 3 + 2) / 4);
    return ticks;
}

void cancel_item_action(Entity& user) {
    user.counter_a = 0;
    user.label_b = 0;
    user.counter_b = 1;
    user.ground_item = {};
}

bool step_melee_action(Game& game, int slot, const Input& input) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item& held = *user.inventory.held();
    const bool melee = item_is_melee(held.kind);
    // PLAYER SLOTS: negative label_b = committed melee slot, counter_a = windup left;
    // point_b = committed facing, ground_item = weapon at windup. Bow uses positive label_b.
    if (user.label_b < 0) {
        if (input.cancel_use || input.drop || input.interact || !melee ||
            -user.label_b != user.inventory.selected + 1 || held.kind != user.ground_item.kind ||
            held.attribute != user.ground_item.attribute) {
            cancel_item_action(user);
            return melee;
        }
        if (--user.counter_a > 0) return true;
        const Cell facing = user.point_b;
        user.label_b = 0;
        user.ground_item = {};
        use_held_item(game, slot, user.cell + facing);
        return true;
    }
    if (!melee) return false;
    if (user.label_b > 0 || input.cancel_use || input.drop || input.interact) {
        cancel_item_action(user);
        if (!input.use) user.counter_b = 0;
        return true;
    }
    if (!input.use) { user.counter_b = 0; return true; }
    if (user.counter_b != 0 || held.cooldown > 0) return true;
    // FIXTURE: Cooking and switches take priority over starting a swing.
    const Cell target = aimed_item_target(user, input.aim, item_pattern(held));
    if (interact_with_fixture(game, user.owner, target)) return true;
    user.counter_a = item_windup(held);
    user.label_b = -(user.inventory.selected + 1);
    user.point_b = user.facing;
    user.ground_item = held;
    const RegionalItem* spec = regional_item(held.kind);
    emit_weapon_sound(game, held, spec != nullptr ? spec->sound : held.kind == ItemKind::Fist ?
        SoundId::FistWindup : held.kind == ItemKind::Pickaxe ? SoundId::PickaxeWindup : SoundId::StickWindup, user.cell);
    return true;
}
