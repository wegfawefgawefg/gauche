#include "industrial_shift.hpp"
#include "mine_crew.hpp"
#include "behavior.hpp"
#include "../props/conveyor.hpp"
#include "../items/sled.hpp"
#include "../world/ground_items.hpp"
#include "../world/floating_items.hpp"
#include <algorithm>

namespace {
const BoilerFeed* supply(const Game& game,const IndustrialShift& shift) {
    for (const auto& feed:game.boiler_feeds) if (feed.tank==shift.tank) return &feed;
    return nullptr;
}
void walk_to(Game& game,int slot,Cell target,bool loaded) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    if (worker.move_wait>0) return;
    const auto next=next_route_cell(game,slot,target,1024);
    if (next && willing_step(game,slot,*next)) {
        if (loaded) worker.move_wait=std::max(worker.move_wait,18);
    } else worker.move_wait=18;
}
bool loose_coal(const Game& game,const Entity& cargo) {
    return cargo.kind==EntityKind::GroundItem && cargo.ground_item.kind==ItemKind::CoalLump &&
        cargo.ground_item.count>0 && cargo.ground_item.flight.slot<0 && cargo.ground_item.anchor.slot<0 &&
        !sled_cargo(game,cargo);
}
}
// Shift relationships are saved independently of the workers' combat slots.
// The hauler carries the actual coal in inventory; no counter represents stock.
// Combat and player whistle orders take precedence, then surviving workers resume.
const IndustrialShift* worker_shift(const Game& game,const Entity& worker) {
    for (const auto& shift:game.industrial_shifts)
        if (worker.entity_a==shift.foreman) return &shift;
    return nullptr;
}
std::optional<Cell> shift_ore_target(const Game& game,const IndustrialShift& shift,int lane) {
    const Cell side{-shift.direction.y,shift.direction.x};
    for (int depth=4;depth<=6;++depth) {
        const Cell cell=shift.origin+Cell{shift.direction.x*depth+side.x*lane,shift.direction.y*depth+side.y*lane};
        const auto& tile=game.stage.at_or_border(cell);
        if (crew_diggable(tile) && tile.contents==ItemKind::CoalLump && tile.content_count>0) return cell;
    }
    return {};
}
bool shift_has_ore(const Game& game,const IndustrialShift& shift) {
    for (int lane=-1;lane<=1;++lane) if (shift_ore_target(game,shift,lane)) return true;
    return false;
}
bool step_shift_hauler(Game& game,int slot,const IndustrialShift& shift) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    if (get_entity(game,shift.hauler)!=&worker) return false;
    Item& held=*worker.inventory.held();
    const bool loaded=held.kind==ItemKind::CoalLump && held.count>0;
    worker.sprite=loaded ? Sprite::PickhandCarry : Sprite::Pickhand;
    const auto* feed=supply(game,shift);
    if (!feed || feed->belts.empty()) return true;
    const Cell intake=feed->belts.front();
    const Tile& tile=game.stage.at_or_border(intake);
    if (!live_belt(tile.prop) || !walkable(tile)) return true;
    if (loaded) {
        // Deliver from the reserved dry side, never wait on the belt's
        // outgoing tile and deadlock the very load we are waiting to clear.
        const Cell stand=intake+cardinal_toward(intake,shift.origin,{1,0});
        if (worker.cell!=stand) {walk_to(game,slot,stand,true);return true;}
        if (worker.move_wait>0) return true;
        const int occupant=entity_at(game,intake,true);
        if (occupant>=0 && occupant!=slot) {worker.move_wait=18;return true;}
        for (const Entity& other:game.entities)
            if (other.kind==EntityKind::GroundItem && other.cell==intake) {worker.move_wait=18;return true;}
        // A full entity pool leaves the load in the worker's hands.
        Entity* cargo=get_entity(game,spawn_entity(game,EntityKind::GroundItem,intake));
        if (!cargo) {worker.move_wait=18;return true;}
        cargo->ground_item=held;cargo->sprite=item_sprite(held);held={};
        worker.facing=cardinal_toward(worker.cell,intake,worker.facing);
        worker.sprite=Sprite::Pickhand;worker.move_wait=30;
        emit_sound(game,SoundId::CoalFeed,intake);return true;
    }
    if (held.count>0 || worker.move_wait>0) return true;
    int best=-1,best_range=1000;
    for (int i=0;i<max_entities;++i) {
        const auto& cargo=game.entities[static_cast<std::size_t>(i)];
        if (!loose_coal(game,cargo) || distance(cargo.cell,shift.origin)>10 ||
            cargo.cell==feed->delivery || std::find(feed->belts.begin(),feed->belts.end(),cargo.cell)!=feed->belts.end()) continue;
        const int range=distance(worker.cell,cargo.cell);
        if (range>=best_range || (range>1 && !next_route_cell(game,slot,cargo.cell,512))) continue;
        best=i;best_range=range;
    }
    if (best<0) {worker.move_wait=30;return true;}
    Entity& cargo=game.entities[static_cast<std::size_t>(best)];
    if (best_range>1) {walk_to(game,slot,cargo.cell,false);return true;}
    if (!walkable(game.stage.at_or_border(cargo.cell))) {worker.move_wait=18;return true;}
    held=cargo.ground_item;held.count=1;
    --cargo.ground_item.count;
    if (cargo.ground_item.count==0) remove_entity(game,{best,cargo.generation});
    worker.sprite=Sprite::PickhandCarry;worker.move_wait=30;
    emit_sound(game,SoundId::CrewHeave,worker.cell);return true;
}
bool valid_industrial_shifts(const Game& game) {
    if (game.industrial_shifts.size()>4) return false;
    for (std::size_t i=0;i<game.industrial_shifts.size();++i) {
        const auto& shift=game.industrial_shifts[i];
        if (!supply(game,shift) || !game.stage.at(shift.origin) || distance({},shift.direction)!=1) return false;
        for (Handle handle:{shift.tank,shift.foreman,shift.hauler})
            if (handle.slot<0 || handle.slot>=max_entities || handle.generation==0) return false;
        const Entity* foreman=get_entity(game,shift.foreman);
        const Entity* hauler=get_entity(game,shift.hauler);
        if ((foreman && foreman->kind!=EntityKind::ShiftForeman) ||
            (hauler && (hauler->kind!=EntityKind::Pickhand || hauler->entity_a!=shift.foreman))) return false;
        const Cell side{-shift.direction.y,shift.direction.x};
        for (int depth:{4,6}) for (int lane:{-1,1})
            if (!game.stage.at(shift.origin+Cell{shift.direction.x*depth+side.x*lane,shift.direction.y*depth+side.y*lane})) return false;
        for (std::size_t j=0;j<i;++j)
            if (game.industrial_shifts[j].tank==shift.tank || game.industrial_shifts[j].foreman==shift.foreman ||
                game.industrial_shifts[j].hauler==shift.hauler) return false;
    }
    return true;
}
