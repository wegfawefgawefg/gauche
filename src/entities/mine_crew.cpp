#include "mine_crew.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
// SHARED: a-handle is foreman (self on leader); b-handle is a real attacker.
// label_a phase; label_b leader's active order. point_a windup origin; point_b
// committed cut/strike, or leader's work direction outside combat. counter_b is
// worker lane (-1,0,+1); counter_a worker damage window / leader casualty count.
// timer_a phase; timer_b worker damage window / leader order lifetime.
// attack_wait is 10s hostility, not swing recovery. Hearing keeps the c slots.
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};

Sprite idle_sprite(const Entity& worker) {
    return worker.kind==EntityKind::Pickhand ? Sprite::Pickhand : Sprite::ShiftForeman;
}
void rest(Entity& worker,int ticks) {
    worker.label_a=CrewRest; worker.timer_a=ticks; worker.sprite=idle_sprite(worker);
}
void hostile(Entity& worker,Handle threat) {
    worker.entity_b=threat; worker.attack_wait=600;
    worker.label_b=0;
    rest(worker,18);
    worker.move_wait=std::min(worker.move_wait,8);
}
void rally(Game& game,Handle leader,Handle threat,Cell source) {
    bool any=false;
    for (Entity& worker:game.entities) {
        if (!mine_worker(worker.kind) || worker.health<=0 || worker.entity_a!=leader) continue;
        hostile(worker,threat); any=true;
    }
    if (any) { emit_sound(game,SoundId::CrewAlarm,source); make_noise(game,source,12); }
}
bool safe_space(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind!=TileKind::Lava && entity_at(game,cell,true)<0;
}
void yield_lane(Game& game,int slot) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    // Alternate free sides, never repeatedly probe one blocked direction.
    const unsigned int first=random_u32(game)%4;
    for (unsigned int i=0;i<4;++i) {
        const Cell at=worker.cell+sides[(first+i)%4];
        if (safe_space(game,at)) { willing_step(game,slot,at); return; }
    }
    worker.move_wait=12;
}
void begin(Game& game,Entity& worker,Cell cell,bool cutting) {
    worker.point_a=worker.cell; worker.point_b=cell;
    worker.facing=cardinal_toward(worker.cell,cell,worker.facing);
    worker.label_a=cutting ? CrewCut : CrewStrike;
    worker.timer_a=cutting ? 36 : 30;
    worker.sprite=worker.kind==EntityKind::Pickhand ? Sprite::PickhandRaise : Sprite::ForemanRaise;
    emit_sound(game,cutting ? SoundId::CrewHeave : SoundId::CrewWarn,worker.cell);
}
void advance_worker(Game& game,int slot,Cell target) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    if (worker.move_wait>0 || worker.cell==target) return;
    const auto next=mine_route_step(game,slot,target);
    if (!next) { yield_lane(game,slot); return; }
    const Tile& tile=game.stage.at_or_border(*next);
    if (crew_diggable(tile) || prop_blocks(tile.prop)) { begin(game,worker,*next,true); return; }
    if (safe_space(game,*next)) willing_step(game,slot,*next);
    else yield_lane(game,slot);
}
void choose_order(Game& game,Entity& leader) {
    int best=-10000;
    Cell direction=leader.facing;
    const unsigned int first=random_u32(game)%4;
    for (unsigned int i=0;i<4;++i) {
        const Cell side=sides[(first+i)%4];
        int score=0;
        for (int reach=1;reach<=6;++reach) {
            const Tile* tile=game.stage.at(leader.cell+Cell{side.x*reach,side.y*reach});
            if (!tile || tile->kind==TileKind::Lava || (!walkable(tile->kind) && !crew_diggable(*tile))) { score-=20; break; }
            score+=crew_diggable(*tile) ? 5 : 1;
        }
        if (score>best) { best=score; direction=side; }
    }
    for (Entity& worker:game.entities)
        if (worker.kind==EntityKind::Pickhand && worker.entity_a==leader.entity_a) worker.label_b=0;
    leader.point_a=leader.cell; leader.point_b=direction; leader.facing=direction;
    leader.label_b=0; leader.label_a=CrewWhistle; leader.timer_a=36;
    leader.sprite=Sprite::ForemanWhistle;
    emit_sound(game,SoundId::CrewWhistle,leader.cell);
}
void direct_crew(Game& game,int slot) {
    Entity& leader=game.entities[static_cast<std::size_t>(slot)];
    if (leader.timer_b==0 || leader.label_b==0) { choose_order(game,leader); return; }
    if (leader.move_wait>0) return;
    const Cell direction=leader.point_b;
    bool ahead=false;
    for (const Entity& worker:game.entities)
        if (worker.kind==EntityKind::Pickhand && worker.health>0 && worker.entity_a==leader.entity_a &&
            (worker.cell.x-leader.cell.x)*direction.x+(worker.cell.y-leader.cell.y)*direction.y>=3) ahead=true;
    if (ahead && safe_space(game,leader.cell+direction)) {
        willing_step(game,slot,leader.cell+direction);
        leader.facing=direction;
    } else leader.move_wait=18;
}
} // namespace

