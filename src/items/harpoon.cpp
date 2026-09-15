#include "harpoon.hpp"
#include "action.hpp"
#include "../projectiles/harpoon.hpp"

namespace {
constexpr RegionalItem gun{"Harpoon Gun",
    "Hold use to reel a hit enemy. Release keeps line. Secondary cuts, then reloads. Cover or stowing snaps it.",
    Sprite::HarpoonGun, {1,10,0,28,36,PatternEffect::Damage,true},
    ItemAction::Gun,32,1,false,0,1,8,66,0,SoundId::HarpoonFire};
}

const RegionalItem* harpoon_item(ItemKind kind) {
    return kind == ItemKind::HarpoonGun ? &gun : nullptr;
}

void release_held_harpoon(Game& game, Entity& owner) {
    const Item& item = *owner.inventory.held();
    if (item.kind == ItemKind::HarpoonGun) release_harpoon(game,item.flight);
}

// INPUT: opened latches Secondary; the ordinary ammo fields remain magazine/reserve.
// A cut consumes the press even with an empty reserve. Holding it cannot reload/fire again.
bool step_harpoon_action(Game& game, int slot, const Input& input) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *user.inventory.held();
    if (item.kind != ItemKind::HarpoonGun) return false;
    if (user.label_b != 0) cancel_item_action(user);
    const bool pressed = input.reload && !item.opened;
    item.opened = input.reload;
    if (!input.use) user.counter_b = 0;
    if (input.cancel_use || input.drop || input.interact) {
        release_held_harpoon(game,user); cancel_item_action(user); return true;
    }
    if (pressed) {
        if (get_entity(game,item.flight)) release_held_harpoon(game,user);
        else reload_held_item(game,slot);
        user.counter_b = 1;
        return true;
    }
    if (input.reload) return true;
    if (Entity* shot = get_entity(game,item.flight)) {
        shot->fixture_open = input.use;
        return true;
    }
    item.flight = {};
    if (!input.use || user.counter_b != 0 || item.cooldown > 0) return true;
    if (!interact_with_fixture(game,user.owner,user.cell+user.facing))
        use_held_item(game,slot,user.cell+user.facing);
    return true;
}
