#include "movement_tools.hpp"
#include "../projectiles/net.hpp"

#include <algorithm>

namespace {

constexpr RegionalItem net{"Throwing Net", "A traveling wide cast. Roots the first group for 2s; they can still attack. Walls catch the net.",
    Sprite::ThrowingNet, {1, 3, 0, 0, 60, PatternEffect::Utility, true, 1},
    ItemAction::Throw, 15, 3, true, 0, 0, 0, 0, 0, SoundId::NetThrow};
constexpr RegionalItem boots{"Sticky Boots", "Grip for 6s: resist shoves and slips, but each step takes longer. Crushers still crush. Four uses.",
    Sprite::StickyBoots, {0, 0, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 18, 1, false, 4, 0, 0, 0, 0, SoundId::BootsStick};
constexpr RegionalItem rabbit{"Rabbit Charm", "Retreat up to 3 cells opposite your aim. Stops at obstacles; hazards still apply. Three escapes.",
    Sprite::RabbitCharm, {0, 0, 0, 0, 90, PatternEffect::Utility},
    ItemAction::Material, 28, 1, false, 3, 0, 0, 0, 0, SoundId::RabbitEscape};

} // namespace

const RegionalItem* forest_movement_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::ThrowingNet: return &net;
    case ItemKind::StickyBoots: return &boots;
    case ItemKind::RabbitCharm: return &rabbit;
    default: return nullptr;
    }
}

bool use_movement_tool(Game& game, int slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item& item = *user.inventory.held();
    if (item.kind == ItemKind::ThrowingNet) return launch_net(game, slot, item, direction);
    if (item.kind == ItemKind::StickyBoots) {
        if (user.vitals.grip > 0) return false;
        user.vitals.grip = 360;
        return true;
    }
    if (item.kind != ItemKind::RabbitCharm || user.vitals.rooted > 0) return false;
    bool moved = false;
    for (int step = 0; step < 3; ++step) {
        const Cell before = user.cell;
        const Cell next = before - direction;
        if (!move_entity(game, slot, next)) break;
        moved = true;
        // SPRINGS/OIL: An extra displacement takes over; don't add more retreat steps.
        if (user.health <= 0 || user.cell != next || user.vitals.rooted > 0) break;
    }
    user.facing = direction;
    if (moved) user.move_wait = std::max(user.move_wait, user.move_interval);
    return moved;
}
