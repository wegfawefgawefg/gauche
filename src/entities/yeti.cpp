#include "yeti.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../combat/shove.hpp"
#include "../combat/toss.hpp"
#include "../props/interaction.hpp"

namespace {
// SLOTS: point_a territory; point_b committed adjacent grab; counter_a/b start
// position; label_a phase; timer_a phase; timer_b idle pause. c-slots are hearing.
void rest(Entity& actor) { actor.label_a=YetiRecover; actor.timer_a=72; }
void grab(Game& game,int slot) {
    Entity& yeti=game.entities[static_cast<std::size_t>(slot)];
    const int target=entity_at(game,yeti.point_b,true);
    if (target>=0) {
        Entity& victim=game.entities[static_cast<std::size_t>(target)];
        const bool guarded=blocks_facing(victim,yeti.cell);
        damage_entity(game,target,2,yeti.cell,true,{slot,yeti.generation});
        if (!guarded && yeti.health>0 && toss_actor(game,target,yeti.facing,yeti.cell,{slot,yeti.generation}))
            emit_sound(game,SoundId::YetiThrow,yeti.cell);
        else emit_sound(game,SoundId::YetiMiss,yeti.cell);
    } else {
        hit_prop(game,yeti.point_b,8,yeti.cell);
        emit_sound(game,SoundId::YetiMiss,yeti.cell);
    }
    rest(yeti);
}
}
void init_yeti(Entity& actor) {
    actor.health=actor.max_health=112;
    actor.move_interval=42;
    actor.impassable=true;
    actor.sprite=Sprite::Yeti;
    actor.point_a=actor.cell;
}
void interrupt_yeti(Entity& actor) {
    if (actor.kind==EntityKind::Yeti && actor.label_a==YetiGrab) rest(actor);
}
void step_yeti(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.label_a==YetiRecover) {
        if (actor.timer_a==0) actor.label_a=YetiIdle;
        return;
    }
    if (actor.label_a==YetiGrab) {
        if (actor.cell!=Cell{actor.counter_a,actor.counter_b} || actor.vitals.rooted>0) { rest(actor); return; }
        if (actor.timer_a==0) grab(game,slot);
        return;
    }
    const auto target=enemy_target(game,actor.cell,1);
    if (target && distance(actor.cell,target->cell)==1 && clear_attack_sight(game,actor.cell,target->cell)) {
        actor.point_b=target->cell;
        actor.facing=target->cell-actor.cell;
        actor.counter_a=actor.cell.x; actor.counter_b=actor.cell.y;
        actor.label_a=YetiGrab; actor.timer_a=36;
        emit_sound(game,SoundId::YetiWarn,actor.cell);
        return;
    }
    if (actor.move_wait>0 || actor.timer_b>0) return;
    actor.timer_b=75+static_cast<int>(random_u32(game)%90);
    constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
    const auto start=random_u32(game)%4;
    for (unsigned int i=0;i<4;++i) {
        const Cell next=actor.cell+sides[(start+i)%4];
        const int gap=distance(next,actor.point_a);
        if (gap>2 && gap>=distance(actor.cell,actor.point_a)) continue;
        // Shuffle in the territory, never route after a fleeing player. Check
        // all directions before resting so a blocked corner cannot trap the RNG.
        if (move_entity(game,slot,next,false)) break;
    }
}
bool valid_yeti(const Entity& actor) {
    return actor.kind!=EntityKind::Yeti || (actor.label_a>=YetiIdle && actor.label_a<=YetiRecover &&
        (actor.label_a!=YetiGrab || distance(Cell{actor.counter_a,actor.counter_b},actor.point_b)==1));
}
