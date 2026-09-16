#include "old_growth_bear.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../combat/shove.hpp"
#include "../props/interaction.hpp"
#include "../world/terrain_material.hpp"
#include <algorithm>

namespace {
void recover(Entity& bear,bool collision=false) {
    bear.label_a=collision ? GrowthStagger : GrowthRecover;
    bear.timer_a=collision ? 120 : 84;
    bear.move_interval=24;
    bear.sprite=collision ? Sprite::OldGrowthStagger : Sprite::OldGrowthBear;
    bear.self_light={};
}
void rush(Game& game,int slot) {
    auto& bear=game.entities[static_cast<std::size_t>(slot)];
    if(bear.move_wait)return;
    const Cell next=bear.cell+bear.facing;
    const auto* tile=game.stage.at(next);
    if(tile && tile->kind==TileKind::Wall && wooden_terrain(*tile))
        hit_terrain(game,next,bear.cell,120);
    tile=game.stage.at(next);
    if(tile && prop_blocks(tile->prop))hit_prop(game,next,90,bear.cell);
    old_growth_timers(bear);
    if(bear.health<=0 || bear.label_a!=GrowthRush)return;
    tile=game.stage.at(next);
    if(!tile || !walkable(*tile)) {
        emit_sound(game,SoundId::GrowthCrash,bear.cell);recover(bear,true);return;
    }
    const int target=entity_at(game,next,true);
    if(target>=0) {
        auto& victim=game.entities[static_cast<std::size_t>(target)];
        const bool guard=blocks_facing(victim,bear.cell),solid=victim.hard_blocker;
        damage_entity(game,target,36,bear.cell);
        if(bear.health<=0)return;
        if(!guard && !solid && victim.health>0)shove_actor(game,target,bear.facing,bear.cell);
        emit_sound(game,SoundId::GrowthCrash,next);recover(bear,guard || solid);return;
    }
    if(!move_entity(game,slot,next,false)){recover(bear,true);return;}
    if(bear.health<=0)return;
    bear.point_b=next;
    // Portals, sliding and springs own their displacement; the rush cannot retarget.
    if(bear.cell!=next || bear.label_a!=GrowthRush || --bear.counter_a<=0)recover(bear);
}
}

// point_a territory; point_b committed/expected cell; label_a phase; timer_a
// phase clock; timer_b alert memory; counter_a remaining rush cells. c slots
// remain available to hearing. One ground collision cell, a larger drawn body.
void init_old_growth_bear(Entity& bear) {
    bear.health=bear.max_health=640;bear.move_interval=24;bear.impassable=true;
    bear.sprite=Sprite::OldGrowthBear;bear.point_a=bear.cell;
}
void interrupt_old_growth(Entity& bear,int damage) {
    if(bear.kind!=EntityKind::OldGrowthBear || bear.health<=0)return;
    bear.timer_b=900;
    if(damage>=40 && (bear.label_a==GrowthMaul || bear.label_a==GrowthPaw || bear.label_a==GrowthRush))recover(bear);
}
void old_growth_timers(Entity& bear) {
    if(bear.kind==EntityKind::OldGrowthBear && (bear.label_a==GrowthMaul || bear.label_a==GrowthPaw || bear.label_a==GrowthRush) &&
        (bear.sleep_ticks || bear.stun_ticks || bear.freeze_ticks || bear.vitals.rooted || bear.toss.ticks))recover(bear);
}
void step_old_growth_bear(Game& game,int slot) {
    auto& bear=game.entities[static_cast<std::size_t>(slot)];
    if(bear.label_a==GrowthRecover || bear.label_a==GrowthStagger) {
        if(bear.label_a==GrowthRecover && bear.timer_a<72)bear.sprite=Sprite::OldGrowthBear;
        if(!bear.timer_a){bear.label_a=GrowthRoam;bear.sprite=Sprite::OldGrowthBear;}
        return;
    }
    if(bear.label_a==GrowthMaul || bear.label_a==GrowthPaw || bear.label_a==GrowthRush) {
        if(bear.cell!=bear.point_b){recover(bear);return;}
        if(bear.label_a==GrowthRush){rush(game,slot);return;}
        if(bear.timer_a)return;
        if(bear.label_a==GrowthMaul) {
            resolve_enemy_attack(game,slot,36,SoundId::GrowthMaul);
            recover(bear);bear.sprite=Sprite::OldGrowthSwipe;
        } else {
            bear.label_a=GrowthRush;bear.move_interval=6;bear.move_wait=0;
            bear.sprite=Sprite::OldGrowthRush;bear.self_light={};
            emit_sound(game,SoundId::GrowthRush,bear.cell);
        }
        return;
    }
    const auto target=enemy_target(game,bear.cell,bear.timer_b ? 14 : 9);
    if(!target || distance(bear.cell,bear.point_a)>20) {
        if(distance(bear.cell,bear.point_a)>2)pursue(game,slot,bear.point_a);
        return; // Territorial; leaving the fight never restores lost health.
    }
    if(!clear_attack_sight(game,bear.cell,target->cell)){pursue(game,slot,target->cell);return;}
    bear.timer_b=900;
    const int gap=distance(bear.cell,target->cell);
    const bool charge=gap>=3 && gap<=7 && (bear.cell.x==target->cell.x || bear.cell.y==target->cell.y);
    if(gap<=2 || charge) {
        bear.facing=cardinal_toward(bear.cell,target->cell,bear.facing);bear.point_b=bear.cell;
        bear.label_a=charge ? GrowthPaw : GrowthMaul;bear.timer_a=charge ? 72 : 54;
        bear.counter_a=charge ? 7 : 0;
        bear.sprite=charge ? Sprite::OldGrowthPaw : Sprite::OldGrowthRear;
        bear.self_light={24,13,4};emit_sound(game,SoundId::GrowthRoar,bear.cell);
    } else {pursue(game,slot,target->cell);bear.sprite=Sprite::OldGrowthBear;}
}
