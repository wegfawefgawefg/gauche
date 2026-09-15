#include "powder_monkey.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../projectiles/exposed_fuse.hpp"
#include "../items/quarry_charge.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

// SLOTS: counter_a unarmed charges (2); entity_a actual lit charge handle.
// label_a phase; timer_a phase duration, timer_b site search cooldown.
// point_a lighting origin / committed melee origin; point_b fixed melee cell.
// c slots remain ordinary hearing. No separate permanent bomb inventory.
namespace {
void rest(Entity& actor,int ticks) { actor.label_a=PowderRest; actor.timer_a=ticks; actor.sprite=Sprite::PowderMonkey; }
void escape(Game& game,int slot,const Entity& shot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    actor.sprite=Sprite::PowderRun;
    if (actor.move_wait>0) return;
    if (const auto next=powder_escape(game,slot,shot.cell,shot.facing)) {
        if (*next!=actor.cell) willing_step(game,slot,*next);
        else actor.move_wait=6;
    } else actor.move_wait=6;
}
}
void init_powder_monkey(Entity& actor) {
    actor.health=actor.max_health=42; actor.move_interval=6; actor.impassable=true;
    actor.counter_a=2; actor.sprite=Sprite::PowderMonkey;
    actor.light={2,220,{207,174,102}};
}
void interrupt_powder_monkey(Entity& actor) {
    if (actor.kind!=EntityKind::PowderMonkey) return;
    if (actor.label_a==PowderLight) { actor.label_a=PowderRun; actor.sprite=Sprite::PowderRun; }
    else if (actor.label_a==PowderStrike) rest(actor,24);
}
void step_powder_monkey(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    const Entity* shot=get_entity(game,actor.entity_a);
    const bool armed=shot && exposed_fuse(*shot);
    if (actor.label_a==PowderLight) {
        if (!armed) { actor.entity_a={}; rest(actor,45); return; }
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) interrupt_powder_monkey(actor);
        else if (actor.timer_a>0) return;
        else { actor.label_a=PowderRun; actor.sprite=Sprite::PowderRun; emit_sound(game,SoundId::PowderPanic,actor.cell); }
    }
    // A disarmed/reused slot never becomes a fresh danger via a stale handle.
    if (armed) { escape(game,slot,*shot); return; }
    actor.entity_a={};
    if (actor.label_a==PowderRun) { rest(actor,60); return; }
    if (actor.label_a==PowderRest) { if (actor.timer_a==0) actor.label_a=PowderReady; return; }
    if (actor.label_a==PowderStrike) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) { rest(actor,24); return; }
        if (actor.timer_a>0) return;
        resolve_enemy_attack(game,slot,6,SoundId::PowderScratch); rest(actor,30); return;
    }
    const auto target=enemy_target(game,actor.cell,10);
    if (!target) { if (!step_hearing(game,slot)) wander(game,slot); return; }
    if (actor.counter_a>0 && actor.timer_b==0 && actor.move_wait==0) {
        const auto site=powder_site(game,slot,target->cell);
        actor.timer_b=12;
        if (site) {
            if (site->cell!=actor.cell) { willing_step(game,slot,site->first); return; }
            const Handle charge=arm_quarry_charge(game,slot,make_item(ItemKind::QuarryCharge),site->direction,120);
            if (get_entity(game,charge)) {
                --actor.counter_a; actor.entity_a=charge; actor.point_a=actor.cell;
                actor.facing=site->direction; actor.label_a=PowderLight; actor.timer_a=24;
                actor.sprite=Sprite::PowderLight; actor.move_wait=0;
                emit_sound(game,SoundId::PowderCork,actor.cell); make_noise(game,actor.cell,5);
            }
            return;
        }
    }
    if (distance(actor.cell,target->cell)==1) {
        actor.point_a=actor.cell; actor.point_b=target->cell;
        actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.label_a=PowderStrike; actor.timer_a=24; actor.sprite=Sprite::PowderScratch;
        emit_sound(game,SoundId::PowderWarn,actor.cell); return;
    }
    if (actor.counter_a==0) flee(game,slot,target->cell);
}
