#include "ant.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include <algorithm>

namespace {
Sprite ant_pose(const Entity& ant,int pose=0) {
    const auto base=ant.counter_a==AntCaptain ? Sprite::AntCaptain : Sprite::Ant;
    return static_cast<Sprite>(static_cast<int>(base)+pose);
}
void rest(Entity& ant,int ticks) {
    ant.label_a=AntRecovery;ant.timer_a=ticks;ant.sprite=ant_pose(ant);ant.self_light={};
}
bool active(const Entity* e,EntityKind kind) {return e && e->kind==kind && e->health>0;}
void travel(Game& game,int slot,Cell destination) {
    auto& ant=game.entities[static_cast<std::size_t>(slot)];
    if (ant.move_wait) return;
    const Cell before=ant.cell;
    if (const auto next=next_route_cell(game,slot,destination,2048)) willing_step(game,slot,*next);
    else wander(game,slot); // A worker yields when another ant occupies a narrow trail.
    if (ant.cell==before) ant.move_wait=18; // Blocked routes do not search every tick.
}
void show_work(Entity& ant,std::uint64_t tick) {
    if (ant.counter_b>0) ant.sprite=ant_pose(ant,4);
    else ant.sprite=ant_pose(ant,ant.move_wait && (tick/10)%2 ? 1 : 0);
}
}

