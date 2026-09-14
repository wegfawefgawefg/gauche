#include "dispatch.hpp"
#include "../items/bow.hpp"
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

void step_player(Game& game, int slot, const Input& input) {
    Entity& player = game.entities[static_cast<std::size_t>(slot)];
    if (input.select >= 0 && input.select < quick_slots) player.inventory.selected = input.select;

    // CONTACT: label_a/point_a latch a held blocked direction; timer_a limits rapid taps.
    if (input.move.x == 0 && input.move.y == 0) player.label_a = 0;
    else {
        Cell movement = input.move;
        if (movement.x != 0) movement.y = 0;
        player.facing = movement;
        if (player.move_wait == 0) {
            const Cell destination = player.cell + movement;
            if (move_entity(game, slot, destination)) player.label_a = 0;
            else {
                if ((player.label_a == 0 || player.point_a != destination) && player.timer_a == 0) {
                    const Tile* tile = game.stage.at(destination);
                    const bool wood = tile != nullptr &&
                        (tile->prop.kind == PropKind::Crate || tile->prop.kind == PropKind::RottenLog);
                    emit_sound(game, wood ? SoundId::BumpWood : SoundId::BumpStone, destination);
                    player.timer_a = 15;
                }
                player.label_a = 1;
                player.point_a = destination;
            }
        }
    }
    // AIM: Explicit aim overrides movement, including an unsuccessful step.
    player.facing = facing_from_aim(input.aim, player.facing);
    collect_coins(game, player);

    // INTERACTIONS: The player owns pickup, fixture use, and the held item.
    if (input.pickup) {
        cancel_item_action(player);
        pickup_or_drop(game, player);
        return;
    }
    if (input.interact && !interact_with_fixture(game, player.owner, player.cell))
        interact_with_fixture(game, player.owner, player.cell + player.facing);
    if (input.drop) drop_player_item(game, player);
    if (step_melee_action(game, slot, input)) return;
    if (step_bow(game, slot, input)) return;
    if (input.cancel_use) return;
    if (input.reload) reload_held_item(game, slot);
    if (input.use) {
        const Cell target = aimed_item_target(player, input.aim,
                                               item_pattern(*player.inventory.held()));
        if (!interact_with_fixture(game, player.owner, target))
            use_held_item(game, slot, target);
    }
}
