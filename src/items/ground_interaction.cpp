#include "ground_interaction.hpp"
#include "../world/floating_items.hpp"
#include "../world/ground_items.hpp"

namespace {

int ground_slot(const Game& game, Cell cell) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind == EntityKind::GroundItem && entity.cell == cell &&
            entity.ground_item.kind != ItemKind::None && entity.ground_item.count > 0) return slot;
    }
    return -1;
}

} // namespace

bool item_can_drop(const Item& item) {
    return item.kind != ItemKind::None && item.kind != ItemKind::Fist && item.count > 0 && item.flight.slot < 0;
}

GroundAction ground_action(const Game& game, const Entity& player) {
    const int slot = ground_slot(game, player.cell);
    if (slot < 0) return item_can_drop(*player.inventory.held()) ? GroundAction::Drop : GroundAction::None;
    Inventory candidate = player.inventory;
    Item incoming = game.entities[static_cast<std::size_t>(slot)].ground_item;
    if (transfer_item(candidate, incoming) > 0) return GroundAction::Pickup;
    return item_can_drop(*player.inventory.held()) ? GroundAction::Swap : GroundAction::Blocked;
}

void drop_player_item(Game& game, Entity& player) {
    Item& held = *player.inventory.held();
    if (!item_can_drop(held)) return;
    const Handle dropped = spawn_entity(game, EntityKind::GroundItem,
        nearby_ground_item_cell(game, player.cell));
    if (Entity* ground = get_entity(game, dropped)) {
        ground->ground_item = held;
        ground->sprite = item_sprite(held);
        held = {};
        player.block_ticks = 0;
        emit_sound(game, SoundId::Drop, player.cell);
    }
}

bool pickup_or_drop(Game& game, Entity& player) {
    const int slot = ground_slot(game, player.cell);
    if (slot < 0) {
        if (!item_can_drop(*player.inventory.held())) return false;
        drop_player_item(game, player);
        return player.inventory.held()->kind == ItemKind::None;
    }
    Entity& ground = game.entities[static_cast<std::size_t>(slot)];
    if (transfer_item(player.inventory, ground.ground_item) > 0) {
        if (ground.ground_item.count == 0) {
            stop_item_float(game, ground);
            remove_entity(game, {slot, ground.generation});
        }
        emit_sound(game, SoundId::Confirm, player.cell);
        return true;
    }
    if (!item_can_drop(*player.inventory.held())) return false;
    // SWAP: Reuse the pickup's entity; reserve any oversized remainder before changing equipment.
    Inventory candidate = player.inventory;
    const Item outgoing = *candidate.held();
    *candidate.held() = {};
    Item remainder = ground.ground_item;
    if (transfer_item(candidate, remainder) == 0) return false;
    if (remainder.count > 0) {
        Entity* excess = get_entity(game, spawn_entity(game, EntityKind::GroundItem,
            nearby_ground_item_cell(game, player.cell)));
        if (excess == nullptr) return false;
        excess->ground_item = remainder;
        excess->sprite = item_sprite(remainder);
    }
    stop_item_float(game, ground);
    ground.ground_item = outgoing;
    ground.sprite = item_sprite(outgoing);
    player.inventory = candidate;
    player.block_ticks = 0;
    emit_sound(game, SoundId::Drop, player.cell);
    return true;
}
