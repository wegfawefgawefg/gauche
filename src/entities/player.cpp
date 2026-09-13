#include "dispatch.hpp"

#include <cstdlib>

namespace {

void pickup_item(Game& game, Entity& player) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& ground = game.entities[static_cast<std::size_t>(slot)];
        if (ground.kind != EntityKind::GroundItem || ground.cell != player.cell) continue;
        Inventory result = player.inventory;
        if (!insert_item(result, ground.ground_item)) return;
        player.inventory = result;
        remove_entity(game, {slot, ground.generation});
        emit_sound(game, SoundId::Confirm, player.cell, false);
        return;
    }
}

Cell facing_from_aim(Cell aim, Cell fallback) {
    if (aim.x == 0 && aim.y == 0) return fallback;
    if (std::abs(aim.x) >= std::abs(aim.y)) return {aim.x > 0 ? 1 : -1, 0};
    return {0, aim.y > 0 ? 1 : -1};
}

} // namespace

void init_player(Entity& entity) {
    entity.sprite = Sprite::Player;
    entity.health = entity.max_health = 100;
    entity.move_interval = 7;
    entity.impassable = true;
    insert_item(entity.inventory, make_item(ItemKind::Wall, 99));
    insert_item(entity.inventory, make_item(ItemKind::Fist));
    insert_item(entity.inventory, make_item(ItemKind::Medkit, 10));
    insert_item(entity.inventory, make_item(ItemKind::Bandage, 10));
    insert_item(entity.inventory, make_item(ItemKind::Bandaid, 20));
    insert_item(entity.inventory, make_item(ItemKind::ConductorHat));
}

void drop_player_item(Game& game, Entity& player) {
    Item& item = *player.inventory.held();
    if (item.kind == ItemKind::None || item.kind == ItemKind::Fist) return;
    for (const Entity& entity : game.entities)
        if (entity.kind == EntityKind::GroundItem && entity.cell == player.cell) return;
    const Handle dropped = spawn_entity(game, EntityKind::GroundItem, player.cell);
    if (Entity* entity = get_entity(game, dropped)) {
        entity->ground_item = item;
        entity->sprite = item_sprite(item.kind);
        item = {};
        emit_sound(game, SoundId::Drop, player.cell);
    }
}

void step_player(Game& game, int slot, const Input& input) {
    Entity& player = game.entities[static_cast<std::size_t>(slot)];
    if (input.select >= 0 && input.select < quick_slots) player.inventory.selected = input.select;

    // MOVEMENT: Aiming can turn the held item while a tile step recovers.
    if (player.move_wait == 0 && (input.move.x != 0 || input.move.y != 0)) {
        Cell movement = input.move;
        if (movement.x != 0) movement.y = 0;
        move_entity(game, slot, player.cell + movement);
    }
    player.facing = facing_from_aim(input.aim, player.facing);

    // INTERACTIONS: The player owns pickup, fixture use, and the held item.
    if (input.pickup) pickup_item(game, player);
    if (input.interact && !interact_with_fixture(game, player.owner, player.cell))
        interact_with_fixture(game, player.owner, player.cell + player.facing);
    if (input.drop) drop_player_item(game, player);
    if (input.reload) reload_held_item(game, slot);
    if (input.use) {
        const Cell target = player.cell + input.aim;
        if (!interact_with_fixture(game, player.owner, target))
            use_held_item(game, slot, target);
    }
}
