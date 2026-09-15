#include "walking_kiln.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../items/fire.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include <algorithm>
#include <array>
#include <vector>

namespace {
constexpr std::array<Cell,5> neighbors{{{0,0},{1,0},{0,1},{-1,0},{0,-1}}};
void appearance(Entity& actor) {
    actor.sprite=actor.label_a==KilnCooled ? Sprite::KilnCooled :
        kiln_open(actor) || actor.label_a==KilnFeed ? Sprite::KilnOpen : Sprite::WalkingKiln;
    actor.light=kiln_hot(actor) ? LightEmitter{3,450,{239,137,57}} : LightEmitter{};
    actor.self_light={};
}
void recover(Entity& actor) {
    actor.label_a=KilnRecover;actor.timer_a=48;actor.counter_b=0;
    actor.entity_a={};actor.label_b=0;appearance(actor);
}
bool wood(const Prop& prop) {
    return !prop.broken && prop.hp>0 && (prop.kind==PropKind::Crate ||
        prop.kind==PropKind::RottenLog || prop.kind==PropKind::Twigs);
}
bool coal(const Entity& item) {
    return item.kind==EntityKind::GroundItem && item.ground_item.kind==ItemKind::CoalLump &&
        item.ground_item.count>0 && item.ground_item.flight.slot<0 && item.toss.ticks==0;
}
bool start_feed(Game& game,Entity& actor) {
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& item=game.entities[static_cast<std::size_t>(slot)];
        if (!coal(item) || distance(actor.cell,item.cell)>1) continue;
        actor.entity_a={slot,item.generation};actor.point_b=item.cell;actor.label_b=0;
        actor.label_a=KilnFeed;actor.timer_a=60;actor.point_a=actor.cell;
        actor.facing=cardinal_toward(actor.cell,actor.point_b,actor.facing);
        appearance(actor);emit_sound(game,SoundId::KilnFeed,actor.cell);return true;
    }
    for (Cell side:neighbors) {
        const Cell cell=actor.cell+side;const Prop& prop=game.stage.at_or_border(cell).prop;
        if (!wood(prop)) continue;
        actor.entity_a={};actor.point_b=cell;actor.label_b=static_cast<int>(prop.kind);
        actor.label_a=KilnFeed;actor.timer_a=60;actor.point_a=actor.cell;
        actor.facing=cardinal_toward(actor.cell,actor.point_b,actor.facing);
        appearance(actor);emit_sound(game,SoundId::KilnFeed,actor.cell);return true;
    }
    return false;
}
void finish_feed(Game& game,Entity& actor) {
    bool ate=false;
    if (Entity* item=get_entity(game,actor.entity_a)) {
        if (coal(*item) && item->cell==actor.point_b && distance(actor.cell,item->cell)<=1) {
            if (--item->ground_item.count==0) remove_entity(game,actor.entity_a);
            ate=true;
        }
    } else if (actor.label_b>0) {
        Tile* tile=game.stage.at(actor.point_b);
        if (tile && wood(tile->prop) && static_cast<int>(tile->prop.kind)==actor.label_b &&
            distance(actor.cell,actor.point_b)<=1) {
            hit_prop(game,actor.point_b,tile->prop.hp,actor.cell);ate=true;
        }
    }
    if (ate) actor.counter_a=std::min(3,actor.counter_a+1);
    recover(actor);
}
void breathe(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const Handle self{slot,actor.generation};const Cell origin=actor.cell,forward=actor.point_b;
    const Cell side{-forward.y,forward.x};const int row=actor.counter_b;
    // Freeze this row's visibility before any fire changes its cover.
    std::vector<Cell> cells;
    for (int lane=-1;lane<=1;++lane) {
        const Cell cell=origin+Cell{forward.x*row+side.x*lane,forward.y*row+side.y*lane};
        if (game.stage.at(cell) && clear_heat_sight(game,origin,cell)) cells.push_back(cell);
    }
    for (Cell cell:cells) {
        const bool covered=prop_blocks(game.stage.at_or_border(cell).prop);
        ignite_surface(game,cell);warm_surface(game,cell,120);
        if (game.shot_count<static_cast<int>(game.shots.size()))
            game.shots[static_cast<std::size_t>(game.shot_count++)]={origin,cell,false,false,false,false,false,false,true};
        if (covered) continue;
        std::vector<Handle> victims;
        for (int victim=0;victim<max_entities;++victim) {
            const Entity& target=game.entities[static_cast<std::size_t>(victim)];
            if (victim!=slot && target.kind!=EntityKind::None && target.health>0 && target.cell==cell)
                victims.push_back({victim,target.generation});
        }
        for (Handle victim:victims) if (get_entity(game,victim)) {
            ignite_struck_actor(game,victim.slot);
            damage_entity(game,victim.slot,8,origin,true,self);
        }
        if (!get_entity(game,self) || actor.health<=0 || actor.label_a!=KilnBreath) return;
    }
    emit_sound(game,SoundId::KilnBreath,origin);make_noise(game,origin,10);
    if (++actor.counter_b>3) recover(actor);
    else actor.timer_a=12;
}
void seek_fuel(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    Cell next=actor.cell;int best=7;
    const auto consider=[&](Cell food) {
        const int range=distance(actor.cell,food);
        if (range>=best) return;
        // Walk to a reachable feeding stance, not into the solid crate itself.
        // Sealed fuel cannot monopolize the search while reachable stock exists.
        for (Cell side:neighbors) {
            const Cell stand=food+side;const Tile* tile=game.stage.at(stand);
            if (!tile || !walkable(*tile) || entity_at(game,stand,true)>=0) continue;
            if (const auto route=next_route_cell(game,slot,stand,256)) {
                next=*route;best=range;return;
            }
        }
    };
    for (const Entity& item:game.entities) if (coal(item)) consider(item.cell);
    for (int y=-6;y<=6;++y) for (int x=-6;x<=6;++x) {
        const Cell cell=actor.cell+Cell{x,y};
        if (wood(game.stage.at_or_border(cell).prop)) consider(cell);
    }
    if (best<7) willing_step(game,slot,next);else wander(game,slot);
}
}
// SLOTS: label_a phase; timer_a clock; counter_a fuel 0..3; counter_b movement
// beats 0..4 in Walk, next flame row 1..3 in Breath. point_a committed origin;
// point_b fixed breath direction or food cell. entity_a generation-safe coal;
// label_b expected wood prop kind (zero for coal). c slots remain hearing.
void init_walking_kiln(Entity& actor) {
    actor.health=actor.max_health=160;actor.move_interval=36;actor.impassable=true;
    actor.counter_a=2;appearance(actor);
}
bool kiln_open(const Entity& actor) {
    return actor.kind==EntityKind::WalkingKiln && (actor.label_a==KilnDoor ||
        actor.label_a==KilnBreath || actor.label_a==KilnCooled || actor.label_a==KilnFeed);
}
bool kiln_hot(const Entity& actor) {
    return actor.kind==EntityKind::WalkingKiln && actor.health>0 && actor.counter_a>0 &&
        actor.label_a!=KilnCooled && actor.freeze_ticks==0;
}
bool cool_walking_kiln(Entity& actor) {
    if (actor.kind!=EntityKind::WalkingKiln || actor.health<=0) return false;
    const bool changed=actor.label_a!=KilnCooled;
    actor.label_a=KilnCooled;actor.timer_a=120;actor.counter_b=0;
    actor.entity_a={};actor.label_b=0;appearance(actor);return changed;
}
void interrupt_walking_kiln(Entity& actor) {
    if (actor.kind==EntityKind::WalkingKiln && (actor.label_a==KilnDoor ||
        actor.label_a==KilnBreath || actor.label_a==KilnFeed)) recover(actor);
}
void step_walking_kiln(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.freeze_ticks>0 || surface_wet(game.stage.at_or_border(actor.cell))) {
        if (cool_walking_kiln(actor)) emit_sound(game,SoundId::ColdQuench,actor.cell);
    }
    if (actor.vitals.rooted>0) interrupt_walking_kiln(actor);
    if (actor.label_a==KilnCooled || actor.label_a==KilnRecover) {
        if (actor.timer_a==0) {actor.label_a=KilnWalk;appearance(actor);}
        return;
    }
    if (actor.label_a!=KilnWalk) {
        if (actor.cell!=actor.point_a) {recover(actor);return;}
        if (actor.timer_a>0) return;
        if (actor.label_a==KilnFeed) {finish_feed(game,actor);return;}
        actor.facing=actor.point_b;
        if (actor.label_a==KilnDoor) {
            if (actor.counter_a<=0) {recover(actor);return;}
            --actor.counter_a;actor.counter_b=1;actor.label_a=KilnBreath;
        }
        const Handle self{slot,actor.generation};
        breathe(game,slot);
        if (Entity* live=get_entity(game,self);live && live->health>0) appearance(*live);
        return;
    }
    if (actor.counter_a<3 && start_feed(game,actor)) return;
    if (actor.move_wait>0 || actor.vitals.rooted>0) return;
    if (actor.counter_a==0) {seek_fuel(game,slot);return;}
    const auto target=enemy_target(game,actor.cell,9);
    if (!target) {wander(game,slot);return;}
    actor.counter_b=std::min(4,actor.counter_b+1);
    const Cell facing=cardinal_toward(actor.cell,target->cell,actor.facing),delta=target->cell-actor.cell;
    const int ahead=delta.x*facing.x+delta.y*facing.y,side=delta.y*facing.x-delta.x*facing.y;
    if (actor.counter_b==4 && ahead>=1 && ahead<=3 && std::abs(side)<=1 &&
        clear_attack_sight(game,actor.cell,target->cell,false)) {
        actor.point_a=actor.cell;actor.point_b=actor.facing=facing;
        actor.label_a=KilnDoor;actor.timer_a=60;appearance(actor);
        emit_sound(game,SoundId::KilnDoor,actor.cell);return;
    }
    const Handle self{slot,actor.generation};
    pursue(game,slot,target->cell);
    if (!get_entity(game,self) || actor.health<=0) return;
    // Blocked attempts still spend a beat, so an adjacent target cannot stall it.
    actor.move_wait=std::max(actor.move_wait,actor.move_interval);
    emit_sound(game,SoundId::KilnStep,actor.cell);
}
bool valid_walking_kiln(const Entity& actor) {
    if (actor.kind!=EntityKind::WalkingKiln) return true;
    if (actor.label_a<KilnWalk || actor.label_a>KilnRecover || actor.timer_a<0 || actor.timer_a>120 ||
        actor.counter_a<0 || actor.counter_a>3 || actor.counter_b<0 || actor.counter_b>4) return false;
    if (actor.label_a==KilnDoor || actor.label_a==KilnBreath)
        return distance({},actor.point_b)==1 && (actor.label_a!=KilnBreath || (actor.counter_b>=1 && actor.counter_b<=3));
    if (actor.label_a==KilnFeed) return distance(actor.point_a,actor.point_b)<=1 &&
        (actor.label_b==0 || actor.label_b==static_cast<int>(PropKind::Crate) ||
         actor.label_b==static_cast<int>(PropKind::RottenLog) || actor.label_b==static_cast<int>(PropKind::Twigs));
    return true;
}
