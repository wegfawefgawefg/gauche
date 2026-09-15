#include "../traps/nail_board.hpp"
#include "sled.hpp"
#include "ground_interaction.hpp"
#include "../props/doorstop.hpp"
#include "machine_fittings.hpp"
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

Item fixture_item(const Game& game,Cell cell,bool allow_candle) {
    const Prop& prop = game.stage.at_or_border(cell).prop;
    if (allow_candle) {
        const Item candle = candle_item(prop);
        if (candle.kind != ItemKind::None) return candle;
    }
    const Item board=recoverable_nail_board(game,cell);
    if (board.kind!=ItemKind::None) return board;
    const Item wedge = recoverable_doorstop(prop);
    if (wedge.kind != ItemKind::None) return wedge;
    const Item spike = recoverable_spike(prop);
    if (spike.kind != ItemKind::None) return spike;
    const Item fitting=removable_machine_fitting(game,cell);
    return fitting.kind!=ItemKind::None ? fitting : recoverable_sled(game,cell);
}

int release_fixture(Game& game,Entity& player,Cell cell,ItemKind kind) {
    if (kind == ItemKind::NailBoard) return release_nail_board(game,cell);
    if (kind == ItemKind::Sled) return release_sled(game,cell);
    if (kind == ItemKind::GroundingSpike) return release_grounding_spike(game,cell,player.cell);
    if (kind == ItemKind::EmergencyDoorstop) return release_doorstop(game,cell,player.cell);
    if (kind==ItemKind::PressureValve || kind==ItemKind::NozzleElbow) return release_machine_fitting(game,cell,player.cell);
    if (kind != ItemKind::CandleStub) return -1;
    const int slot = release_candle(game,cell);
    if (slot >= 0) keeper_candle_stolen(game,cell,
        {static_cast<int>(&player-game.entities.data()),player.generation});
    return slot;
}

} // namespace

bool item_can_drop(const Item& item) {
    return item.kind != ItemKind::None && item.kind != ItemKind::Fist && item.count > 0 && item.flight.slot < 0;
}

Item pickup_item_at(const Game& game, Cell cell) {
    const int slot = ground_slot(game,cell);
    if (slot >= 0) return game.entities[static_cast<std::size_t>(slot)].ground_item;
    return fixture_item(game,cell,true);
}

Item reachable_pickup_item(const Game& game, const Entity& player) {
    const Item feet = pickup_item_at(game,player.cell);
    return feet.kind != ItemKind::None ? feet : fixture_item(game,player.cell+player.facing,false);
}

GroundAction ground_action(const Game& game, const Entity& player) {
    if (pickup_fixture(game,player)) return GroundAction::Interact;
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
        ground->facing = player.facing;
        ground->sprite = item_sprite(held);
        held = {};
        player.block_ticks = 0;
        emit_sound(game, SoundId::Drop, player.cell);
    }
}

bool pickup_or_drop(Game& game, Entity& player) {
    int slot = ground_slot(game, player.cell);
    if (slot < 0) {
        // PRIORITY: Match the HUD: loose item, fixture at feet, then reachable fixture ahead.
        const Item feet = fixture_item(game,player.cell,true);
        const Cell cell = feet.kind != ItemKind::None ? player.cell : player.cell+player.facing;
        const Item incoming = feet.kind != ItemKind::None ? feet : fixture_item(game,cell,false);
        if (incoming.kind != ItemKind::None) {
            if (ground_action(game,player) == GroundAction::Blocked) return false;
            slot = release_fixture(game,player,cell,incoming.kind);
            if (slot < 0) return false;
        }
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
    ground.facing = player.facing;
    ground.sprite = item_sprite(outgoing);
    player.inventory = candidate;
    player.block_ticks = 0;
    emit_sound(game, SoundId::Drop, player.cell);
    return true;
}
