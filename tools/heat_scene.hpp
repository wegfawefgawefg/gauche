#pragma once
#include "../src/items/heat_capsule.hpp"
#include "../src/surfaces/interaction.hpp"
#include "../src/surfaces/render.hpp"
#include "../src/surfaces/temperature.hpp"

inline void arrange_heat_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {x >= 18 && x <= 21 ? TileKind::Ice : TileKind::Snow, 0, 0};
    player.cell = {21, 12};
    player.facing = {1, 0};
    player.scorch_ticks = 0;
    player.freeze_ticks = 180;
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 3));
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 3));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 4, ItemAttribute::Big));
    pour_surface(game, {22, 12}, LiquidKind::Oil, 900);
    *game.stage.at({20, 12}) = {TileKind::IceHole, 0, 0};
    freeze_water(game, {20, 12}, 480);
    player.inventory.selected = 2;
    use_held_item(game, game.players[0].slot, player.cell);
    player.inventory.selected = 0;
    // CAPTURE: Place visible steam and surface fire without advancing the simulation.
    game.tick = 36;
    observe_surfaces(cosmetics, game, player.cell);
    cosmetics.camera = {21, 12};
    cosmetics.camera_ready = true;
}
