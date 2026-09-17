#pragma once
#include "crew_scene.hpp"
#include "../src/items/quarry_charge.hpp"

inline void arrange_quarry_scene(Game& game,Cosmetics& cosmetics,bool items) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Player)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    auto& player=*get_entity(game,player_state(game,0).controlled);player.cell={20,10};player.facing={1,0};
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::QuarryCharge,3));
    insert_item(player.inventory,make_item(ItemKind::FuseScissors));
    insert_item(player.inventory,make_item(ItemKind::FuseScissors,1,ItemAttribute::Durable));
    if (!items) {
        place_quarry_charge(game,player_state(game,0).controlled.slot);
        player.cell={18,12};player.facing={1,0};
    }
    cosmetics.camera={20,11};
}
