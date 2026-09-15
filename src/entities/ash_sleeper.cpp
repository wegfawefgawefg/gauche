#include "ash_sleeper.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
void settle(Entity& actor) {
    actor.label_a=AshSettling;actor.timer_a=30;actor.counter_a=0;
    actor.sprite=Sprite::AshWhirl;actor.point_a=actor.cell;
}
}
// SLOTS: label_a phase, timer_a rise/swipe/recovery/settling; timer_b dampness.
// counter_a remaining agitation; point_a committed origin, point_b fixed strike.
// point_c/label_c/timer_c retain the shared audible destination. No new state.
void init_ash_sleeper(Entity& actor) {
    actor.health=actor.max_health=44;actor.move_interval=18;actor.impassable=true;
    actor.sprite=Sprite::AshMound;
}
void rouse_ash_sleeper(Game& game,Entity& actor,Cell noise) {
    if (actor.kind!=EntityKind::AshSleeper || actor.health<=0 || actor.timer_b>0 || actor.freeze_ticks>0 ||
        actor.sleep_ticks || actor.stun_ticks || surface_wet(game.stage.at_or_border(actor.cell))) return;
    actor.counter_a=360;actor.point_c=noise;actor.label_c=InvestigateNoise;actor.timer_c=300;
    if (actor.label_a==AshDormant || actor.label_a==AshSettling) {
        actor.label_a=AshRising;actor.timer_a=30;actor.point_a=actor.cell;actor.sprite=Sprite::AshMound;
        emit_sound(game,SoundId::AshRise,actor.cell);
    }
}
void hurt_ash_sleeper(Game& game,Entity& actor,Cell attacker) {
    rouse_ash_sleeper(game,actor,attacker);
    if (actor.kind==EntityKind::AshSleeper && actor.label_a==AshSwipe) {actor.label_a=AshRecover;actor.timer_a=24;}
}
void hear_ash_sleepers(Game& game,Cell origin,int radius) {
    if (radius<7) return;
    const auto candidate=[&](const Entity& actor) {return actor.kind==EntityKind::AshSleeper && actor.health>0 && distance(actor.cell,origin)<=radius;};
    if (std::none_of(game.entities.begin(),game.entities.end(),candidate)) return;
    const auto heard=audible_cells(game,origin,radius);
    for (Entity& actor:game.entities) if (candidate(actor) && std::find(heard.begin(),heard.end(),actor.cell)!=heard.end())
        rouse_ash_sleeper(game,actor,origin);
}
bool damp_ash_sleeper(Entity& actor) {
    if (actor.kind!=EntityKind::AshSleeper || actor.health<=0) return false;
    const bool changed=actor.label_a!=AshDormant && actor.label_a!=AshSettling;
    if (changed) settle(actor);
    actor.counter_a=0;actor.timer_b=120;
    return changed;
}
void interrupt_ash_sleeper(Entity& actor) {
    if (actor.kind!=EntityKind::AshSleeper || actor.health<=0) return;
    if (actor.label_a==AshRising || actor.label_a==AshSwipe || actor.label_a==AshRoam || actor.label_a==AshRecover)
        settle(actor);
}
void step_ash_sleeper(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.freeze_ticks || surface_wet(game.stage.at_or_border(actor.cell))) {
        if (damp_ash_sleeper(actor)) emit_sound(game,SoundId::AshSettle,actor.cell);
    }
    if (actor.counter_a>0) --actor.counter_a;
    if (actor.label_a==AshDormant) return;
    if (actor.label_a==AshSettling) {
        if (!actor.timer_a) {actor.label_a=AshDormant;actor.sprite=Sprite::AshMound;}
        return;
    }
    if (!actor.counter_a) {settle(actor);emit_sound(game,SoundId::AshSettle,actor.cell);return;}
    if (actor.label_a==AshRising) {
        if (actor.cell!=actor.point_a) {actor.point_a=actor.cell;actor.timer_a=30;}
        if (!actor.timer_a) {actor.label_a=AshRoam;actor.sprite=Sprite::AshWhirl;actor.move_wait=18;}
        return;
    }
    if (actor.label_a==AshSwipe) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted) {actor.label_a=AshRecover;actor.timer_a=24;return;}
        if (actor.timer_a) return;
        const Cell target=actor.point_a+actor.point_b;
        actor.label_a=AshRecover;actor.timer_a=30;
        if (clear_attack_sight(game,actor.cell,target,false)) {
            const int victim=entity_at(game,target,true);
            if (victim>=0 && victim!=slot) damage_entity(game,victim,8,actor.cell,true,{slot,actor.generation});
        }
        emit_sound(game,SoundId::AshSwipe,target);return;
    }
    if (actor.label_a==AshRecover) {if (!actor.timer_a) actor.label_a=AshRoam;return;}
    const auto target=enemy_target(game,actor.cell,7);
    if (target && distance(actor.cell,target->cell)==1) {
        actor.point_a=actor.cell;actor.point_b=target->cell-actor.cell;actor.facing=actor.point_b;
        actor.label_a=AshSwipe;actor.timer_a=30;emit_sound(game,SoundId::AshInhale,actor.cell);return;
    }
    if (target && clear_attack_sight(game,actor.cell,target->cell)) approach(game,slot,target->cell);
    else if (!step_hearing(game,slot)) wander(game,slot);
}
bool valid_ash_sleeper(const Entity& actor) {
    if (actor.kind!=EntityKind::AshSleeper) return true;
    return actor.label_a>=AshDormant && actor.label_a<=AshSettling && actor.timer_a<=30 && actor.timer_b<=120 &&
        actor.counter_a>=0 && actor.counter_a<=360 && (actor.label_a!=AshSwipe || distance({},actor.point_b)==1);
}
