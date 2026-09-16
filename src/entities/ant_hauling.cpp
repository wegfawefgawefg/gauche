#include "ant_hauling.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../combat/shove.hpp"
#include <algorithm>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool live(const Entity* e,EntityKind kind) {return e && e->kind==kind && e->health>0;}
bool disabled(const Entity& e) {return e.sleep_ticks || e.stun_ticks || e.freeze_ticks || e.vitals.rooted || e.toss.ticks;}
Cell stance(const Entity& load,int side) {
    const Cell across{-load.facing.y,load.facing.x};
    return load.cell+load.facing+Cell{across.x*(side ? 1 : -1),across.y*(side ? 1 : -1)};
}
void rest(Entity& load,int ticks) {load.label_a=LoadRest;load.timer_a=ticks;load.timer_b=0;}
std::array<int,2> crew(const Game& game,int slot) {
    std::array<int,2> result{-1,-1};const Handle owner{slot,game.entities[static_cast<std::size_t>(slot)].generation};
    for (int i=0;i<max_entities;++i) {
        const auto& a=game.entities[static_cast<std::size_t>(i)];
        if (a.kind==EntityKind::Ant && a.counter_a==AntPuller && a.entity_a==owner && a.health>0 && a.label_b>=0 && a.label_b<2)
            result[static_cast<std::size_t>(a.label_b)]=i;
    }
    return result;
}
bool working(const Game& game,const Entity& ant) {
    const auto* threat=ant.timer_b ? get_entity(game,ant.entity_b) : nullptr;
    return !disabled(ant) && ant.label_a==AntWorking && (!threat || threat->health<=0);
}
bool free_cell(const Game& game,Cell cell,int own=-1) {
    const auto* tile=game.stage.at(cell);if (!tile || !walkable(*tile)) return false;
    const int occupant=entity_at(game,cell,true);return occupant<0 || occupant==own;
}
}

// LOAD: entity_a nest, entity_b whistle captain; counter_a sugar units (0..12);
// label_a phase, timer_a warning/recovery, timer_b assembly deadline;
// point_a committed origin, point_b next cell. Two AntPullers reference this
// load in entity_a; label_b selects their left/right rope. No raw slot ownership.
void init_ant_load(Entity& load) {
    load.health=load.max_health=75;load.impassable=load.hard_blocker=true;load.sprite=Sprite::AntLoad;
}
Handle ant_colony(const Game& game,const Entity& member) {
    if (member.kind==EntityKind::AntNest)
        return {static_cast<int>(&member-game.entities.data()),member.generation};
    if (member.kind==EntityKind::Ant && member.counter_a==AntPuller) {
        const auto* load=get_entity(game,member.entity_a);
        return load && load->kind==EntityKind::AntLoad ? load->entity_a : Handle{};
    }
    return member.entity_a;
}
void interrupt_ant_load(Entity& load) {if (load.kind==EntityKind::AntLoad) rest(load,60);}

