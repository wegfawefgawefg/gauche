#include "toss.hpp"
#include "../items/action.hpp"
#include "../items/sled.hpp"
#include "../surfaces/slip.hpp"
#include "../world/water.hpp"
#include "../world/chasm.hpp"
#include "../projectiles/arrow_contact.hpp"

#include <algorithm>
#include <cmath>

namespace {
Cell position_at(const ActorToss& toss) {
    const int steps=(toss_duration-toss.ticks)/toss_beat;
    return toss.origin+Cell{toss.direction.x*steps,toss.direction.y*steps};
}
void land(Game& game,int slot,bool impact) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const ActorToss flight=actor.toss;
    actor.toss={};
    if (actor.health<=0) return;
    if (chasm_contact(game,slot) || deep_river_contact(game,slot)) return;
    const Tile* tile=game.stage.at(actor.cell);
    // Deep water is an actual hole, like a broken bridge. Flying species never
    // enter this system. A shortened flight does not teleport to a safe bank.
    if (!tile || (tile->kind==TileKind::Water && !gap_flyer(actor))) {
        crush_entity(game,slot,flight.source);
        emit_sound(game,SoundId::BridgeSplash,actor.cell);
        return;
    }
    if (impact) {
        damage_entity(game,slot,12,flight.source,false,flight.instigator);
        if (actor.health>0) apply_stun(actor,30);
    }
    emit_sound(game,impact ? SoundId::TossImpact : SoundId::TossLand,actor.cell);
    actor.move_wait=std::max(actor.move_wait,6);
    enter_actor_cell(game,slot);
    if (!impact && actor.health>0) slip_on_surface(game,slot,flight.direction);
}
}

bool toss_actor(Game& game,int slot,Cell direction,Cell source,Handle instigator) {
    if (slot<0 || slot>=max_entities || distance({},direction)!=1) return false;
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (!wading_actor(actor) || actor.hard_blocker || actor.vitals.grip>0 ||
        actor.vitals.rooted>0 || ridden_sled(game,actor) || actor.kind==EntityKind::Sled) return false;
    if (actor.kind==EntityKind::Player) cancel_item_action(actor);
    actor.block_ticks=0;
    actor.vitals.slide_momentum=0;
    actor.toss={actor.cell,direction,source,instigator,toss_duration};
    return true;
}

void step_actor_tosses(Game& game) {
    for (int slot=0;slot<max_entities;++slot) {
        Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.toss.ticks==0) continue;
        if (actor.health<=0 || actor.kind==EntityKind::None) { actor.toss={}; continue; }
        // A portal or another displacement owns its new position, not this throw.
        if (actor.cell!=position_at(actor.toss)) { land(game,slot,false); continue; }
        --actor.toss.ticks;
        if (actor.toss.ticks%toss_beat!=0) continue;
        const Cell next=actor.cell+actor.toss.direction;
        const Tile* tile=game.stage.at(next);
        const bool open=tile && (walkable(*tile) || tile->kind==TileKind::Water || open_drop(tile->kind));
        if (!open || entity_at(game,next,true)>=0) { land(game,slot,true); continue; }
        actor.cell=next;
        contact_arrows(game,slot);
        if (actor.health<=0) continue;
        if (actor.toss.ticks==0) land(game,slot,false);
    }
}

bool valid_actor_toss(const Entity& actor) {
    const ActorToss& toss=actor.toss;
    if (toss.ticks<0 || toss.ticks>toss_duration ||
        toss.instigator.slot< -1 || toss.instigator.slot>=max_entities) return false;
    if (toss.ticks==0) return toss.origin==Cell{} && toss.direction==Cell{} &&
        toss.source==Cell{} && toss.instigator==Handle{};
    return actor.health>0 && actor.move_interval>0 && !actor.hard_blocker &&
        distance({},toss.direction)==1;
}

float actor_toss_height(const Entity& actor) {
    const float progress=1-static_cast<float>(actor.toss.ticks)/toss_duration;
    return actor.toss.ticks>0 ? std::sin(progress*3.14159265F)*.65F : 0;
}
