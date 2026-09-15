#include "../items/chain_hook.hpp"
#include "../items/pocket_drill.hpp"
#include "../items/magnet.hpp"
#include "../items/arc_torch.hpp"
#include "../items/sled.hpp"
#include "../items/ice_anchor.hpp"
#include "dispatch.hpp"
#include "../items/effigy_mask.hpp"
#include "../items/heat_siphon.hpp"
#include "../items/harpoon.hpp"
#include "player_movement.hpp"
#include "../items/bow.hpp"
#include "../items/rivet_gun.hpp"
#include "../items/storm_lantern.hpp"
#include "../items/quarry_tools.hpp"
#include "../items/action.hpp"
#include "../items/ground_interaction.hpp"
#include "../item_pattern.hpp"
#include "../world/ground_items.hpp"
#include "../world/loot.hpp"

#include <cstdlib>

namespace {

Cell facing_from_aim(Cell aim, Cell fallback) {
    if (aim.x == 0 && aim.y == 0) return fallback;
    if (std::abs(aim.x) >= std::abs(aim.y)) return {aim.x > 0 ? 1 : -1, 0};
    return {0, aim.y > 0 ? 1 : -1};
}

} // namespace

void init_player(Entity& entity) {
    entity.sprite = Sprite::Player;
    entity.light = {8, 1350, {255, 247, 224}};
    entity.health = entity.max_health = 100;
    entity.move_interval = 7;
    entity.impassable = true;
    insert_item(entity.inventory, make_item(ItemKind::Fist));
    insert_item(entity.inventory, make_item(ItemKind::Bandage, 2));
}

void step_player(Game& game, int slot, const Input& incoming) {
    Input input=incoming;
    Entity& player = game.entities[static_cast<std::size_t>(slot)];
    if (input.select >= 0 && input.select < quick_slots) {
        if (input.select != player.inventory.selected) player.block_ticks = 0;
        player.inventory.selected = input.select;
    }
    if (input.cancel_use || input.pickup || input.drop || input.interact) player.block_ticks = 0;

    sled_player_input(game,slot,input);
    move_player(game, slot, input.move);
    // AIM: Explicit aim overrides movement, including an unsuccessful step.
    player.facing = facing_from_aim(input.aim, player.facing);
    collect_coins(game, player);

    // INTERACTIONS: The player owns pickup, fixture use, and the held item.
    if (input.pickup || input.drop || input.interact || input.cancel_use) {
        release_held_harpoon(game,player);release_held_chain(game,player);
    }
    if (input.pickup) {
        cancel_item_action(player);
        // WEST: Objectives and mechanisms share pickup, before the empty-ground drop fallback.
        if (!use_pickup_fixture(game,player)) pickup_or_drop(game, player);
        return;
    }
    if (input.interact && !interact_with_fixture(game, player.owner, player.cell))
        interact_with_fixture(game, player.owner, player.cell + player.facing);
    if (input.drop) drop_player_item(game, player);
    if (step_pocket_drill(game,slot,input)) return;
    if (step_arc_torch(game,slot,input)) return;
    if (step_magnet(game,slot,input)) return;
    if (step_rivet_action(game,slot,input)) return;
    if (step_anchor_action(game,slot,input)) return;
    if (step_effigy_mask(game,slot,input)) return;
    if (step_chain_action(game,slot,input)) return;
    if (step_harpoon_action(game,slot,input)) return;
    if (step_siphon_action(game,slot,input)) return;
    if (step_lantern_action(game,slot,input)) return;
    if (step_melee_action(game, slot, input)) return;
    if (step_ice_brick(game, slot, input)) return;
    if (step_bow(game, slot, input)) return;
    if (input.cancel_use) return;
    if (input.reload) reload_held_item(game, slot);
    if (input.use) {
        const Cell target = aimed_item_target(player, input.aim,
                                               item_pattern(*player.inventory.held()));
        if (!interact_with_fixture(game, player.owner, target, true))
            use_held_item(game, slot, target);
    }
}
