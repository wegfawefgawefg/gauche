#include "conveyor.hpp"
#include "../world/water.hpp"
#include "../world/floating_items.hpp"
#include "../items/sled.hpp"
#include <algorithm>
#include <array>

bool live_belt(const Prop& prop) { return prop.kind==PropKind::Conveyor && !prop.broken && prop.hp>0; }
Cell belt_direction(const Prop& prop) {
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    return live_belt(prop) ? directions[prop.variant&3U] : Cell{};
}
// A straight run is one mechanism, bounded to 32 sections. Bends and a change
// of power source separate drives. Broken sections physically disconnect runs.
std::vector<Cell> belt_run(const Stage& stage,Cell cell) {
    const Prop& origin=stage.at_or_border(cell).prop;
    if (!live_belt(origin)) return {};
    const Cell direction=belt_direction(origin);
    std::vector<Cell> cells{cell};
    for (int sign:{-1,1}) {
        Cell next=cell;
        while (cells.size()<32) {
            next=next+Cell{direction.x*sign,direction.y*sign};
            const auto& prop=stage.at_or_border(next).prop;
            if (!live_belt(prop) || prop.variant!=origin.variant) break;
            cells.push_back(next);
        }
    }
    return cells;
}
void step_belt_timer(Prop& prop) {
    if (live_belt(prop) && prop.growth_ticks>0) --prop.growth_ticks;
}
namespace {
bool carried(const Game& game,const Entity& actor) {
    if (actor.kind==EntityKind::None || (actor.health<=0 && actor.kind!=EntityKind::GroundItem) || (actor.hard_blocker && actor.kind!=EntityKind::BoilerTank) ||
        actor.vitals.grip>0 || actor.vitals.rooted>0 || sled_cargo(game,actor) || ridden_sled(game,actor)) return false;
    if (actor.kind==EntityKind::GroundItem) return actor.ground_item.kind!=ItemKind::None && actor.ground_item.count>0 &&
        actor.ground_item.flight.slot<0 && !floating_item(actor);
    return wading_actor(actor) || actor.kind==EntityKind::BoilerTank;
}
void advance(Game& game,const std::vector<Cell>& manual) {
    struct Move { Handle actor; Cell from,to; };
    std::vector<Move> moves;
    // Occupancy is captured before any movement. A front object moving away
    // cannot allow a trailing object to advance only because of its slot order.
    std::vector<int> occupied(game.stage.tiles.size(),0);
    for (const Entity& actor:game.entities)
        if (actor.kind!=EntityKind::None && game.stage.at(actor.cell) &&
            (actor.impassable || actor.kind==EntityKind::GroundItem))
            ++occupied[static_cast<std::size_t>(actor.cell.y*game.stage.width+actor.cell.x)];
    std::vector<bool> reserved(game.stage.tiles.size(),false);
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        if (!carried(game,actor) || actor.birth_tick==game.tick) continue;
        const Prop& prop=game.stage.at_or_border(actor.cell).prop;
        if (!live_belt(prop) || prop.growth_ticks>0) continue;
        if (manual.empty() ? (prop.variant&belt_manual)!=0 :
            std::find(manual.begin(),manual.end(),actor.cell)==manual.end()) continue;
        const Cell to=actor.cell+belt_direction(prop);
        const Tile* tile=game.stage.at(to);
        if (!tile || (!walkable(*tile) && tile->kind!=TileKind::Chasm)) continue;
        const auto index=static_cast<std::size_t>(to.y*game.stage.width+to.x);
        if (occupied[index]>0 || reserved[index]) continue;
        reserved[index]=true;
        moves.push_back({{slot,actor.generation},actor.cell,to});
    }
    for (const Move& move:moves) {
        Entity* actor=get_entity(game,move.actor);
        if (!actor || actor->cell!=move.from || !carried(game,*actor)) continue;
        // Earlier entry effects can move another body or close a gate. Recheck
        // destinations before applying, without turning blockage into crushing.
        const Tile* tile=game.stage.at(move.to);
        if (!tile || (!walkable(*tile) && tile->kind!=TileKind::Chasm) || entity_at(game,move.to,true)>=0) continue;
        bool cargo=false;
        for (const Entity& other:game.entities)
            if (other.kind==EntityKind::GroundItem && other.cell==move.to) cargo=true;
        if (cargo) continue;
        actor->cell=move.to;
        enter_actor_cell(game,move.actor.slot);
        emit_sound(game,SoundId::BeltCarry,move.to);
    }
}
}
void step_conveyors(Game& game) { if (game.tick%belt_beat==0) advance(game,{}); }
bool crank_belt(Game& game,Cell cell) {
    const Prop& prop=game.stage.at_or_border(cell).prop;
    if (!live_belt(prop) || !(prop.variant&belt_manual) || prop.growth_ticks>0) return false;
    const auto run=belt_run(game.stage,cell);
    // A shoe anywhere on a connected mechanism prevents turning it.
    for (Cell section:run) if (game.stage.at(section)->prop.growth_ticks>0) return false;
    advance(game,run); return true;
}
bool brake_belt(Game& game,Cell cell) {
    const auto run=belt_run(game.stage,cell);
    if (run.empty()) return false;
    for (Cell section:run) if (game.stage.at(section)->prop.growth_ticks>0) return false;
    for (Cell section:run) game.stage.at(section)->prop.growth_ticks=360;
    return true;
}
void hit_belt_brake(Game& game,Cell cell,int damage) {
    if (damage<16 || game.stage.at_or_border(cell).prop.growth_ticks==0) return;
    for (Cell section:belt_run(game.stage,cell)) game.stage.at(section)->prop.growth_ticks=0;
    emit_sound(game,SoundId::BrakeSnap,cell);
}
