#include "pressure_rat.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <array>

namespace {
void burst(Game& game,int slot) {
    Entity& rat=game.entities[static_cast<std::size_t>(slot)];
    if (rat.health<=0 || rat.label_a!=RatDash) return;
    const Cell origin=rat.cell;
    const Handle source{slot,rat.generation};
    rat.label_a=RatSpent;rat.counter_a=0;
    // Kill once before damaging neighbours. A chain reaction cannot re-enter
    // this rat, and normal death owns its single drop roll.
    damage_entity(game,slot,rat.health,origin,false,source);
    constexpr std::array offsets{Cell{0,0},Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}};
    for (Cell offset:offsets) {
        const Cell cell=origin+offset;
        if (!game.stage.at(cell) || !clear_attack_sight(game,origin,cell,false)) continue;
        hit_prop(game,cell,8,origin);
        for (int victim=0;victim<max_entities;++victim) {
            const Entity& actor=game.entities[static_cast<std::size_t>(victim)];
            if (victim!=slot && actor.cell==cell && actor.health>0) damage_entity(game,victim,8,origin,true,source);
        }
    }
    emit_sound(game,SoundId::RatBurst,origin);
}
}
// SLOTS: label_a phase, timer_a inflation/cooling, counter_a remaining dash
// cells (24 maximum); point_a expected position, point_b committed heading.
// A carried pressure bladder supplies one charge, not infinite spawned bombs.
void init_pressure_rat(Entity& rat) {
    rat.health=rat.max_health=22;rat.move_interval=12;rat.impassable=true;
    rat.sprite=Sprite::PressureRat;
}
bool cool_pressure_rat(Entity& rat) {
    if (rat.kind!=EntityKind::PressureRat || rat.health<=0) return false;
    rat.label_a=RatCooled;rat.timer_a=120;rat.counter_a=0;
    rat.move_interval=8;rat.sprite=Sprite::PressureRat;
    return true;
}
void step_pressure_rat(Game& game,int slot) {
    Entity& rat=game.entities[static_cast<std::size_t>(slot)];
    if (rat.health<=0) return;
    if (rat.freeze_ticks>0 || surface_wet(game.stage.at_or_border(rat.cell))) cool_pressure_rat(rat);
    if (rat.label_a==RatCooled) {
        if (rat.timer_a==0) {rat.label_a=RatRoam;rat.move_interval=12;}
        else if (const auto target=enemy_target(game,rat.cell,7)) flee(game,slot,target->cell);
        return;
    }
    if (rat.label_a==RatInflate || rat.label_a==RatDash) {
        if (rat.cell!=rat.point_a || rat.vitals.rooted>0) {cool_pressure_rat(rat);return;}
        rat.facing=rat.point_b;
        if (rat.label_a==RatInflate) {
            if (rat.timer_a>0) return;
            rat.label_a=RatDash;rat.move_interval=6;rat.move_wait=0;rat.sprite=Sprite::RatDash;
            emit_sound(game,SoundId::RatRush,rat.cell);
            return;
        }
        if (rat.move_wait>0) return;
        const Cell next=rat.cell+rat.point_b;
        const Tile* tile=game.stage.at(next);
        if (!tile || !walkable(*tile) || entity_at(game,next,true)>=0) {burst(game,slot);return;}
        if (!move_entity(game,slot,next,false)) {burst(game,slot);return;}
        if (rat.health<=0 || rat.label_a!=RatDash) return;
        rat.point_a=next;
        if (rat.cell!=next) {cool_pressure_rat(rat);return;}
        if (--rat.counter_a==0) burst(game,slot);
        return;
    }
    const auto target=enemy_target(game,rat.cell,7);
    if (!target || !clear_attack_sight(game,rat.cell,target->cell)) {
        if (!step_hearing(game,slot)) wander(game,slot);
        return;
    }
    if (target->cell!=rat.cell && (target->cell.x==rat.cell.x || target->cell.y==rat.cell.y)) {
        rat.facing=cardinal_toward(rat.cell,target->cell,rat.facing);
        rat.point_a=rat.cell;rat.point_b=rat.facing;
        rat.label_a=RatInflate;rat.timer_a=36;rat.counter_a=24;rat.sprite=Sprite::RatInflate;
        emit_sound(game,SoundId::RatInflate,rat.cell);
    } else if (rat.move_wait==0) approach(game,slot,target->cell);
}
bool valid_pressure_rat(const Entity& rat) {
    if (rat.kind!=EntityKind::PressureRat) return true;
    if (rat.label_a<RatRoam || rat.label_a>RatSpent || rat.timer_a>120 || rat.counter_a<0 || rat.counter_a>24) return false;
    if (rat.label_a==RatSpent && rat.health>0) return false;
    if (rat.label_a==RatInflate && rat.timer_a>36) return false;
    return (rat.label_a!=RatInflate && rat.label_a!=RatDash) || (rat.counter_a>0 && distance({},rat.point_b)==1);
}
