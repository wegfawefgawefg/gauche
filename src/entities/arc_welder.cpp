#include "arc_welder.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/conduction.hpp"
#include <array>
#include <algorithm>

namespace {
void recover(Entity& actor) {
    actor.label_a=WelderRecover;actor.timer_a=72;actor.counter_a=0;
    actor.sprite=Sprite::ArcWelder;
}
void sweep_lane(Game& game,Entity& actor) {
    const Cell source=actor.cell,forward=actor.point_b,side{-forward.y,forward.x};
    const int lane=actor.counter_a-1;
    std::array<ConductedShock,2> shocks;
    int count=0;
    std::array<Cell,2> cuts{};int cut_count=0;
    for (int reach=1;reach<=2;++reach) {
        const Cell contact=source+Cell{forward.x*reach+side.x*lane,forward.y*reach+side.y*lane};
        // Side lanes cannot reach around a solid corner. Current cover also
        // stops the far cell, even when this pass will cut the near grate.
        const Tile* tile=game.stage.at(contact);
        if (tile && prop_cuttable_metal(tile->prop)) {
            if (clear_attack_sight(game,source,contact)) cuts[static_cast<std::size_t>(cut_count++)]=contact;
            break;
        }
        if (!clear_attack_sight(game,source,contact)) break;
        shocks[static_cast<std::size_t>(count++)]=trace_conducted_shock(game,contact,3);
    }
    // A ready sink takes this lane's complete pulse before cutting or damage.
    for (int i=0;i<count;++i) if (shocks[static_cast<std::size_t>(i)].wave.ground_node>=0) {
        apply_conducted_shock(game,shocks[static_cast<std::size_t>(i)],source,7);
        emit_sound(game,SoundId::ArcPulse,source);return;
    }
    if (count==2) {
        auto& first=shocks[0].victims;auto& second=shocks[1].victims;
        // Overlapping water paths do not multiply one lane's damage. Keep the
        // stronger of its two contact distances, while drawing both traces.
        for (auto& victim:first)
            for (const auto& other:second)
                if (victim.handle==other.handle) victim.steps=std::min(victim.steps,other.steps);
        std::erase_if(second,[&](const ShockVictim& victim) {
            return std::any_of(first.begin(),first.end(),[&](const ShockVictim& other) { return victim.handle==other.handle; });
        });
    }
    for (int i=0;i<cut_count;++i) {
        const Cell cell=cuts[static_cast<std::size_t>(i)];
        hit_prop(game,cell,14,source);
        if (game.shot_count<static_cast<int>(game.shots.size()))
            game.shots[static_cast<std::size_t>(game.shot_count++)]={source,cell,true,false,false,true};
    }
    for (int i=0;i<count;++i) apply_conducted_shock(game,shocks[static_cast<std::size_t>(i)],source,7);
    emit_sound(game,cut_count>0 ? SoundId::ArcCut : SoundId::ArcPulse,source);
}
bool in_fan(Cell delta,Cell forward) {
    const int ahead=delta.x*forward.x+delta.y*forward.y;
    const int side=delta.y*forward.x-delta.x*forward.y;
    return ahead>=1 && ahead<=2 && std::abs(side)<=1;
}
}
// SLOTS: label_a phase, timer_a phase, counter_a next lane (0 left,1 middle,2
// right); point_a committed origin, point_b facing. c-slots remain hearing.
void init_arc_welder(Entity& actor) {
    actor.health=actor.max_health=70;actor.move_interval=22;actor.impassable=true;
    actor.sprite=Sprite::ArcWelder;
}
void interrupt_arc_welder(Entity& actor) {
    if (actor.kind==EntityKind::ArcWelder && (actor.label_a==WelderMask || actor.label_a==WelderSweep)) recover(actor);
}
void step_arc_welder(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.label_a==WelderRecover) {
        if (actor.timer_a==0) actor.label_a=WelderReady;
        return;
    }
    if (actor.label_a==WelderMask || actor.label_a==WelderSweep) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) { recover(actor);return; }
        actor.facing=actor.point_b;
        if (actor.timer_a>0) return;
        sweep_lane(game,actor);
        if (actor.health<=0 || actor.label_a==WelderRecover) return;
        actor.use_flash=6;
        if (++actor.counter_a==3) recover(actor);
        else {actor.label_a=WelderSweep;actor.timer_a=9;}
        return;
    }
    const auto target=enemy_target(game,actor.cell,8);
    if (!target) {if (!step_hearing(game,slot)) wander(game,slot);return;}
    const Cell facing=cardinal_toward(actor.cell,target->cell,actor.facing);
    const bool close=in_fan(target->cell-actor.cell,facing) && clear_attack_sight(game,actor.cell,target->cell);
    const bool cut=prop_cuttable_metal(game.stage.at_or_border(actor.cell+facing).prop) && clear_shot_sight(game,actor.cell,target->cell);
    if (close || cut) {
        actor.point_a=actor.cell;actor.point_b=actor.facing=facing;
        actor.label_a=WelderMask;actor.timer_a=42;actor.counter_a=0;
        actor.sprite=Sprite::WelderMask;emit_sound(game,SoundId::WelderMask,actor.cell);return;
    }
    pursue(game,slot,target->cell);
}
bool valid_arc_welder(const Entity& actor) {
    if (actor.kind!=EntityKind::ArcWelder) return true;
    if (actor.label_a<WelderReady || actor.label_a>WelderRecover || actor.timer_a>72 || actor.counter_a<0 || actor.counter_a>2) return false;
    return (actor.label_a!=WelderMask && actor.label_a!=WelderSweep) || distance({},actor.point_b)==1;
}
