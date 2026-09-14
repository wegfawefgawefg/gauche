#pragma once
#include "woodland_tools_scene.hpp"
#include "../src/item_attribute.hpp"

inline void arrange_ground_tools(Game& game, Cosmetics& cosmetics, Entity& player) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::BirdSeed, ItemKind::ThornCaltrops,
                         ItemKind::ThornCaltrops, ItemKind::HerbBag})
        insert_item(player.inventory, make_item(kind));
    // Distinct attributes prevent the two caltrop examples merging into one stack.
    player.inventory.slots[2] = make_item(ItemKind::ThornCaltrops);
    player.inventory.slots[3] = make_item(ItemKind::ThornCaltrops);
    player.inventory.slots[3].attribute = ItemAttribute::Big;
    player.inventory.selected = 3;
    place_prop(game.stage, {11, 10}, PropKind::BirdSeed);
    for (Cell cell : {Cell{15, 9}, {15, 10}, {15, 11}})
        place_prop(game.stage, cell, PropKind::Thorns, 6);
    game.stage.at({15, 10})->prop.hp = 1;
    for (auto [kind, cell] : {std::pair{EntityKind::Owl, Cell{11, 9}},
                             std::pair{EntityKind::CarrionCrow, Cell{10, 10}},
                             std::pair{EntityKind::Chicken, Cell{11, 11}}}) {
        Entity* bird = get_entity(game, spawn_entity(game, kind, cell));
        if (kind == EntityKind::Chicken) { bird->label_a = 1; bird->sprite = Sprite::Hen; }
        bird->facing = cardinal_toward(cell, {11, 10}, {1, 0});
    }
}
