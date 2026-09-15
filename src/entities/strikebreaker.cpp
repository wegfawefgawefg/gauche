#include "strikebreaker.hpp"
#include "mine_crew.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../combat/shove.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

// SLOTS: a-handle protected foreman, b-handle attacker; attack_wait hostility.
// label_a phase, timer_a phase clock, point_a origin, point_b fixed impact cell.
// counter_a/b home coordinates, counter_c finite shield HP. c slots are hearing.
namespace {
void appearance(Entity& guard) {
    guard.sprite=guard.label_a==BreakerPush ? Sprite::BreakerPush :
        guard.label_a==BreakerHammer || guard.label_a==BreakerCut ? Sprite::BreakerRaise :
        guard.label_a==BreakerRecover || guard.freeze_ticks>0 ? Sprite::BreakerRecover :
        guard.counter_c>0 ? Sprite::Strikebreaker : Sprite::BreakerUnshielded;
}
void recover(Entity& guard,int ticks) {
    guard.label_a=BreakerRecover; guard.timer_a=ticks; appearance(guard);
}
void begin(Game& game,Entity& guard,Cell target,BreakerPhase phase) {
    guard.point_a=guard.cell; guard.point_b=target;
    guard.facing=cardinal_toward(guard.cell,target,guard.facing);
    guard.label_a=phase; guard.timer_a=phase==BreakerPush ? 18 : 42;
    appearance(guard);
    emit_sound(game,phase==BreakerPush ? SoundId::BreakerBrace : SoundId::BreakerHeave,guard.cell);
}
void advance(Game& game,int slot,Cell target) {
    auto& guard=game.entities[static_cast<std::size_t>(slot)];
    if (guard.move_wait>0 || guard.vitals.rooted>0) return;
    const auto next=mine_route_step(game,slot,target);
    if (!next) { guard.move_wait=18; return; }
    const auto& tile=game.stage.at_or_border(*next);
    if (crew_diggable(tile) || prop_blocks(tile.prop)) begin(game,guard,*next,BreakerCut);
    else willing_step(game,slot,*next);
}
}

