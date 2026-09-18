// Carry one light body or prop. References include generations; changing hands releases it.
#include "basic_actions.hpp"
#include "../combat/toss.hpp"
#include "../props/interaction.hpp"
#include "../world/chasm.hpp"
#include "../world/water.hpp"
#include "../entities/attacks.hpp"
#include <algorithm>
namespace {
bool light_prop(const Prop& prop) {
    if (prop.broken || !prop.hp) return false;
    switch (prop.kind) {
    case PropKind::Crate:case PropKind::ClayPot:case PropKind::RottenLog:case PropKind::Pallet:
    case PropKind::SnowCache:case PropKind::IceBlock:case PropKind::StrawDecoy:
    case PropKind::Scarecrow:case PropKind::FrozenLunchTin:case PropKind::BonePile:return true;
    default:return false;
    }
}
bool light_actor(const Entity& actor) {
    if (actor.hard_blocker || actor.basic.carried_by.slot>=0 || actor.vitals.grip || actor.vitals.rooted || actor.toss.ticks) return false;
    if (actor.kind==EntityKind::GroundItem) return actor.ground_item.flight.slot<0;
    // Shared mobility/size rule: machinery and large bodies keep their footing.
    return actor.kind!=EntityKind::Player && actor.health>0 && actor.move_interval>0 &&
        actor.max_health<=80 && actor.kind!=EntityKind::Train && actor.kind!=EntityKind::Sled && actor.kind!=EntityKind::RiverRaft;
}
void land_prop(Game& game,Entity& user) {
    auto& state=user.basic;
    Tile* tile=game.stage.at(state.prop_cell);
    if (tile && walkable(*tile) && tile->prop.kind==PropKind::None) tile->prop=state.held_prop;
    else emit_sound(game,tile && open_drop(tile->kind) ? SoundId::ChasmFall : SoundId::BoxBreak,state.prop_cell);
    state.held_prop={};state.prop_ticks=0;state.prop_cell=state.prop_direction={};
}
}
void release_grapple(Game& game,Entity& user,bool throwing,Cell direction) {
    auto& state=user.basic;
    if (Entity* cargo=get_entity(game,state.grabbed)) {
        const int slot=state.grabbed.slot;
        cargo->basic.carried_by={};
        Cell destination=user.cell+direction;
        const Tile* tile=game.stage.at(destination);
        if (!tile || (!walkable(*tile) && !open_drop(tile->kind)) || entity_at(game,destination,true)>=0) destination=user.cell;
        cargo->cell=destination;
        if (throwing && cargo->kind!=EntityKind::GroundItem &&
            toss_actor(game,slot,direction,user.cell,{static_cast<int>(&user-game.entities.data()),user.generation})) {
            damage_entity(game,slot,12+(has_artifact(user,ArtifactKind::Iron) ? 8 : 0),user.cell,true,
                {static_cast<int>(&user-game.entities.data()),user.generation});
        } else {
            if (throwing && cargo->kind==EntityKind::GroundItem) {
                for (int n=0;n<3;++n) {
                    const Cell next=cargo->cell+direction;
                    const Tile* ground=game.stage.at(next);
                    if (!ground || (!walkable(*ground) && !open_drop(ground->kind)) || entity_at(game,next,true)>=0) break;
                    cargo->cell=next;
                }
            }
            enter_actor_cell(game,slot);
        }
        basic_contact(game,user,cargo->cell);
        if (cargo->health>0) cargo->move_wait=std::max(cargo->move_wait,12);
    }
    state.grabbed={};
    if (state.held_prop.kind!=PropKind::None && state.prop_ticks==0) {
        state.prop_cell=user.cell;state.prop_direction=direction;
        if (throwing) state.prop_ticks=24;else land_prop(game,user);
    }
}
bool use_grapple(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    auto& state=user.basic;
    if (state.grabbed.slot>=0 || (state.held_prop.kind!=PropKind::None && state.prop_ticks==0)) {
        release_grapple(game,user,true,direction);emit_sound(game,SoundId::GrappleThrow,user.cell);return true;
    }
    if (state.prop_ticks>0) return false;
    const int reach=has_artifact(user,ArtifactKind::Oversized) ? 2 : 1;
    const int width=has_artifact(user,ArtifactKind::Sweeping) ? 1 : 0;
    for (int along=1;along<=reach;++along) for (int lane=-width;lane<=width;++lane) {
        const Cell cell=user.cell+Cell{direction.x*along-direction.y*lane,direction.y*along+direction.x*lane};
        if (!clear_attack_sight(game,user.cell,cell,false,true)) continue;
        basic_contact(game,user,cell);
        for (int i=0;i<max_entities;++i) {
            auto& cargo=game.entities[static_cast<std::size_t>(i)];
            if (i==slot || cargo.cell!=cell || !light_actor(cargo)) continue;
            state.grabbed={i,cargo.generation};cargo.basic.carried_by={slot,user.generation};cargo.cell=user.cell;
            emit_sound(game,SoundId::GrappleLift,cell);return true;
        }
        Tile* tile=game.stage.at(cell);
        if (tile && light_prop(tile->prop)) {state.held_prop=tile->prop;tile->prop={};emit_sound(game,SoundId::GrappleLift,cell);return true;}
    }
    return false;
}
void step_grapple(Game& game,int slot) {
    auto& user=game.entities[static_cast<std::size_t>(slot)];
    auto& state=user.basic;
    if (state.carried_by.slot>=0) {
        const Entity* carrier=get_entity(game,state.carried_by);
        if (!carrier || carrier->health<=0 || carrier->basic.grabbed!=Handle{slot,user.generation}) state.carried_by={};
        else user.cell=carrier->cell;
    }
    if (state.grabbed.slot>=0 && !get_entity(game,state.grabbed)) state.grabbed={};
    if ((user.owner>=0 && has_player(game,user.owner) && !player_state(game,user.owner).online) || user.inventory.held()->kind!=ItemKind::Grapple || user.health<=0 || user.stun_ticks || user.sleep_ticks)
        release_grapple(game,user,false,user.facing);
    if (state.prop_ticks<=0) return;
    if (--state.prop_ticks%6!=0) return;
    const Cell next=state.prop_cell+state.prop_direction;
    const Tile* tile=game.stage.at(next);
    const int victim=entity_at(game,next,true);
    if (victim>=0 && victim!=slot) {
        damage_entity(game,victim,24+(has_artifact(user,ArtifactKind::Iron) ? 12 : 0),user.cell,true,{slot,user.generation});
        emit_sound(game,SoundId::TossImpact,next);land_prop(game,user);return;
    }
    if (!tile || (!walkable(*tile) && !open_drop(tile->kind))) {
        hit_prop(game,next,24,user.cell);hit_terrain(game,next,user.cell,24,0);land_prop(game,user);return;
    }
    state.prop_cell=next;
    if (!state.prop_ticks) {emit_sound(game,SoundId::TossLand,next);land_prop(game,user);}
}
