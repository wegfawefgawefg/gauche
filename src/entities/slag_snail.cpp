#include "slag_snail.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../surfaces/slag.hpp"
#include "../surfaces/interaction.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
void appearance(Entity& actor) {
    actor.sprite=actor.label_a==SlagTuck ? Sprite::SlagSnailTucked :
        actor.label_a==SlagCooled ? Sprite::SlagSnailCooled : Sprite::SlagSnail;
    actor.light=actor.label_a==SlagCooled ? LightEmitter{} : LightEmitter{2,160,{229,125,61}};
}
void recover(Entity& actor) {
    actor.label_a=SlagRecover;actor.timer_a=72;actor.counter_a=0;
    actor.move_interval=40;appearance(actor);
}
void lunge(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const Handle self{slot,actor.generation};
    const Cell origin=actor.cell,next=origin+actor.point_b;
    const Tile* tile=game.stage.at(next);
    if (!tile || !walkable(*tile)) {
        hit_prop(game,next,18,origin);recover(actor);emit_sound(game,SoundId::SlagImpact,origin);return;
    }
    const int victim=entity_at(game,next,true);
    if (victim>=0) {
        recover(actor);
        damage_entity(game,victim,18,origin,true,self);
        emit_sound(game,SoundId::SlagImpact,next);return;
    }
    if (!move_entity(game,slot,next,false)) {recover(actor);return;}
    if (!get_entity(game,self) || actor.health<=0 || actor.label_a!=SlagLunge) return;
    leave_slag(game,origin);
    if (actor.cell!=next) {recover(actor);return;}
    actor.point_a=next;
    if (--actor.counter_a==0) recover(actor);
    else actor.timer_a=8;
}
}
// SLOTS: label_a phase, timer_a tell/recovery, counter_a lunge cells remaining.
// point_a expected position, point_b committed cardinal heading; c slots hearing.
void init_slag_snail(Entity& actor) {
    actor.health=actor.max_health=96;actor.move_interval=40;actor.impassable=true;
    appearance(actor);
}
bool slag_snail_shelled(const Entity& actor) {
    return actor.kind==EntityKind::SlagSnail && actor.freeze_ticks==0 &&
        (actor.label_a==SlagCrawl || actor.label_a==SlagTuck);
}
bool cool_slag_snail(Entity& actor) {
    if (actor.kind!=EntityKind::SlagSnail || actor.health<=0) return false;
    const bool changed=actor.label_a!=SlagCooled;
    actor.label_a=SlagCooled;actor.timer_a=120;actor.counter_a=0;
    actor.move_interval=40;appearance(actor);return changed;
}
void interrupt_slag_snail(Entity& actor) {
    if (actor.kind==EntityKind::SlagSnail && (actor.label_a==SlagTuck || actor.label_a==SlagLunge)) recover(actor);
}
void step_slag_snail(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.freeze_ticks>0 || surface_wet(game.stage.at_or_border(actor.cell))) {
        if (cool_slag_snail(actor)) emit_sound(game,SoundId::SlagCool,actor.cell);
    }
    if (actor.vitals.rooted>0) interrupt_slag_snail(actor);
    if (actor.label_a==SlagCooled || actor.label_a==SlagRecover) {
        if (actor.timer_a==0) {actor.label_a=SlagCrawl;appearance(actor);}
        return;
    }
    if (actor.label_a==SlagTuck || actor.label_a==SlagLunge) {
        if (actor.cell!=actor.point_a) {recover(actor);return;}
        actor.facing=actor.point_b;
        if (actor.timer_a>0) return;
        if (actor.label_a==SlagTuck) {
            actor.label_a=SlagLunge;actor.counter_a=2;appearance(actor);
            emit_sound(game,SoundId::SlagLunge,actor.cell);
        }
        lunge(game,slot);return;
    }
    if (actor.move_wait>0 || actor.vitals.rooted>0) return;
    const auto target=enemy_target(game,actor.cell,8);
    if (target && distance(actor.cell,target->cell)<=2 && actor.cell!=target->cell &&
        (actor.cell.x==target->cell.x || actor.cell.y==target->cell.y) && clear_attack_sight(game,actor.cell,target->cell)) {
        actor.point_a=actor.cell;actor.point_b=actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.label_a=SlagTuck;actor.timer_a=48;actor.counter_a=2;appearance(actor);
        emit_sound(game,SoundId::SlagTuck,actor.cell);return;
    }
    const Cell previous=actor.cell;const Handle self{slot,actor.generation};
    if (target) pursue(game,slot,target->cell);else if (!step_hearing(game,slot)) wander(game,slot);
    if (!get_entity(game,self) || actor.health<=0) return;
    actor.move_wait=std::max(actor.move_wait,40);
    if (actor.cell!=previous && actor.label_a==SlagCrawl) {
        leave_slag(game,previous);emit_sound(game,SoundId::SlagCrawl,actor.cell);
    }
}
bool valid_slag_snail(const Entity& actor) {
    if (actor.kind!=EntityKind::SlagSnail) return true;
    if (actor.label_a<SlagCrawl || actor.label_a>SlagCooled || actor.timer_a<0 || actor.timer_a>120 ||
        actor.counter_a<0 || actor.counter_a>2) return false;
    if (actor.label_a==SlagTuck || actor.label_a==SlagLunge)
        return actor.counter_a>0 && distance({},actor.point_b)==1;
    return true;
}
