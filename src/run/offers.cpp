#include "../artifacts/powers.hpp"
#include "offers.hpp"
#include "../items/supply.hpp"
#include "../items/ground_interaction.hpp"
#include "../items/action.hpp"
#include "../world/ground_items.hpp"

Reward current_offer(const Game& game, int owner, int choice) {
    if (owner<0 || !has_player(game, owner) || choice<0 || choice>=3) return {};
    const PlayerId row=owner; const auto column=static_cast<std::size_t>(choice);
    if (game.run.phase==RunPhase::Shop) {
        const ItemKind kind=game.run.shop_stock[column];
        return {RewardKind::Item,kind,ArtifactKind::None,supply_count(kind)};
    }
    if (game.run.phase==RunPhase::Playing && player_state(game, row).pending_count>0)
        return player_state(game, row).pending_offers[0][column];
    return player_state(game, row).offers[column];
}

std::uint64_t offer_token(const Game& game, int owner, int choice) {
    if (owner<0 || !has_player(game, owner) || choice<0 || choice>=3) return 0;
    const PlayerId row = owner;
    const Entity* player=get_entity(game,player_state(game, row).controlled);
    if (!player || player->health<=0 || !player_state(game, row).online) return 0;
    if (game.run.phase==RunPhase::Reward && player_state(game, row).chosen) return 0;
    if (game.run.phase==RunPhase::Shop && (player_state(game, row).shop_ready ||
        game.run.shop_stock[static_cast<std::size_t>(choice)]==ItemKind::None)) return 0;
    if (game.run.phase!=RunPhase::Reward && game.run.phase!=RunPhase::Shop &&
        !(game.run.phase==RunPhase::Playing && player_state(game, row).pending_count>0)) return 0;
    const Reward reward=current_offer(game,owner,choice);
    std::uint64_t hash=1469598103934665603ULL;
    const auto mix=[&](std::uint64_t value) { hash=(hash^value)*1099511628211ULL; };
    mix(game.run.seed); mix(static_cast<unsigned>(game.run.floor));
    mix(static_cast<unsigned>(game.run.phase)); mix(static_cast<unsigned>(owner));
    mix(player->generation); mix(static_cast<unsigned>(player_state(game, row).pending_count));
    mix(static_cast<unsigned>(choice)); mix(static_cast<unsigned>(reward.kind));
    mix(static_cast<unsigned>(reward.item)); mix(static_cast<unsigned>(reward.artifact));
    mix(static_cast<unsigned>(reward.amount)); mix(static_cast<unsigned>(reward.attribute));
    return hash==0 ? 1 : hash;
}

void apply_offer_choice(Game& game, int owner, const Input& input) {
    const auto token=offer_token(game,owner,input.select);
    if (token==0 || (input.offer_token!=0 && token!=input.offer_token) ||
        (input.replace_slot>=0 && input.offer_token==0)) return;
    if (game.run.phase==RunPhase::Shop)
        buy_shop_item(game,owner,input.select,input.replace_slot,input.replace_kind);
    else if (game.run.phase==RunPhase::Reward)
        choose_reward(game,owner,input.select,input.replace_slot,input.replace_kind);
    else choose_pending_reward(game,owner,input.select,input.replace_slot,input.replace_kind);
}

bool accept_offer_item(Game& game, Entity& player, Item incoming,
                       int replace_slot, ItemKind expected) {
    improve_pickup(player,incoming);
    if (replace_slot<0) return insert_item(player.inventory,incoming);
    if (replace_slot>=quick_slots || incoming.kind==ItemKind::None || incoming.count<=0 ||
        incoming.count>(item_stackable(incoming) ? incoming.max_count : 1)) return false;
    const auto slot=static_cast<std::size_t>(replace_slot);
    const Item outgoing=player.inventory.slots[slot];
    if (outgoing.kind!=expected || !item_can_drop(outgoing)) return false;
    // RESERVE FIRST: Full entity storage must not eat an item or charge a buyer.
    Entity* ground=get_entity(game,spawn_entity(game,EntityKind::GroundItem,
        nearby_ground_item_cell(game,player.cell)));
    if (!ground) return false;
    ground->ground_item=outgoing;
    ground->sprite=item_sprite(outgoing);
    ground->facing=player.facing;
    player.inventory.slots[slot]=incoming;
    cancel_item_action(player);
    player.block_ticks=0;
    emit_sound(game,SoundId::Drop,player.cell);
    return true;
}
