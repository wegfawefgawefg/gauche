#include "magnet_crane.hpp"
#include "crane_operator.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../items/magnet.hpp"
#include "../items/sled.hpp"
#include "../combat/shove.hpp"
#include "../world/floating_items.hpp"
#include "../projectiles/projectile.hpp"
#include <algorithm>

namespace {
int metal_priority(const Game& game,const Entity& target) {
    if (target.kind==EntityKind::GroundItem) {
        const Item& item=target.ground_item;
        if (item.count<=0 || item.flight.slot>=0 || sled_cargo(game,target) || !magnetic_item(item.kind)) return -1;
        return item.kind==ItemKind::HorseshoeMagnet ? 0 : 2;
    }
    if (target.health<=0 || target.hard_blocker || target.toss.ticks>0) return -1;
    const Item& held=*target.inventory.held();
    if (target.kind==EntityKind::Player && held.count>0 && magnetic_item(held.kind))
        return held.kind==ItemKind::HorseshoeMagnet ? 0 : 1;
    // These workers actually carry iron tools/shields in their ordinary poses.
    if (target.kind==EntityKind::Pickhand || target.kind==EntityKind::RivetGunner ||
        target.kind==EntityKind::Strikebreaker) return 1;
    return -1;
}
bool blocked_cargo(const Game& game,const Entity& crane,const Entity& target) {
    if (target.kind!=EntityKind::GroundItem) return false;
    const Cell next=target.cell+cardinal_toward(target.cell,crane.cell,{1,0});
    if (projectile_blocked(game,next) || entity_at(game,next,true)>=0) return true;
    for (const Entity& other:game.entities)
        if (&other!=&target && other.kind==EntityKind::GroundItem && other.cell==next) return true;
    return false;
}
Handle choose_target(const Game& game,const Entity& crane) {
    Handle best{};int best_score=100;
    for (int i=0;i<max_entities;++i) {
        const Entity& target=game.entities[static_cast<std::size_t>(i)];
        const int priority=metal_priority(game,target),range=distance(crane.cell,target.cell);
        if (priority<0 || range<1 || range>crane_reach) continue;
        if (crane.label_b==1 && blocked_cargo(game,crane,target)) continue;
        const int score=priority*16+range;
        if (score>=best_score || !clear_shot_sight(game,crane.cell,target.cell)) continue;
        best={i,target.generation};best_score=score;
    }
    return best;
}
void return_head(Entity& crane) {
    crane.counter_a=crane.label_a==CraneTravel ? std::clamp(30-crane.timer_a,0,30) : 30;
    crane.label_a=CraneReturn;crane.timer_a=60;crane.entity_a={};
}
void grab(Game& game,Entity& crane) {
    Entity* target=get_entity(game,crane.entity_a);
    if (!target || target->cell!=crane.point_b || metal_priority(game,*target)<0 ||
        !clear_shot_sight(game,crane.cell,target->cell)) return;
    const Cell source=target->cell,direction=cardinal_toward(source,crane.cell,{1,0});
    const Cell next=source+direction;
    bool moved=false;
    if (target->kind==EntityKind::GroundItem) {
        if (projectile_blocked(game,next) || entity_at(game,next,true)>=0) return;
        for (const Entity& other:game.entities)
            if (&other!=target && other.kind==EntityKind::GroundItem && other.cell==next) return;
        stop_item_float(game,*target);target->cell=next;moved=true;
    } else {
        if (target->vitals.grip>0 || target->vitals.rooted>0 || ridden_sled(game,*target)) return;
        // A magnetic tug cannot use shove_actor's instant wall-crush rule.
        // Cover or the anchored pedestal stops it with a small pinch instead.
        const Tile* tile=game.stage.at(next);
        if (tile && walkable(*tile) && entity_at(game,next,true)<0)
            moved=shove_actor(game,crane.entity_a.slot,direction,crane.cell);
        damage_entity(game,crane.entity_a.slot,6,crane.cell,false);
    }
    if (moved && game.shot_count<static_cast<int>(game.shots.size())) {
        auto& event=game.shots[static_cast<std::size_t>(game.shot_count++)];
        event={};event.source=source;event.end=next;event.magnetic=true;
    }
    emit_sound(game,SoundId::CraneTug,source);
}
}
// SLOTS: label_a phase; timer_a phase ticks; entity_a generation-checked marked
// target; point_a anchored base, point_b committed contact; counter_a retract
// start fraction in thirtieths (presentation only). label_b staffed latch,
// entity_b operator. No inventory is detached.
void init_magnet_crane(Entity& crane) {
    crane.health=crane.max_health=90;crane.impassable=crane.hard_blocker=true;
    crane.sprite=Sprite::MagnetCrane;crane.point_a=crane.point_b=crane.cell;
}
void interrupt_magnet_crane(Entity& crane) {
    if (crane.kind==EntityKind::MagnetCrane && (crane.label_a==CraneTravel || crane.label_a==CraneLock)) return_head(crane);
}
void step_magnet_crane(Game& game,int slot) {
    Entity& crane=game.entities[static_cast<std::size_t>(slot)];
    if (crane.cell!=crane.point_a) {
        crane.point_a=crane.point_b=crane.cell;crane.label_a=CraneReturn;
        crane.timer_a=60;crane.counter_a=0;crane.entity_a={};return;
    }
    if (crane.freeze_ticks>0) {interrupt_magnet_crane(crane);return;}
    if (!crane_operator_ready(game,crane)) {
        interrupt_magnet_crane(crane);
        if (crane.label_a!=CraneReturn) {crane.label_a=CraneIdle;crane.timer_a=0;return;}
    }
    if (crane.timer_a>0) return;
    switch (crane.label_a) {
    case CraneIdle: {
        const Handle target=choose_target(game,crane);
        const Entity* actor=get_entity(game,target);
        if (!actor) {crane.timer_a=18;return;}
        crane.entity_a=target;crane.point_b=actor->cell;crane.counter_a=0;
        crane.label_a=CraneTravel;crane.timer_a=30;
        emit_sound(game,SoundId::CraneSlew,crane.cell);break;
    }
    case CraneTravel:
        crane.label_a=CraneLock;crane.timer_a=45;
        emit_sound(game,SoundId::CraneCoil,crane.point_b);break;
    case CraneLock: grab(game,crane);return_head(crane);break;
    case CraneReturn:
        crane.label_a=CraneIdle;crane.point_b=crane.cell;crane.counter_a=0;break;
    }
}
bool valid_magnet_crane(const Entity& crane) {
    if (crane.kind!=EntityKind::MagnetCrane) return true;
    if (crane.label_b<0 || crane.label_b>1) return false;
    if (crane.label_a<CraneIdle || crane.label_a>CraneReturn || crane.timer_a<0 || crane.timer_a>60 ||
        crane.counter_a<0 || crane.counter_a>30 || distance(crane.point_a,crane.point_b)>crane_reach) return false;
    if (crane.label_a==CraneTravel && crane.timer_a>30) return false;
    return crane.label_a!=CraneLock || crane.timer_a<=45;
}
