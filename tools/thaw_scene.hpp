#pragma once
#include "../src/items/thaw_charge.hpp"
#include "../src/particles/templates.hpp"

inline void arrange_thaw_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool burst) {
    for (Entity& actor : game.entities)
        if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; }
    for (int y=8;y<=16;++y)
        for (int x=12;x<=27;++x) {
            Tile& tile=*game.stage.at({x,y});
            tile={x==22 || x==23 ? TileKind::Wall : TileKind::Ruin,100,0};
            tile.material=TileMaterial::Ice;
        }
    player.cell={21,12}; player.facing={1,0}; player.scorch_ticks=0;
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::ThawCharge,3));
    insert_item(player.inventory,make_item(ItemKind::ThawCharge,3,ItemAttribute::Big));
    insert_item(player.inventory,make_item(ItemKind::ThawCharge,3,ItemAttribute::Strong));
    use_held_item(game,player_state(game,0).controlled.slot,player.cell);
    player.cell={19,12};
    if (burst)
        for (int slot=0;slot<max_entities;++slot) {
            Entity& shot=game.entities[static_cast<std::size_t>(slot)];
            if (shot.kind!=EntityKind::Projectile) continue;
            shot.timer_a=0; step_thaw_charge(game,slot);
        }
    for (int i=0;i<game.sound_count;++i)
        spawn_sound_effect(cosmetics,game.sounds[static_cast<std::size_t>(i)],static_cast<std::uint64_t>(300+i));
    cosmetics.camera={21,12}; cosmetics.camera_ready=true;
}
