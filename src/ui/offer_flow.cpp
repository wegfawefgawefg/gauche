#include "interaction.hpp"
#include "../run/offers.hpp"
#include "../items/ground_interaction.hpp"

void cancel_offer_flow(InteractionUi& ui) {
    ui.offer_mode=OfferMode::Browse;
    ui.replace_slot=-1;
    ui.replace_kind=ItemKind::None;
    ui.active_offer_token=0;
    ui.inventory_open=false;
    ui.notice.clear();
}

void begin_offer_flow(InteractionUi& ui, const Game& game, int owner, int choice) {
    const auto token=offer_token(game,owner,choice);
    if (token==0) { ui.notice="That offer is no longer available."; return; }
    const auto row=static_cast<std::size_t>(owner);
    const Entity* player=get_entity(game,game.players[row]);
    const Reward offer=current_offer(game,owner,choice);
    if (game.run.phase==RunPhase::Shop && game.run.coins[row]<shop_price(offer.item)) {
        ui.notice="NOT ENOUGH GOLD"; return;
    }
    ui.offer_focus=choice;
    ui.active_offer_token=token;
    ui.replace_slot=-1;
    ui.replace_kind=ItemKind::None;
    Inventory trial=player->inventory;
    const bool fits=offer.kind!=RewardKind::Item || insert_item(trial,reward_item(offer));
    ui.offer_mode=fits ? OfferMode::Confirm : OfferMode::Replace;
    ui.inventory_open=!fits;
    ui.slot_focus=player->inventory.selected;
    ui.compare_ground=false;
    ui.notice=fits ? "" : "NO ROOM - choose the item to replace.";
}

void update_offer_flow(InteractionUi& ui, const Game& game, int owner,
                       bool confirm, bool cancel, Input& input) {
    if (ui.offer_mode==OfferMode::Browse) return;
    const auto current=offer_token(game,owner,ui.offer_focus);
    if (current==0 || current!=ui.active_offer_token) {
        cancel_offer_flow(ui);
        ui.notice="Offer changed. Choose again.";
        return;
    }
    if (cancel) {
        if (ui.offer_mode==OfferMode::Confirm && ui.replace_slot>=0) {
            ui.offer_mode=OfferMode::Replace;
            ui.inventory_open=true;
            ui.notice="NO ROOM - choose the item to replace.";
        } else cancel_offer_flow(ui);
        return;
    }
    if (!confirm) return;
    const Entity* player=get_entity(game,game.players[static_cast<std::size_t>(owner)]);
    if (ui.offer_mode==OfferMode::Replace) {
        const Item& outgoing=player->inventory.slots[static_cast<std::size_t>(ui.slot_focus)];
        if (!item_can_drop(outgoing)) { ui.notice="This item cannot be replaced."; return; }
        ui.replace_slot=ui.slot_focus;
        ui.replace_kind=outgoing.kind;
        ui.offer_mode=OfferMode::Confirm;
        ui.notice.clear();
        return;
    }
    input.select=ui.offer_focus;
    input.replace_slot=ui.replace_slot;
    input.replace_kind=ui.replace_kind;
    input.offer_token=ui.active_offer_token;
    cancel_offer_flow(ui);
}
