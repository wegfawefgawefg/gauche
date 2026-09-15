#pragma once
#include "quarry_scene.hpp"

inline void arrange_hammer_scene(Game& game,Cosmetics& cosmetics) {
    arrange_quarry_scene(game,cosmetics,true);
    auto& player=*get_entity(game,game.players[0]);
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::PressHammer));
    insert_item(player.inventory,make_item(ItemKind::RubberMallet));
    insert_item(player.inventory,make_item(ItemKind::RubberMallet,1,ItemAttribute::Durable));
    player.inventory.selected=1;
}
