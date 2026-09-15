#pragma once
#include "../src/entities/gate.hpp"
#include "../src/props/doorstop.hpp"
#include "../src/world/sluice.hpp"

inline void arrange_sluice_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=8;y<24;++y) for(int x=8;x<34;++x)
        *game.stage.at({x,y})={TileKind::Ruin,0,0};
    FloorPlan plan; plan.width=game.stage.width; plan.height=game.stage.height;
    plan.protected_cells.resize(static_cast<std::size_t>(plan.width*plan.height));
    RoomPlan room; room.center={22,14}; room.role=RoomRole::Reservoir; room.half_width=8; room.half_height=7;
    place_sluice_chamber(game,plan,room);
    player.cell={18,15}; player.facing={0,1}; player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::EmergencyDoorstop));
    Item sturdy=make_item(ItemKind::EmergencyDoorstop,1,ItemAttribute::Durable);
    sturdy.durability=37; insert_item(player.inventory,sturdy);
    for (int slot=0;slot<max_entities;++slot) {
        Entity& gate=game.entities[static_cast<std::size_t>(slot)];
        if (gate.kind!=EntityKind::EncounterGate) continue;
        place_doorstop(game,gate.cell,sturdy); request_gate(game,gate,false);
        gate.timer_a=80;
    }
    cosmetics={}; cosmetics.camera={20,16}; cosmetics.camera_ready=true;
}
