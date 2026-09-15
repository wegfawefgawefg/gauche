#include "ember.hpp"
#include "behavior.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../projectiles/coal_spit.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

// SLOTS: label_a phase, timer_a phase clock; counter_a reserve coal (0..5),
// counter_b loaded coal (0/1), label_b damp next shot. point_a windup origin,
// point_b committed lane/strike cell. c slots remain ordinary hearing memory.
namespace {
void rest(Entity& actor,int ticks) {
    actor.label_a=StokerRest; actor.timer_a=ticks; actor.sprite=Sprite::Stoker;
}
void light(Entity& actor) {
    actor.light=stoker_hot(actor) ? LightEmitter{4,700,{255,130,55}} : LightEmitter{2,180,{177,122,79}};
    actor.self_light={};
}
}
void init_ember(Entity& actor) {
    actor.sprite=Sprite::Stoker;
    actor.health=actor.max_health=48; actor.move_interval=20;
    actor.impassable=true; actor.counter_a=5; actor.counter_b=1;
    light(actor);
}
bool stoker_hot(const Entity& actor) {
    return actor.kind==EntityKind::Ember && actor.health>0 && actor.counter_b>0 && actor.label_b==0;
}
void interrupt_stoker(Entity& actor) {
    if (actor.kind!=EntityKind::Ember) return;
    if (actor.label_a==StokerPack || actor.label_a==StokerScoop || actor.label_a==StokerStrike) rest(actor,24);
}
bool damp_stoker(Entity& actor) {
    if (actor.kind!=EntityKind::Ember || actor.health<=0) return false;
    const bool changed=actor.label_b==0;
    actor.label_b=1; light(actor);
    return changed;
}
int stoker_at(const Game& game,Cell cell) {
    for (int slot=0;slot<max_entities;++slot) {
        const auto& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::Ember && actor.health>0 && actor.cell==cell) return slot;
    }
    return -1;
}
bool feed_stoker(Game& game,Cell cell) {
    const int slot=stoker_at(game,cell);
    if (slot<0) return false;
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.counter_a>=5) return false;
    ++actor.counter_a; emit_sound(game,SoundId::StokerScoop,cell);
    return true;
}
void step_ember(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (surface_wet(game.stage.at_or_border(actor.cell))) damp_stoker(actor);
    light(actor);
    if (actor.label_a==StokerRest) {
        if (actor.timer_a==0) { actor.label_a=StokerReady; actor.sprite=Sprite::Stoker; }
        return;
    }
    if (actor.label_a!=StokerReady) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) { interrupt_stoker(actor); return; }
        if (actor.timer_a>0) return;
        if (actor.label_a==StokerScoop) {
            if (actor.counter_a>0 && actor.counter_b==0) { --actor.counter_a; actor.counter_b=1; }
            rest(actor,18);
        } else if (actor.label_a==StokerPack) {
            if (launch_coal_spit(game,slot,actor.point_b,actor.label_b==0)) {
                actor.counter_b=0; actor.label_b=0;
                emit_sound(game,SoundId::StokerSpit,actor.cell); make_noise(game,actor.cell,7);
            }
            rest(actor,24);
        } else {
            resolve_enemy_attack(game,slot,14,SoundId::StokerSwing);
            rest(actor,36);
        }
        light(actor); return;
    }
    if (actor.counter_b==0 && actor.counter_a>0) {
        actor.label_a=StokerScoop; actor.timer_a=60; actor.point_a=actor.cell;
        actor.sprite=Sprite::StokerScoop; emit_sound(game,SoundId::StokerScoop,actor.cell); return;
    }
    const auto target=enemy_target(game,actor.cell,8);
    if (!target) { if (!step_hearing(game,slot)) wander(game,slot); return; }
    const Cell delta=target->cell-actor.cell;
    if (actor.counter_b>0 && (delta.x==0 || delta.y==0) && clear_attack_sight(game,actor.cell,target->cell)) {
        actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.point_a=actor.cell; actor.point_b=actor.facing;
        actor.label_a=StokerPack; actor.timer_a=30; actor.sprite=Sprite::StokerPack;
        emit_sound(game,SoundId::StokerCough,actor.cell); return;
    }
    if (distance(actor.cell,target->cell)==1) {
        actor.point_a=actor.cell; actor.point_b=target->cell;
        actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.label_a=StokerStrike; actor.timer_a=30; actor.sprite=Sprite::StokerSwing;
        emit_sound(game,SoundId::StokerHeave,actor.cell); return;
    }
    approach(game,slot,target->cell);
}