void init_strikebreaker(Entity& guard) {
    guard.health=guard.max_health=150; guard.move_interval=20; guard.impassable=true;
    guard.counter_a=guard.cell.x; guard.counter_b=guard.cell.y;
    guard.counter_c=breaker_shield_health; appearance(guard);
}
bool breaker_blocks(const Entity& guard,Cell source) {
    return guard.kind==EntityKind::Strikebreaker && guard.health>0 && guard.counter_c>0 &&
        guard.label_a==BreakerMarch && guard.sleep_ticks==0 && guard.stun_ticks==0 &&
        guard.freeze_ticks==0 && guard.vitals.rooted==0 && source!=guard.cell &&
        cardinal_toward(guard.cell,source,guard.facing)==guard.facing;
}
void interrupt_strikebreaker(Entity& guard) {
    if (guard.kind!=EntityKind::Strikebreaker) return;
    if (guard.label_a==BreakerPush || guard.label_a==BreakerHammer || guard.label_a==BreakerCut)
        recover(guard,36);
}
void alert_strikebreakers(Game& game,int victim_slot,Cell source) {
    const auto& victim=game.entities[static_cast<std::size_t>(victim_slot)];
    if (!mine_worker(victim.kind) && victim.kind!=EntityKind::Strikebreaker) return;
    const int attacker=entity_at(game,source,true);
    if (attacker<0 || attacker==victim_slot) return;
    const auto& culprit=game.entities[static_cast<std::size_t>(attacker)];
    if ((mine_worker(culprit.kind) || culprit.kind==EntityKind::Strikebreaker) &&
        culprit.entity_a.slot>=0 && culprit.entity_a==victim.entity_a) return;
    for (int slot=0;slot<max_entities;++slot) {
        auto& guard=game.entities[static_cast<std::size_t>(slot)];
        if (guard.kind!=EntityKind::Strikebreaker || guard.health<=0) continue;
        if (slot!=victim_slot && (guard.entity_a.slot<0 || guard.entity_a!=victim.entity_a ||
            distance(guard.cell,victim.cell)>10 || !clear_attack_sight(game,guard.cell,victim.cell))) continue;
        if (guard.attack_wait==0) emit_sound(game,SoundId::BreakerAlarm,guard.cell);
        guard.entity_b={attacker,culprit.generation}; guard.attack_wait=600;
    }
}
void hit_breaker_shield(Game& game,int slot,int damage,Cell source) {
    auto& guard=game.entities[static_cast<std::size_t>(slot)];
    guard.counter_c=std::max(0,guard.counter_c-damage);
    emit_sound(game,guard.counter_c==0 ? SoundId::BreakerShieldBreak : SoundId::BreakerBlock,guard.cell);
    alert_strikebreakers(game,slot,source);
    if (guard.counter_c==0) recover(guard,60);
}
void step_strikebreaker(Game& game,int slot) {
    auto& guard=game.entities[static_cast<std::size_t>(slot)];
    if (guard.cell!=guard.point_a || guard.freeze_ticks>0 || guard.vitals.rooted>0)
        interrupt_strikebreaker(guard);
    appearance(guard);
    if (guard.label_a==BreakerRecover) {
        if (guard.timer_a==0) { guard.label_a=BreakerMarch; appearance(guard); }
        return;
    }
    if (guard.label_a!=BreakerMarch) {
        if (guard.timer_a>0) return;
        const Cell target=guard.point_b;
        if (!clear_attack_sight(game,guard.cell,target,false)) { recover(guard,36); return; }
        if (guard.label_a==BreakerPush) {
            const int victim=entity_at(game,target,true);
            Cell impact=target;
            if (victim>=0 && victim!=slot && !blocks_facing(game.entities[static_cast<std::size_t>(victim)],guard.cell)) {
                shove_actor(game,victim,guard.facing,guard.cell);
                // A spring/teleport triggered by the shove must not drag the
                // hammer's target across the room. Follow only the one-cell push.
                impact=game.entities[static_cast<std::size_t>(victim)].cell==target ? target : target+guard.facing;
            }
            emit_sound(game,SoundId::BreakerShove,target);
            begin(game,guard,impact,BreakerHammer);
        } else {
            hit_prop(game,target,64,guard.cell);
            hit_terrain(game,target,guard.cell,32,1);
            const int victim=entity_at(game,target,true);
            if (victim>=0 && victim!=slot) damage_entity(game,victim,32,guard.cell);
            emit_sound(game,SoundId::BreakerSlam,target);
            recover(guard,54);
        }
        return;
    }
    auto* threat=get_entity(game,guard.entity_b);
    if (guard.attack_wait>0 && threat && threat->health>0) {
        if (distance(guard.cell,threat->cell)==1 && clear_attack_sight(game,guard.cell,threat->cell))
            begin(game,guard,threat->cell,BreakerPush);
        else advance(game,slot,threat->cell);
        return;
    }
    guard.entity_b={}; guard.attack_wait=0;
    const auto* leader=get_entity(game,guard.entity_a);
    const Cell home=leader && leader->health>0 && leader->kind==EntityKind::ShiftForeman ?
        leader->cell : Cell{guard.counter_a,guard.counter_b};
    if (distance(guard.cell,home)>3) advance(game,slot,home);
}
bool valid_strikebreaker(const Entity& guard) {
    if (guard.kind!=EntityKind::Strikebreaker) return true;
    const bool committed=guard.label_a==BreakerPush || guard.label_a==BreakerHammer || guard.label_a==BreakerCut;
    const Cell delta=guard.point_b-guard.point_a;
    if (committed && (distance({},delta)<1 || distance({},delta)>(guard.label_a==BreakerHammer ? 2 : 1) ||
        (delta.x!=0 && delta.y!=0))) return false;
    return
        (guard.label_a>=BreakerMarch && guard.label_a<=BreakerRecover &&
         guard.counter_c>=0 && guard.counter_c<=breaker_shield_health && guard.timer_a>=0 && guard.timer_a<=60);
}
