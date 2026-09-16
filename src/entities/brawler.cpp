#include "brawler.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include <algorithm>

namespace {
struct BrawlerSpec {
    int health,move,windup,recovery,damage;
    Sprite idle,raised,swing;
    SoundId warn,hit;
};
BrawlerSpec spec(EntityKind kind) {
    if (kind==EntityKind::FrostGoblin)
        return {28,16,24,30,10,Sprite::FrostGoblin,Sprite::FrostGoblinRaise,Sprite::FrostGoblinSwing,
            SoundId::FrostGoblinWarn,SoundId::FrostGoblinSwing};
    return {44,22,30,42,14,Sprite::PipeGuard,Sprite::PipeGuardRaise,Sprite::PipeGuardSwing,
        SoundId::PipeGuardWarn,SoundId::PipeGuardSwing};
}
void recover(Entity& actor) {
    actor.label_a=BrawlerRecover;actor.timer_a=spec(actor.kind).recovery;
    actor.sprite=spec(actor.kind).idle;
}
}
// SLOTS: label_a hunt/windup/recover, timer_a phase, timer_b idle wander beat;
// point_a committed origin, point_b committed strike cell. c-slots stay hearing-owned.
// No elemental gimmick, armor puzzle or instant contact damage: just chase/swing/rest.
void init_brawler(Entity& actor) {
    const auto s=spec(actor.kind);
    actor.health=actor.max_health=s.health;actor.move_interval=s.move;
    actor.impassable=true;actor.sprite=s.idle;
}
void interrupt_brawler(Entity& actor) {
    if (brawler_kind(actor.kind) && actor.label_a==BrawlerWindup) recover(actor);
}
void step_brawler(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const auto s=spec(actor.kind);
    if (actor.label_a==BrawlerWindup) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) {recover(actor);return;}
        if (actor.timer_a>0) return;
        resolve_enemy_attack(game,slot,s.damage,s.hit);
        recover(actor);actor.sprite=s.swing;return;
    }
    if (actor.label_a==BrawlerRecover) {
        if (actor.timer_a<s.recovery-6) actor.sprite=s.idle;
        if (actor.timer_a==0) actor.label_a=BrawlerHunt;
        return;
    }
    const auto target=enemy_target(game,actor.cell,7);
    if (!target || !clear_attack_sight(game,actor.cell,target->cell)) {
        if (step_hearing(game,slot)) return;
        if (actor.timer_b==0) {wander(game,slot);actor.timer_b=60;}
        return;
    }
    if (distance(actor.cell,target->cell)==1) {
        actor.point_a=actor.cell;actor.point_b=target->cell;
        actor.facing=target->cell-actor.cell;
        actor.label_a=BrawlerWindup;actor.timer_a=s.windup;actor.sprite=s.raised;
        emit_sound(game,s.warn,actor.cell);return;
    }
    pursue(game,slot,target->cell);
}
bool valid_brawler(const Entity& actor) {
    if (!brawler_kind(actor.kind)) return true;
    const auto s=spec(actor.kind);
    return actor.label_a>=BrawlerHunt && actor.label_a<=BrawlerRecover &&
        actor.timer_a>=0 && actor.timer_a<=std::max(s.windup,s.recovery) &&
        actor.timer_b>=0 && actor.timer_b<=60 &&
        (actor.label_a!=BrawlerWindup || distance(actor.point_a,actor.point_b)==1);
}
