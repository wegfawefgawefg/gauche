#pragma once
#include "../src/projectiles/harpoon.hpp"

inline void arrange_harpoon_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=8;y<23;++y)
        for (int x=8;x<33;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={16,16}; player.facing={1,0}; player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::HarpoonGun));
    Item longer=make_item(ItemKind::HarpoonGun); longer.attribute=ItemAttribute::Long;
    insert_item(player.inventory,longer);
    const Handle victim=spawn_entity(game,EntityKind::AvalancheRam,{23,16});
    const int owner=static_cast<int>(&player-game.entities.data());
    launch_harpoon(game,owner,*player.inventory.held(),{1,0});
    Entity& shot=*get_entity(game,player.inventory.held()->flight);
    shot.entity_b=victim; shot.label_b=1; shot.counter_a=0; shot.cell={23,16};
    cosmetics={}; cosmetics.camera={21,15}; cosmetics.camera_ready=true;
}
