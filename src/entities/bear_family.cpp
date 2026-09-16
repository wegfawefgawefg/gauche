#include "bear_family.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include <algorithm>

void set_bear_role(Entity& bear,BearRole role) {
    bear.counter_a=role;
    bear.sprite=role==BearCub ? Sprite::BearCub : role==BearMother ? Sprite::BearMother :
        role==BearFather ? Sprite::BearFather : role==BearOld ? Sprite::BearOld : Sprite::Bear;
    bear.health=bear.max_health=role==BearCub ? 36 : role==BearMother ? 180 :
        role==BearFather ? 220 : role==BearOld ? 320 : 140;
    bear.move_interval=role==BearCub ? 18 : role==BearMother ? 26 : role==BearFather ? 32 : role==BearOld ? 36 : 28;
}
int bear_reach(const Entity& bear) {return bear.counter_a==BearOld ? 3 : 2;}
int bear_damage(const Entity& bear) {
    return bear.counter_a==BearMother ? 28 : bear.counter_a==BearFather ? 32 : bear.counter_a==BearOld ? 42 : 24;
}
float bear_size(const Entity& bear) {
    return bear.counter_a==BearCub ? .75F : bear.counter_a==BearMother ? 1.25F :
        bear.counter_a==BearFather ? 1.5F : bear.counter_a==BearOld ? 1.75F : 1;
}

void alarm_bear_family(Game& game,int victim_slot,Cell source) {
    const auto& victim=game.entities[static_cast<std::size_t>(victim_slot)];
    if (victim.kind!=EntityKind::Bear || victim.entity_a.slot<0) return;
    const auto heard=audible_cells(game,victim.cell,10);
    // Identity survives a parent's death, but a reused entity slot never joins
    // the family. Walls and distance keep this a local response, not telepathy.
    for (auto& bear:game.entities) {
        if (bear.kind!=EntityKind::Bear || bear.health<=0 || bear.entity_a!=victim.entity_a ||
            std::find(heard.begin(),heard.end(),bear.cell)==heard.end()) continue;
        const bool warning=bear.sleep_ticks>0 || bear.timer_b==0;
        bear.sleep_ticks=0;bear.timer_b=600;bear.point_b=source;
        bear.counter_b=360;bear.label_b=0;
        if (bear.label_a==0) {bear.point_c=source;bear.label_c=InvestigateNoise;bear.timer_c=300;}
        if (warning && bear.counter_a!=BearCub) emit_sound(game,SoundId::BearRear,bear.cell);
    }
}

bool step_bear_cub(Game& game,int slot) {
    auto& cub=game.entities[static_cast<std::size_t>(slot)];
    if (cub.counter_a!=BearCub) return false;
    const int threat=nearest_player(game,cub.cell,4);
    if (threat>=0) {
        const Cell source=game.entities[static_cast<std::size_t>(threat)].cell;
        if (clear_attack_sight(game,cub.cell,source)) {
            if (cub.timer_b==0) alarm_bear_family(game,slot,source);
            cub.point_b=source;cub.timer_b=300;flee(game,slot,source);return true;
        }
    }
    if (cub.timer_b>0) {flee(game,slot,cub.point_b);return true;}
    if (const auto* parent=get_entity(game,cub.entity_a);parent && parent->kind==EntityKind::Bear && parent->health>0) {
        if (distance(cub.cell,parent->cell)>2) pursue(game,slot,parent->cell);
        else if (game.tick%90==0) wander(game,slot);
    } else if (distance(cub.cell,cub.point_a)>2) pursue(game,slot,cub.point_a);
    else if (game.tick%90==0) wander(game,slot);
    return true;
}