// ANT: counter_a role; counter_b carried sugar units; label_a phase;
// timer_a phase/handling delay; timer_b remembered threat lifetime;
// entity_a nest, entity_b threat (generation-qualified); point_a/b bite origin/target.
// NEST: counter_a delivered units; timer_a alarm, timer_b whistle work boost;
// entity_a attacker, entity_b finite sugar source. SUGAR: counter_a remaining units,
// entity_a owning nest. Colony links never use room encounter ownership.
void set_ant_role(Entity& ant,AntRole role) {
    ant.counter_a=role;ant.health=ant.max_health=role==AntPorter ? 40 : role==AntCaptain ? 28 : 12;
    ant.move_interval=role==AntPorter ? 20 : 14;ant.sprite=ant_pose(ant);
}
void init_ant(Entity& ant) {ant.impassable=true;set_ant_role(ant,AntWorker);}
void init_ant_nest(Entity& nest) {
    nest.sprite=Sprite::AntNest;nest.health=nest.max_health=90;nest.impassable=true;nest.hard_blocker=true;
}
void init_ant_sugar(Entity& sugar) {
    sugar.sprite=Sprite::AntSugar;sugar.health=sugar.max_health=16;sugar.counter_a=60;
    sugar.impassable=true;sugar.hard_blocker=true;
}
float ant_size(const Entity& ant) {return ant.counter_a==AntPorter ? 1.35F : ant.counter_a==AntCaptain ? 1.F : .7F;}
void ant_timers(Entity& ant) {
    if (ant.kind!=EntityKind::Ant) return;
    if ((ant.label_a==AntWindup || ant.label_a==AntWhistling) &&
        (ant.sleep_ticks || ant.stun_ticks || ant.vitals.rooted || ant.toss.ticks)) rest(ant,40);
}
void step_ant_nest(Entity& nest) {if (!nest.timer_a) nest.entity_a={};}
void step_ant_sugar(Entity& sugar) {
    sugar.sprite=sugar.counter_a>18 ? Sprite::AntSugar : sugar.counter_a>0 ? Sprite::AntSugarLow : Sprite::AntSugarEmpty;
    sugar.impassable=sugar.hard_blocker=sugar.counter_a>0;
}
void hurt_ant_colony(Game& game,int slot,Cell attacker) {
    auto& victim=game.entities[static_cast<std::size_t>(slot)];
    if (victim.kind!=EntityKind::Ant && victim.kind!=EntityKind::AntNest && victim.kind!=EntityKind::AntSugar) return;
    if (victim.kind==EntityKind::Ant) rest(victim,36);
    const int from=entity_at(game,attacker,true);
    if (from<0 || from==slot) return;
    const auto& threat=game.entities[static_cast<std::size_t>(from)];
    const Handle colony=victim.kind==EntityKind::AntNest ? Handle{slot,victim.generation} : victim.entity_a;
    if (threat.kind==EntityKind::Ant && threat.entity_a==colony) return;
    const Handle handle{from,threat.generation};
    if (auto* nest=get_entity(game,colony);active(nest,EntityKind::AntNest)) {
        nest->entity_a=handle;nest->timer_a=600;
    }
    // Copy the alarm now: killing the nest must not erase its defenders' memory.
    for (auto& ant:game.entities) if (ant.kind==EntityKind::Ant && ant.health>0 && ant.entity_a==colony) {
        ant.entity_b=handle;ant.timer_b=600;
    }
}
void step_ant(Game& game,int slot) {
    auto& ant=game.entities[static_cast<std::size_t>(slot)];
    if (ant.label_a==AntWindup) {
        if (ant.cell!=ant.point_a || ant.toss.ticks) {rest(ant,45);return;}
        if (ant.timer_a) return;
        resolve_enemy_attack(game,slot,ant.counter_a==AntPorter ? 12 : 5,SoundId::SpiderBite);
        rest(ant,ant.counter_a==AntPorter ? 72 : 48);ant.sprite=ant_pose(ant,3);return;
    }
    if (ant.label_a==AntRecovery && ant.timer_a) return;
    auto* nest=get_entity(game,ant.entity_a);
    const bool home=active(nest,EntityKind::AntNest);
    if (home && nest->timer_a && get_entity(game,nest->entity_a)) {ant.entity_b=nest->entity_a;ant.timer_b=600;}
    const auto* threat=ant.timer_b ? get_entity(game,ant.entity_b) : nullptr;
    if (threat && threat->health>0 && distance(ant.cell,threat->cell)<18) {
        ant.label_a=AntWorking;ant.move_interval=ant.counter_a==AntPorter ? 18 : 12;
        if (distance(ant.cell,threat->cell)==1 && clear_attack_sight(game,ant.cell,threat->cell)) {
            ant.point_a=ant.cell;ant.point_b=threat->cell;ant.facing=cardinal_toward(ant.cell,threat->cell,ant.facing);
            ant.label_a=AntWindup;ant.timer_a=ant.counter_a==AntPorter ? 45 : 30;
            ant.sprite=ant_pose(ant,2);ant.self_light={24,12,6};emit_sound(game,SoundId::SpiderWarn,ant.cell);
        } else {travel(game,slot,threat->cell);show_work(ant,game.tick);}
        return;
    }
    if (ant.label_a==AntWhistling) {
        if (ant.cell!=ant.point_a) {rest(ant,30);return;}
        if (ant.timer_a) return;
        if (home) {nest->timer_b=180;emit_sound(game,SoundId::CrewWhistle,ant.cell);}
        ant.label_a=AntWorking;ant.timer_a=420;ant.sprite=ant_pose(ant);return;
    }
    ant.label_a=AntWorking;
    if (!home) {
        if (!ant.move_wait && game.tick%45==0) wander(game,slot);
        show_work(ant,game.tick);return;
    }
    ant.move_interval=(ant.counter_a==AntPorter ? 20 : 14)-(nest->timer_b ? 4 : 0);
    if (ant.counter_a==AntCaptain) {
        if (distance(ant.cell,nest->cell)>3) travel(game,slot,nest->cell);
        else if (!ant.timer_a) {
            ant.label_a=AntWhistling;ant.timer_a=36;ant.point_a=ant.cell;ant.sprite=ant_pose(ant,2);
        }
        return;
    }
    if (ant.timer_a) {show_work(ant,game.tick);return;}
    if (ant.counter_b>0) {
        if (distance(ant.cell,nest->cell)<=1) {
            nest->counter_a=std::min(4096,nest->counter_a+ant.counter_b);ant.counter_b=0;ant.timer_a=24;
            emit_sound(game,SoundId::ScavengeTake,ant.cell);
        } else travel(game,slot,nest->cell);
    } else {
        auto* source=get_entity(game,nest->entity_b);
        if (!active(source,EntityKind::AntSugar) || source->counter_a<=0) {
            if (distance(ant.cell,nest->cell)>4) travel(game,slot,nest->cell);
            else if (!ant.move_wait && game.tick%60==0) wander(game,slot);
        } else if (distance(ant.cell,source->cell)<=1) {
            ant.counter_b=std::min(ant.counter_a==AntPorter ? 3 : 1,source->counter_a);
            source->counter_a-=ant.counter_b;ant.timer_a=30;step_ant_sugar(*source);
            emit_sound(game,SoundId::ScavengeTake,ant.cell);
        } else travel(game,slot,source->cell);
    }
    show_work(ant,game.tick);
}
