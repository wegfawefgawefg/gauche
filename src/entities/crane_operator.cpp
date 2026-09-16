#include "crane_operator.hpp"
#include "magnet_crane.hpp"
#include "behavior.hpp"

// SLOTS: entity_a is the staffed crane; point_b is the fixed control stand;
// point_a is the last harmful source; label_a work/wait/flee/return, timer_a
// alarm duration. Crane label_b latches staffed mode, entity_b owns this worker.
// Killing/removing a worker never silently converts their crane to automatic.
void init_crane_operator(Entity& actor) {
    actor.health=actor.max_health=32;actor.impassable=true;actor.move_interval=18;
    actor.sprite=Sprite::CraneOperator;actor.point_b=actor.cell;
}
Handle staff_crane(Game& game,Handle handle,Cell station) {
    Entity* crane=get_entity(game,handle);
    const Tile* tile=game.stage.at(station);
    if (!crane || crane->kind!=EntityKind::MagnetCrane || crane->label_b!=0 ||
        distance(crane->cell,station)!=1 || !tile || !walkable(*tile) || entity_at(game,station,true)>=0) return {};
    const Handle worker=spawn_entity(game,EntityKind::CraneOperator,station);
    Entity* actor=get_entity(game,worker);
    if (!actor) return {};
    actor->entity_a=handle;actor->facing=crane->cell-station;
    crane->entity_b=worker;crane->label_b=1;
    return worker;
}
bool crane_operator_ready(const Game& game,const Entity& crane) {
    if (crane.label_b==0) return true;
    const Entity* worker=get_entity(game,crane.entity_b);
    return worker && worker->kind==EntityKind::CraneOperator && worker->health>0 &&
        get_entity(game,worker->entity_a)==&crane && worker->cell==worker->point_b &&
        distance(worker->cell,crane.cell)==1 && worker->timer_a==0 &&
        worker->stun_ticks==0 && worker->sleep_ticks==0 && worker->freeze_ticks==0 &&
        worker->toss.ticks==0 && worker->vitals.rooted==0;
}
void hurt_crane_operator(Game& game,Entity& victim,Cell source) {
    Entity* worker=victim.kind==EntityKind::CraneOperator ? &victim :
        victim.kind==EntityKind::MagnetCrane && victim.label_b==1 ? get_entity(game,victim.entity_b) : nullptr;
    if (!worker || worker->kind!=EntityKind::CraneOperator || worker->health<=0) return;
    if (worker->timer_a==0) emit_sound(game,SoundId::CrewWarn,worker->cell);
    worker->point_a=source;worker->timer_a=180;worker->label_a=OperatorFleeing;
    worker->sprite=Sprite::CraneOperatorAlarm;
    Entity* crane=get_entity(game,worker->entity_a);
    if (crane && crane->kind==EntityKind::MagnetCrane) interrupt_magnet_crane(*crane);
}
void step_crane_operator(Game& game,int slot) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    if (worker.timer_a>0) {
        worker.label_a=OperatorFleeing;worker.sprite=Sprite::CraneOperatorAlarm;
        if (distance(worker.cell,worker.point_a)<5) flee(game,slot,worker.point_a);
        return;
    }
    const Entity* crane=get_entity(game,worker.entity_a);
    if (!crane || crane->kind!=EntityKind::MagnetCrane || crane->health<=0 ||
        get_entity(game,crane->entity_b)!=&worker || distance(crane->cell,worker.point_b)!=1) {
        worker.label_a=OperatorWaiting;worker.sprite=Sprite::CraneOperatorWait;return;
    }
    if (worker.cell!=worker.point_b) {
        worker.label_a=OperatorReturning;worker.sprite=Sprite::CraneOperator;
        if (worker.move_wait==0) {
            const auto next=next_route_cell(game,slot,worker.point_b,1024);
            if (next) willing_step(game,slot,*next);
            else worker.move_wait=worker.move_interval;
        }
        return;
    }
    worker.facing=crane->cell-worker.cell;
    const bool active=crane->freeze_ticks==0 && crane->stun_ticks==0 && crane->sleep_ticks==0 &&
        (crane->label_a==CraneTravel || crane->label_a==CraneLock);
    worker.label_a=active ? OperatorWorking : OperatorWaiting;
    worker.sprite=active ? (game.tick%48<24 ? Sprite::CraneOperatorWork : Sprite::CraneOperator) : Sprite::CraneOperatorWait;
}
bool valid_crane_operator(const Entity& actor) {
    return actor.kind!=EntityKind::CraneOperator ||
        (actor.label_a>=OperatorWorking && actor.label_a<=OperatorReturning &&
         actor.timer_a>=0 && actor.timer_a<=180);
}