void init_mine_worker(Entity& worker) {
    const bool leader=worker.kind==EntityKind::ShiftForeman;
    worker.health=worker.max_health=leader ? 110 : 80;
    worker.move_interval=leader ? 18 : 10;
    worker.impassable=true;
    worker.sprite=idle_sprite(worker);
    worker.light={2,650,{220,180,105}};
}
void interrupt_mine_worker(Entity& worker) {
    if (!mine_worker(worker.kind)) return;
    if (worker.label_a==CrewWhistle) worker.label_b=0;
    if (worker.label_a==CrewCut || worker.label_a==CrewStrike || worker.label_a==CrewWhistle) rest(worker,24);
}
void hurt_mine_worker(Game& game,int slot,int damage,Cell source) {
    Entity& victim=game.entities[static_cast<std::size_t>(slot)];
    if (!mine_worker(victim.kind)) return;
    interrupt_mine_worker(victim);
    const int attacker=entity_at(game,source,true);
    if (attacker<0 || attacker==slot) return;
    const Entity& culprit=game.entities[static_cast<std::size_t>(attacker)];
    if (mine_worker(culprit.kind) && victim.entity_a.slot>=0 && culprit.entity_a==victim.entity_a) return;
    const Handle threat{attacker,culprit.generation};
    if (victim.kind==EntityKind::ShiftForeman) { rally(game,victim.entity_a,threat,victim.cell); return; }
    if (victim.health==0) {
        Entity* leader=get_entity(game,victim.entity_a);
        if (leader && leader->kind==EntityKind::ShiftForeman && leader->health>0 && ++leader->counter_a>=2)
            rally(game,victim.entity_a,threat,victim.cell);
        return;
    }
    if (victim.timer_b==0) { victim.counter_a=0; victim.timer_b=120; }
    victim.counter_a+=std::min(damage,1000);
    if (damage>=12 || victim.counter_a>=20) hostile(victim,threat);
    else emit_sound(game,SoundId::CrewComplain,victim.cell);
}
void step_mine_worker(Game& game,int slot) {
    Entity& worker=game.entities[static_cast<std::size_t>(slot)];
    if (worker.label_a==CrewRest) {
        if (worker.timer_a==0) { worker.label_a=CrewReady; worker.sprite=idle_sprite(worker); }
        return;
    }
    if (worker.label_a==CrewWhistle || worker.label_a==CrewCut || worker.label_a==CrewStrike) {
        if (worker.cell!=worker.point_a || worker.vitals.rooted>0) { interrupt_mine_worker(worker); return; }
        if (worker.timer_a>0) return;
        if (worker.label_a==CrewWhistle) {
            worker.label_b=1; worker.timer_b=360; worker.label_a=CrewReady;
            worker.sprite=idle_sprite(worker);
            const auto heard=audible_cells(game,worker.cell,8);
            for (Entity& follower:game.entities)
                if (follower.kind==EntityKind::Pickhand && follower.health>0 && follower.entity_a==worker.entity_a &&
                    std::find(heard.begin(),heard.end(),follower.cell)!=heard.end()) {
                    follower.label_b=1;
                    if (follower.label_c==PlayerWorkOrder) { follower.label_c=0; follower.timer_c=0; follower.point_c={}; }
                }
            emit_sound(game,SoundId::CrewMarch,worker.cell); make_noise(game,worker.cell,8);
            return;
        }
        if (worker.label_a==CrewCut) {
            const Tile& tile=game.stage.at_or_border(worker.point_b);
            if (crew_diggable(tile)) hit_terrain(game,worker.point_b,worker.cell,25,1,TileImpact::Strike);
            else if (prop_blocks(tile.prop)) hit_prop(game,worker.point_b,25,worker.cell);
            emit_sound(game,(worker.point_b.x+worker.point_b.y)%2==0 ? SoundId::CrewPick : SoundId::CrewPick2,worker.point_b); make_noise(game,worker.point_b,8);
        } else resolve_enemy_attack(game,slot,worker.kind==EntityKind::Pickhand ? 18 : 14,SoundId::CrewSwing);
        rest(worker,worker.label_a==CrewCut ? 12 : 30);
        worker.sprite=worker.kind==EntityKind::Pickhand ? Sprite::PickhandSwing : Sprite::ForemanSwing;
        return;
    }
    const Entity* threat=get_entity(game,worker.entity_b);
    if (worker.attack_wait>0 && threat && threat->health>0 &&
        (threat->kind!=EntityKind::Player || (threat->owner>=0 && threat->owner<4 && game.run.online[static_cast<std::size_t>(threat->owner)]))) {
        if (distance(worker.cell,threat->cell)==1) { begin(game,worker,threat->cell,false); return; }
        if (worker.kind==EntityKind::Pickhand) advance_worker(game,slot,threat->cell);
        else if (worker.move_wait==0) {
            if (const auto next=next_route_cell(game,slot,threat->cell,512)) willing_step(game,slot,*next);
            else worker.move_wait=18;
        }
        return;
    }
    worker.entity_b={}; worker.attack_wait=0;
    if (worker.kind==EntityKind::ShiftForeman) { direct_crew(game,slot); return; }
    if (worker.label_c==PlayerWorkOrder && worker.timer_c>0) {
        advance_worker(game,slot,worker.point_c);
        return;
    }
    const Entity* leader=get_entity(game,worker.entity_a);
    if (!leader || leader->kind!=EntityKind::ShiftForeman || leader->health<=0) {
        if (!step_hearing(game,slot) && worker.move_wait==0) yield_lane(game,slot);
        return;
    }
    if (worker.label_b==1 && leader->label_b==1 && leader->label_a!=CrewWhistle && leader->attack_wait==0 &&
        leader->sleep_ticks==0 && leader->stun_ticks==0) {
        const Cell direction=leader->point_b,side{-direction.y,direction.x};
        advance_worker(game,slot,leader->cell+Cell{direction.x*4+side.x*worker.counter_b,direction.y*4+side.y*worker.counter_b});
    } else if (worker.move_wait==0 && distance(worker.cell,leader->cell)<=1) yield_lane(game,slot);
}
