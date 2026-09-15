#include "bolt_pouch.hpp"
#include "../projectiles/projectile.hpp"
#include "../combat/parry.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr RegionalItem pouch{"Bolt Pouch",
    "Five flying bolts fan out to five cells. 6 damage each; all five can hit close up. Passes grates. One handful per use.",
    Sprite::BoltPouch,{1,5,0,6,42,PatternEffect::Damage,true,2,0,false,true},
    ItemAction::Throw,9,6,true,0,0,0,0,0,SoundId::BoltsThrow};
void finish(Game& game,int slot,Cell cell) {
    emit_sound(game,SoundId::BoltsClatter,cell);
    remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
}
}
const RegionalItem* bolt_pouch_item(ItemKind kind) {return kind==ItemKind::BoltPouch ? &pouch : nullptr;}

// Five separate physical bolts; all originate at the hand. Lane expands by at
// most one cell per forward beat. Parried bolts return straight, on a finite clock.
// label_b lane -2..2, counter_a remaining range, counter_b damage, counter_c piercing.
// point_a origin, entity_a owner, ground_item original pouch, attack_interval range.
bool throw_bolts(Game& game,int owner,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(owner)];
    const Item item=*user.inventory.held();
    if (item.kind!=ItemKind::BoltPouch || distance({},direction)!=1) return false;
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    const ItemPattern pattern=item_pattern(item);
    for (int lane=-2;lane<=2;++lane) {
        auto* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,user.cell));
        shot->label_a=static_cast<int>(ProjectileKind::ThrownBolt);shot->label_b=lane;
        shot->counter_a=shot->attack_interval=pattern.maximum;shot->counter_b=pattern.damage;
        shot->counter_c=has_artifact(user,ArtifactKind::AllPiercing) ? 1 : 0;
        shot->point_a=user.cell;shot->entity_a={owner,user.generation};shot->facing=direction;
        shot->ground_item=item;shot->ground_item.count=1;shot->sprite=Sprite::Rivet;
        shot->timer_b=4;shot->timer_a=pattern.maximum*4+1;shot->timer_c=shot->timer_a*4;
    }
    return true;
}
Cell thrown_bolt_next(const Entity& shot) {
    const int step=shot.attack_interval-shot.counter_a+1;
    const int lane=std::clamp(shot.label_b,1-step,step-1);
    const Cell side{-shot.facing.y,shot.facing.x};
    return shot.point_a+Cell{shot.facing.x*step+side.x*lane,shot.facing.y*step+side.y*lane};
}
void step_thrown_bolt(Game& game,int slot) {
    auto& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.timer_a==0 || shot.timer_c==0 || shot.counter_a==0) {finish(game,slot,shot.cell);return;}
    if (shot.timer_b>0) return;
    const Cell next=thrown_bolt_next(shot);
    // A diagonal fan segment cannot cut through the corner of a wall or cover.
    std::array<Cell,3> path{next,next,next};
    if (next.x!=shot.cell.x && next.y!=shot.cell.y)
        path={Cell{next.x,shot.cell.y},Cell{shot.cell.x,next.y},next};
    for (Cell cell:path) if (projectile_blocked(game,cell,false,true)) {
        hit_prop(game,cell,shot.counter_b,shot.cell);
        hit_terrain(game,cell,shot.cell,shot.counter_b,0);
        finish(game,slot,cell);return;
    }
    const Cell source=shot.cell;shot.cell=next;--shot.counter_a;shot.timer_b=4;
    for (int victim=0;victim<max_entities;++victim) {
        const auto& actor=game.entities[static_cast<std::size_t>(victim)];
        if (!actor.impassable || actor.health<=0 || actor.cell!=next || Handle{victim,actor.generation}==shot.entity_a) continue;
        if (parry_ranged_hit(game,victim,shot.facing)) {
            reflect_projectile(shot,actor,victim);shot.label_b=0;return;
        }
        damage_entity(game,victim,shot.counter_b,source,true,shot.entity_a);
        if (!shot.counter_c) {finish(game,slot,next);return;}
    }
    if (!shot.counter_a) finish(game,slot,next);
}
bool valid_thrown_bolt(const Entity& shot) {
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::ThrownBolt)) return true;
    const auto pattern=item_pattern(shot.ground_item);
    return shot.ground_item.kind==ItemKind::BoltPouch && shot.ground_item.count==1 &&
        shot.label_b>=-2 && shot.label_b<=2 && shot.counter_a>=0 && shot.counter_a<=pattern.maximum &&
        shot.attack_interval==pattern.maximum && shot.counter_b==pattern.damage &&
        shot.counter_c>=0 && shot.counter_c<=1 && shot.timer_b<=4 &&
        shot.timer_a<=pattern.maximum*4+3 && shot.timer_c<=(pattern.maximum*4+1)*4;
}
