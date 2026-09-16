#include "../entities/river_raft.hpp"
#include "../items/sled.hpp"
#include "../items/air_bladder.hpp"
#include "currents.hpp"
#include "route.hpp"
#include "room_frame.hpp"
#include "water.hpp"
#include "floating_items.hpp"
#include <algorithm>

std::uint8_t make_current(Cell direction,int strength) {
    if (distance({},direction)!=1 || strength<1 || strength>2) return 0;
    const int heading=direction.x>0 ? 1 : direction.y>0 ? 2 : direction.x<0 ? 3 : 4;
    return static_cast<std::uint8_t>(heading+4*(strength-1));
}
Cell stored_current_direction(const Tile& tile) {
    if (!tile.current || tile.current>8) return {};
    switch ((tile.current-1)%4+1) {
    case 1: return {1,0}; case 2: return {0,1};
    case 3: return {-1,0}; case 4: return {0,-1}; default: return {};
    }
}
int water_current_strength(const Tile& tile) {
    if (!river_water(tile.kind) || tile.surface.still_ticks>0 || !tile.current || tile.current>8) return 0;
    return tile.current<=4 ? 1 : 2;
}
int water_current_beat(const Tile& tile,int normal_ticks) {
    return std::max(1,normal_ticks/std::max(1,water_current_strength(tile)));
}
Cell water_current(const Tile& tile) {
    return water_current_strength(tile)>0 ? stored_current_direction(tile) : Cell{};
}

void step_water_currents(Game& game) {
    if (game.tick%30!=0) return;
    // DRIFT: Gentle water pushes once per second, fast water twice. Floats own
    // their quicker beats; riders/cargo cannot get a second push from this pass.
    // Water cannot crush an actor against a bank or drag an unsupported wader into deep water.
    for (int slot=0;slot<max_entities;++slot) {
        Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        const bool cargo=actor.kind==EntityKind::GroundItem;
        if ((!cargo && !wading_actor(actor)) || actor.hard_blocker || actor.vitals.grip>0 ||
            actor.vitals.rooted>0 || floating_item(actor) || actor.kind==EntityKind::GlassEel ||
            actor.kind==EntityKind::SealThief || actor.kind==EntityKind::BellDiver) continue;
        // Live shoals hold their place in the stream; casts can still retrieve them.
        if (cargo && actor.ground_item.kind==ItemKind::RiverFish) continue;
        if (sled_cargo(game,actor) || ridden_sled(game,actor) || ridden_river_raft(game,actor)) continue;
        const auto& tile=game.stage.at_or_border(actor.cell);
        if (game.tick%static_cast<unsigned>(water_current_beat(tile,60))!=0) continue;
        const Cell flow=water_current(tile);
        if (flow==Cell{}) continue;
        const Cell target=actor.cell+flow;
        if (game.stage.at_or_border(target).kind==TileKind::DeepRiver && tile.kind!=TileKind::DeepRiver &&
            !personal_flotation(actor)) continue;
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
                if (!tile || !river_water(tile->kind) || tile->kind==TileKind::IceHole ||
                    plan.protected_cell(cell) || tile->current!=0) continue;
                const Cell flow=turn_cell({0,1},room.turns);
                if (!walkable(game.stage.at_or_border(cell+flow))) continue;
                tile->current=make_current(flow);
            }
    }
}
