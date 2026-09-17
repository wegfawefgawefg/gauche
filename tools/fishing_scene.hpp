#pragma once
#include "../src/projectiles/fishing.hpp"
#include "../src/item_attribute.hpp"

// DISPLAY: Loose cargo remains a real item beside an outbound or attached hook.
inline void arrange_fishing_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool caught) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y=8;y<=17;++y)
        for (int x=12;x<=28;++x)
            *game.stage.at({x,y}) = {x>=19 && x<=24 && y>=10 && y<=14 ? TileKind::ShallowWater : TileKind::Snow,0,0};
    player.cell = {18,12}; player.facing = {1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {14,1700,{211,225,225}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::FishingLine));
    insert_item(player.inventory, make_item(ItemKind::FishingLine,1,ItemAttribute::Long));
    insert_item(player.inventory, make_item(ItemKind::FishingLine,1,ItemAttribute::Durable));
    player.inventory.slots[0].uses = 13;
    const Handle handle = spawn_entity(game,EntityKind::GroundItem,{23,12});
    Entity* cargo = get_entity(game,handle);
    cargo->ground_item = make_item(ItemKind::Pickaxe,1,ItemAttribute::Heavy);
    cargo->ground_item.durability = 17; cargo->sprite = item_sprite(cargo->ground_item);
    launch_fishing_hook(game,player_state(game,0).controlled.slot,*player.inventory.held(),{1,0});
    for (Entity& hook : game.entities) {
        if (hook.kind != EntityKind::Projectile) continue;
        hook.cell = caught ? cargo->cell : Cell{21,12};
        hook.label_b = caught ? FishingCargo : FishingOut;
        hook.counter_a = caught ? 0 : 3;
        hook.timer_b = 2;
        if (caught) { hook.entity_b = handle; hook.point_b = cargo->cell; }
    }
    player.inventory.selected = 2;
    cosmetics = {}; cosmetics.camera = {21,12}; cosmetics.camera_ready = true;
    game.tick = 60;
}
