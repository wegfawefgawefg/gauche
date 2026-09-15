#pragma once
#include "../src/entities/boiler_porter.hpp"
#include "../src/entities/boiler_tank.hpp"
#include "../src/props/interaction.hpp"
#include "../src/particles/templates.hpp"

// DISPLAY: Committed nozzle, pressure gauge and plugged vessel share the actual state.
inline void arrange_boiler_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool sealed) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    Entity* porter = get_entity(game,spawn_boiler_porter(game,{17,13}));
    Entity* tank = get_entity(game,porter->entity_a);
    tank->counter_a = 86; tank->facing = {1,0};
    tank->ground_item = make_item(ItemKind::PressureValve);
    tank->light = {3,650,{240,177,91}};
    if (sealed) { tank->timer_b = 371; tank->sprite = Sprite::BoilerPlugged; }
    else arm_boiler(game,*tank);
    Entity* second = get_entity(game,spawn_entity(game,EntityKind::BoilerTank,{25,14}));
    second->counter_a = 12; second->counter_b = 0; second->facing = {0,1};
    second->ground_item = make_item(ItemKind::PressureValve);
    place_prop(game.stage,{24,18},PropKind::MaintenanceLocker);
    player.cell = {21,17}; player.light = {10,1250,{210,226,224}};
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::PressureValve));
    insert_item(player.inventory,make_item(ItemKind::Sealant));
    insert_item(player.inventory,make_item(ItemKind::CoalLump,4));
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{24,16},DebrisKind::BrassRivet,6,6157,true);
}
