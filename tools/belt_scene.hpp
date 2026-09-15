#pragma once
#include "crew_scene.hpp"
#include "../src/world/assembly.hpp"
#include "../src/props/conveyor.hpp"
#include "../src/entities/rivet_gunner.hpp"

inline void arrange_belt_scene(Game& game,Cosmetics& cosmetics) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Player)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    for (auto& tile:game.stage.tiles) { tile.prop={}; if(tile.kind==TileKind::Wall && tile.required_dig_power==1) tile={TileKind::Ruin,0,0}; }
    FloorPlan plan;plan.width=36;plan.height=28;plan.protected_cells.assign(36*28,0);
    RoomPlan room;room.center={18,13};room.role=RoomRole::AssemblyLine;room.half_width=9;room.half_height=7;plan.rooms.push_back(room);
    for(int y=0;y<28;++y) for(int x=17;x<=19;++x) plan.protected_cells[static_cast<std::size_t>(y*36+x)]=1;
    place_assembly_belts(game,plan);assembly_supplies(game,room);
    brake_belt(game,{14,16});
    auto& player=*get_entity(game,game.players[0]);player.cell={22,11};player.facing={0,-1};
    player.inventory={};insert_item(player.inventory,make_item(ItemKind::BeltCrank));insert_item(player.inventory,make_item(ItemKind::BrakeShoe));
    spawn_entity(game,EntityKind::RivetGunner,{22,15});
    cosmetics.camera={18,13};
}
