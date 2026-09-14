#include "quarry_tools.hpp"
#include "action.hpp"
#include "../props/ice_cover.hpp"
#include "../projectiles/ice_brick.hpp"

#include <algorithm>

// PLAYER SLOTS: positive label_b is the preparing slot; counter_a counts the hold.
// ground_item identifies the prepared kind/attribute; point_b follows the latest cardinal aim.
bool step_ice_brick(Game& game, int slot, const Input& input) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *user.inventory.held();
    const bool held = item.kind == ItemKind::IceBrick;
    if (!held) {
        if (user.label_b > 0 && user.ground_item.kind == ItemKind::IceBrick) cancel_item_action(user);
        return false;
    }
    if (input.cancel_use || input.drop || input.interact || input.reload ||
        (user.label_b != 0 && (user.label_b != user.inventory.selected + 1 ||
         user.ground_item.kind != item.kind || user.ground_item.attribute != item.attribute))) {
        cancel_item_action(user);
        if (!input.use) user.counter_b = 0;
        return true;
    }
    if (user.counter_b != 0) {
        if (!input.use) user.counter_b = 0;
        return true;
    }
    if (input.use) {
        if (item.cooldown > 0 || item.count <= 0) return true;
        const bool was_ready = user.counter_a >= brick_throw_hold_ticks;
        user.counter_a = std::min(user.counter_a + 1, brick_throw_hold_ticks);
        user.label_b = user.inventory.selected + 1;
        user.ground_item = item;
        user.point_b = user.facing;
        // READY: One cue at the threshold, never repeated while the trigger remains held.
        if (!was_ready && user.counter_a == brick_throw_hold_ticks)
            emit_sound(game, SoundId::IceBrickReady, user.cell);
        return true;
    }
    if (user.counter_a > 0) {
        const bool throwing = user.counter_a >= brick_throw_hold_ticks;
        const bool used = throwing ? launch_ice_brick(game, slot, item, user.point_b) :
            place_ice_cover(game, user.cell + user.point_b);
        if (used) {
            item.cooldown = item_pattern(item).cooldown;
            user.use_flash = 8;
            if (--item.count <= 0) item = {};
        } else emit_sound(game, SoundId::BumpStone, user.cell);
    }
    user.counter_a = user.label_b = 0;
    user.ground_item = {};
    return true;
}