std::vector<Cell> ant_load_route(const Game& game,Cell origin,Cell target,int own_slot) {
    const auto& stage=game.stage;const auto index=[&](Cell c){return static_cast<std::size_t>(c.y*stage.width+c.x);};
    if (!stage.in_bounds(origin) || !stage.in_bounds(target)) return {};
    std::vector<std::uint8_t> ground(stage.tiles.size());
    for (std::size_t i=0;i<ground.size();++i) {
        const auto& tile=stage.tiles[i];ground[i]=walkable(tile) && tile.kind!=TileKind::Lava && tile.kind!=TileKind::ShallowWater && tile.kind!=TileKind::Spring && tile.kind!=TileKind::IceHole;
    }
    for (int i=0;i<max_entities;++i) {
        const auto& e=game.entities[static_cast<std::size_t>(i)];
        const bool other_ant=e.kind==EntityKind::Ant && (e.counter_a==AntCaptain ||
            (own_slot>=0 && e.entity_a!=Handle{own_slot,game.entities[static_cast<std::size_t>(own_slot)].generation}));
        if (i!=own_slot && e.kind!=EntityKind::None && (e.hard_blocker || other_ant) && e.impassable && stage.in_bounds(e.cell)) ground[index(e.cell)]=0;
    }
    const auto wide=[&](Cell c) {
        for (int y=-1;y<=1;++y) for(int x=-1;x<=1;++x) {const Cell at=c+Cell{x,y};if (!stage.in_bounds(at) || !ground[index(at)]) return false;}
        return true;
    };
    if (!wide(origin)) return {};
    std::vector<int> parent(ground.size(),-1);std::vector<Cell> queue{origin};parent[index(origin)]=static_cast<int>(index(origin));
    for (std::size_t next=0;next<queue.size() && next<4096;++next) {
        const Cell at=queue[next];
        if (distance(at,target)<=2 && clear_attack_sight(game,at,target)) {
            std::vector<Cell> path;
            for (Cell c=at;;) {path.push_back(c);if(c==origin)break;const int p=parent[index(c)];c={p%stage.width,p/stage.width};}
            std::reverse(path.begin(),path.end());return path;
        }
        for (Cell d:directions) {
            const Cell c=at+d;if (!stage.in_bounds(c) || parent[index(c)]>=0 || !wide(c)) continue;
            parent[index(c)]=static_cast<int>(index(at));queue.push_back(c);
        }
    }
    return {};
}
void step_ant_puller(Game& game,int slot) {
    auto& ant=game.entities[static_cast<std::size_t>(slot)];auto* load=get_entity(game,ant.entity_a);
    if (!live(load,EntityKind::AntLoad)) {if (!ant.move_wait && game.tick%60==0) wander(game,slot);return;}
    if (load->label_a!=LoadAlign && load->label_a!=LoadWarn) {ant.sprite=Sprite::AntPuller;return;}
    const Cell goal=stance(*load,ant.label_b);
    if (ant.cell!=goal && !ant.move_wait) {
        if (const auto next=next_route_cell(game,slot,goal,2048)) willing_step(game,slot,*next);
        else ant.move_wait=30;
    }
    ant.facing=load->facing;
    ant.sprite=load->label_a==LoadWarn ? Sprite::AntPullerTell : ant.move_wait && (game.tick/10)%2 ? Sprite::AntPullerWalk : Sprite::AntPuller;
}
void step_ant_load(Game& game,int slot) {
    auto& load=game.entities[static_cast<std::size_t>(slot)];load.sprite=load.counter_a ? Sprite::AntLoadFull : Sprite::AntLoad;
    auto* nest=get_entity(game,load.entity_a);
    if (!live(nest,EntityKind::AntNest) || disabled(load) || nest->timer_a) {rest(load,60);return;}
    const auto team=crew(game,slot);
    if (team[0]<0 || team[1]<0) {rest(load,60);return;}
    for(int i:team) if (!working(game,game.entities[static_cast<std::size_t>(i)])) {rest(load,60);return;}
    if (load.label_a==LoadRest) {if(load.timer_a)return;load.label_a=LoadIdle;}
    if (load.label_a==LoadIdle) {
        auto* source=get_entity(game,nest->entity_b);const auto* target=load.counter_a ? nest : source;
        if (!target || (!load.counter_a && (!live(source,EntityKind::AntSugar) || !source->counter_a))) {rest(load,120);return;}
        if (distance(load.cell,target->cell)<=2 && clear_attack_sight(game,load.cell,target->cell)) {
            if (load.counter_a) {nest->counter_a=std::min(4096,nest->counter_a+load.counter_a);load.counter_a=0;}
            else {load.counter_a=std::min(12,source->counter_a);source->counter_a-=load.counter_a;step_ant_sugar(*source);}
            emit_sound(game,SoundId::CartCargo,load.cell);rest(load,60);return;
        }
        const auto path=ant_load_route(game,load.cell,target->cell,slot);
        if (path.size()<2) {rest(load,90);return;}
        load.point_a=load.cell;load.point_b=path[1];load.facing=path[1]-load.cell;
        load.label_a=LoadAlign;load.timer_b=240;
    }
    if (load.cell!=load.point_a) {rest(load,60);return;}
    bool aligned=true;
    for(int side=0;side<2;++side) aligned&=game.entities[static_cast<std::size_t>(team[static_cast<std::size_t>(side)])].cell==stance(load,side);
    if (load.label_a==LoadAlign) {
        if (!load.timer_b) {rest(load,90);return;}
        if (!aligned || !free_cell(game,stance(load,0)+load.facing,team[0]) || !free_cell(game,stance(load,1)+load.facing,team[1])) return;
        load.label_a=LoadWarn;load.timer_a=60;emit_sound(game,SoundId::PorterHeave,load.cell);return;
    }
    if (!aligned || !free_cell(game,stance(load,0)+load.facing,team[0]) || !free_cell(game,stance(load,1)+load.facing,team[1])) {rest(load,60);return;}
    if (load.timer_a) return;
    // Warned center-cell impact; survivors are shoved, never overlapped by cargo.
    const int victim=entity_at(game,load.point_b,true);
    if(victim>=0) {
        const auto& other=game.entities[static_cast<std::size_t>(victim)];
        if (other.hard_blocker || (other.kind==EntityKind::Ant && ant_colony(game,other)==load.entity_a)) {rest(load,90);return;}
        damage_entity(game,victim,48,load.cell,false,{slot,load.generation});
        shove_actor(game,victim,load.facing,load.cell);emit_sound(game,SoundId::CartImpact,load.point_b);
    }
    if (!free_cell(game,load.point_b)) {rest(load,120);return;}
    // The formation was checked as a whole. Normal movement applies floor contacts.
    for(int i:team) move_entity(game,i,game.entities[static_cast<std::size_t>(i)].cell+load.facing,false);
    load.cell=load.point_b;emit_sound(game,SoundId::CartRoll,load.cell);
    const auto* captain=get_entity(game,load.entity_b);
    const bool called=live(captain,EntityKind::Ant) && captain->counter_a==AntCaptain && captain->entity_a==load.entity_a && nest->timer_b;
    rest(load,called ? 12 : 36); // A whistle quickens recovery, never the warning.
}
