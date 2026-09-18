#include "bow.hpp"
#include "muffling.hpp"
#include "action.hpp"
#include "../item_pattern.hpp"
#include "../projectiles/projectile.hpp"

#include <algorithm>

// PLAYER SLOTS: counter_a = draw ticks, counter_b = wait for release after cancel,
// label_b = drawing inventory slot + 1; point_b = last held aim.
bool step_bow(Game& game, int slot, const Input& input) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& bow = *user.inventory.held();
    const bool held = bow.kind == ItemKind::Bow;
    if (!held || input.cancel_use || input.drop || input.interact ||
        (user.label_b != 0 && user.label_b != user.inventory.selected + 1)) {
        cancel_item_action(user);
        if (!input.use) user.counter_b = 0;
        return held;
    }
    if (user.counter_b != 0) {
        if (!input.use) user.counter_b = 0;
        return true;
    }
    if (input.use) {
        if (bow.cooldown > 0) return true;
        if (bow.loaded <= 0) {
            emit_sound(game, SoundId::WeaponEmpty, user.cell);
            bow.cooldown = 15;
            return true;
        }
        if (user.counter_a == 0) emit_weapon_sound(game, bow, SoundId::BowDraw, user.cell);
        user.counter_a = std::min(user.counter_a + user.action_steps, 12);
        user.label_b = user.inventory.selected + 1;
        user.point_b = user.facing;
        return true;
    }
    if (user.counter_a > 0 && bow.loaded > 0 &&
        launch_projectile(game, slot, bow, user.point_b, item_pattern(bow).maximum)) {
        finish_muffled_use(game, bow, user.cell);
        --bow.loaded;
        bow.cooldown = item_pattern(bow).cooldown;
        user.use_flash = 6;
    }
    user.counter_a = 0;
    user.label_b = 0;
    return true;
}
