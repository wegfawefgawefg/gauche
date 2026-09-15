#pragma once
#include "../src/ui/interaction.hpp"
#include "../src/debug/panels.hpp"

inline void arrange_ice_equipment_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool sliding) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y})={TileKind::Ice,0,0};
    player.cell={19,16}; player.facing={1,0}; player.light={12,1400,{215,227,224}};
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::SkateBlade));
    insert_item(player.inventory,make_item(ItemKind::SkateBlade,1,ItemAttribute::Big));
    insert_item(player.inventory,make_item(ItemKind::Crampons));
    player.vitals.slide_momentum=sliding ? 10 : 0;
    cosmetics={};cosmetics.camera={19,15};cosmetics.camera_ready=true;
}
