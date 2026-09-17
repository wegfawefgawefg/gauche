#pragma once
#include "woodland_tools_scene.hpp"

// Static armed states, a visible captive and the shared root timer; no gameplay ticks.
inline void arrange_woodland_traps(Game& game, Cosmetics& cosmetics, Entity& player, bool big) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::RopeSnare, ItemKind::SpringTrap, ItemKind::AcornMine})
        insert_item(player.inventory, make_item(kind));
    player.inventory.selected = 1;
    player.inventory.slots[3] = make_item(ItemKind::AcornMine, 3, big ? ItemAttribute::Big : ItemAttribute::None);
    player.vitals.rooted = 150;
    const ItemKind kinds[]{ItemKind::RopeSnare, ItemKind::SpringTrap, ItemKind::AcornMine};
    const Sprite sprites[]{Sprite::SnareSet, Sprite::SpringReady, Sprite::AcornReady};
    for (int index = 0; index < 3; ++index) {
        Entity* trap = get_entity(game, spawn_entity(game, EntityKind::Trap, {12 + index * 2, 10}));
        trap->ground_item = make_item(kinds[index]);
        trap->sprite = sprites[index];
        trap->facing = index == 1 ? Cell{0, 1} : Cell{1, 0};
        trap->health = trap->max_health = 8;
    }
    Entity* caught = get_entity(game, spawn_entity(game, EntityKind::Trap, player.cell));
    caught->ground_item = make_item(ItemKind::RopeSnare);
    caught->sprite = Sprite::SnareTight;
    caught->entity_a = player_state(game,0).controlled;
    caught->label_a = 1; caught->timer_a = 150;
}
