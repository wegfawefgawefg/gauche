#include "../entities/river_raft.hpp"
#include "../items/sled.hpp"
#include "currents.hpp"
#include "route.hpp"
#include "room_frame.hpp"
#include "water.hpp"
#include "floating_items.hpp"

Cell water_current(const Tile& tile) {
    if (!shallow_water(tile.kind) || tile.surface.still_ticks>0) return {};
    switch (tile.current) {
    case 1: return {1,0}; case 2: return {0,1};
    case 3: return {-1,0}; case 4: return {0,-1}; default: return {};
    }
}

void step_water_currents(Game& game) {
    if (game.tick%60!=0) return;
    // DRIFT: One real step per second. Float-equipped cargo owns its quicker beat.
    // Water cannot crush an actor against a bank or drag someone into deep water.
    for (int slot=0;slot<max_entities;++slot) {
        Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        const bool cargo=actor.kind==EntityKind::GroundItem;
        if ((!cargo && !wading_actor(actor)) || actor.hard_blocker || actor.vitals.grip>0 ||
            actor.vitals.rooted>0 || floating_item(actor) || actor.kind==EntityKind::GlassEel ||
            actor.kind==EntityKind::SealThief || actor.kind==EntityKind::BellDiver) continue;
        // Live shoals hold their place in the stream; casts can still retrieve them.
        if (cargo && actor.ground_item.kind==ItemKind::RiverFish) continue;
        if (sled_cargo(game,actor) || ridden_sled(game,actor) || ridden_river_raft(game,actor)) continue;
        const Cell flow=water_current(game.stage.at_or_border(actor.cell));
        if (flow==Cell{}) continue;
        const Cell target=actor.cell+flow;
        if (!float_cell_free(game,target,slot)) continue;
        if (cargo) {
            if (actor.ground_item.flight.slot>=0) continue; // A hook/return owns this item.
            actor.cell=target;
            enter_actor_cell(game,slot);
        } else {
            const Cell facing=actor.facing;
            const int wait=actor.move_wait;
            move_entity(game,slot,target,false);
            actor.facing=facing; actor.move_wait=wait;
        }
        emit_sound(game,SoundId::CurrentDrift,target);
    }
}

void place_water_currents(Game& game, const FloorPlan& plan) {
    for (const RoomPlan& room : plan.rooms) {
        if (room.shape==RoomShape::ThawCavern) continue;
        if (room.role!=RoomRole::Brook && room.role!=RoomRole::Reservoir) continue;
        for (int y=-room.half_height;y<=room.half_height;++y)
            for (int x=-room.half_width;x<=room.half_width;++x) {
                const Cell cell=room.center+Cell{x,y};
                Tile* tile=game.stage.at(cell);
                if (!tile || !shallow_water(tile->kind) || tile->kind==TileKind::IceHole ||
                    plan.protected_cell(cell) || tile->current!=0) continue;
                const Cell flow=turn_cell({0,1},room.turns);
                if (!walkable(game.stage.at_or_border(cell+flow))) continue;
                tile->current=static_cast<std::uint8_t>(flow.x>0 ? 1 : flow.y>0 ? 2 : flow.x<0 ? 3 : 4);
            }
    }
}
