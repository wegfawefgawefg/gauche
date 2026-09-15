#include "ground_interaction.hpp"
#include "pressure.hpp"
#include "../props/circuits.hpp"
#include "../props/candle.hpp"
#include "../entities/candle_keeper.hpp"
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

Item pickup_item_at(const Game& game, Cell cell) {
    const int slot = ground_slot(game,cell);
    if (slot >= 0) return game.entities[static_cast<std::size_t>(slot)].ground_item;
    const Item candle = candle_item(game.stage.at_or_border(cell).prop);
    if (candle.kind != ItemKind::None) return candle;
    const Item spike = recoverable_spike(game.stage.at_or_border(cell).prop);
    return spike.kind != ItemKind::None ? spike : removable_valve(game,cell);
}

Item reachable_pickup_item(const Game& game, const Entity& player) {
    const Item feet = pickup_item_at(game,player.cell);
    if (feet.kind != ItemKind::None) return feet;
    const Item spike = recoverable_spike(game.stage.at_or_border(player.cell+player.facing).prop);
    return spike.kind != ItemKind::None ? spike : removable_valve(game,player.cell+player.facing);
}

GroundAction ground_action(const Game& game, const Entity& player) {
    Item incoming = reachable_pickup_item(game,player);
    if (incoming.kind == ItemKind::None) return item_can_drop(*player.inventory.held()) ? GroundAction::Drop : GroundAction::None;
    Inventory candidate = player.inventory;
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
    int slot = ground_slot(game, player.cell);
    if (slot < 0 && candle_item(game.stage.at_or_border(player.cell).prop).kind != ItemKind::None) {
        if (ground_action(game,player) == GroundAction::Blocked) return false;
        slot = release_candle(game,player.cell);
        if (slot < 0) return false;
        const int player_slot = static_cast<int>(&player - game.entities.data());
        keeper_candle_stolen(game,player.cell,{player_slot,player.generation});
    }
    if (slot < 0) {
        const Cell cell = recoverable_spike(game.stage.at_or_border(player.cell).prop).kind != ItemKind::None ?
            player.cell : player.cell+player.facing;
        if (recoverable_spike(game.stage.at_or_border(cell).prop).kind != ItemKind::None) {
            if (ground_action(game,player) == GroundAction::Blocked) return false;
            slot = release_grounding_spike(game,cell,player.cell);
            if (slot < 0) return false;
        }
    }
    if (slot < 0 && removable_valve(game,player.cell+player.facing).kind != ItemKind::None) {
        if (ground_action(game,player) == GroundAction::Blocked) return false;
        slot = release_valve(game,player.cell+player.facing,player.cell);
        if (slot < 0) return false;
    }
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
