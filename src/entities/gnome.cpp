#include "gnome.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../projectiles/projectile.hpp"
#include <algorithm>

namespace {
constexpr Cell sides[]{{0,1},{1,0},{-1,0},{0,-1}};
bool live_house(const Entity* e) {return e && e->kind==EntityKind::GnomeHouse && e->health>0;}
void rest(Entity& g,int ticks) {g.label_a=GnomeRest;g.timer_a=ticks;g.sprite=gnome_pose(g);g.self_light={};}
bool exit_home(Game& game,int slot,const Entity* home) {
    auto& g=game.entities[static_cast<std::size_t>(slot)];
    const Cell origin=home ? home->cell : g.cell;
    for (Cell d:{Cell{0,1},{1,0},{-1,0},{0,-1},{0,0}}) {
        if (live_house(home) && d!=Cell{0,1}) continue;
        const Cell at=origin+d;const auto* tile=game.stage.at(at);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava) continue;
        bool blocked=false;
        for (const auto& e:game.entities) if (&e!=&g && e.kind!=EntityKind::None && e.health>0 && e.impassable && e.cell==at) blocked=true;
        if (blocked) continue;
        g.cell=at;g.impassable=true;rest(g,30);emit_sound(game,SoundId::GnomeDoor,origin);return true;
    }
    return false; // Wait for a legal doorway; never teleport through enclosing walls.
}
bool lane(const Game& game,Cell a,Cell b) {
    return distance(a,b)>=2 && distance(a,b)<=7 && (a.x==b.x || a.y==b.y) && clear_shot_sight(game,a,b);
}
void seek_lane(Game& game,int slot,Cell target) {
    auto& g=game.entities[static_cast<std::size_t>(slot)];if(g.move_wait)return;
    // Try short stand-off positions on the target's axes, then use normal pursuit.
    std::optional<Cell> first;int score=10000;
    for (Cell d:sides) for(int gap:{3,5}) {
        const Cell at=target+Cell{d.x*gap,d.y*gap};const int travel=distance(g.cell,at);
        if (travel>=score || !lane(game,at,target) || entity_at(game,at,true)>=0) continue;
        if (auto next=next_route_cell(game,slot,at,256)) {first=next;score=travel;}
    }
    if(first)willing_step(game,slot,*first);else pursue(game,slot,target);
}
}
// GNOME: entity_a home, entity_b threat; counter_a role, counter_b finite arrows;
// label_a phase; timer_a action/hiding, timer_b threat memory; point_a/b committed
// attack origin/target. HOUSE: entity_a settlement identity (first house handle,
// retained even after its death), entity_b attacker; timer_a alarm, counter_a cap
// color. Hidden residents remain real entities, nonblocking inside their home.
Sprite gnome_pose(const Entity& g,int phase) {return static_cast<Sprite>(static_cast<int>(Sprite::GnomeStick)+g.counter_a*4+phase);}
bool hidden_gnome(const Entity& g) {return g.kind==EntityKind::Gnome && g.label_a==GnomeHidden;}
void set_gnome_role(Entity& g,GnomeRole role) {
    g.counter_a=role;g.counter_b=role==GnomeBow ? 6 : role==GnomeCrossbow ? 4 : 0;
    g.health=g.max_health=role==GnomeRider ? 38 : 16;g.move_interval=role==GnomeRider ? 10 : 14;g.sprite=gnome_pose(g);
}
void init_gnome(Entity& g) {g.impassable=true;set_gnome_role(g,GnomeStick);}
void init_gnome_house(Entity& house) {house.sprite=Sprite::GnomeHouse;house.health=house.max_health=60;house.impassable=house.hard_blocker=true;}
void gnome_timers(Entity& g) {
    if(g.kind==EntityKind::Gnome && g.label_a==GnomeTell && (g.stun_ticks || g.sleep_ticks || g.freeze_ticks || g.vitals.rooted || g.toss.ticks))rest(g,45);
}
void hurt_gnome_settlement(Game& game,int slot,Cell from) {
    auto& victim=game.entities[static_cast<std::size_t>(slot)];
    if(victim.kind!=EntityKind::Gnome && victim.kind!=EntityKind::GnomeHouse)return;
    if(victim.kind==EntityKind::Gnome && victim.label_a==GnomeTell)rest(victim,45);
    const int attacker=entity_at(game,from,true);if(attacker<0 || attacker==slot)return;
    const auto& threat=game.entities[static_cast<std::size_t>(attacker)];
    const auto* home=victim.kind==EntityKind::GnomeHouse ? &victim : get_entity(game,victim.entity_a);
    const Handle identity=home && home->kind==EntityKind::GnomeHouse ? home->entity_a : Handle{};
    if(threat.kind==EntityKind::GnomeHouse && identity.slot>=0 && threat.entity_a==identity)return;
    if(threat.kind==EntityKind::Gnome) {
        const auto* other=get_entity(game,threat.entity_a);
        if(identity.slot>=0 && other && other->kind==EntityKind::GnomeHouse && other->entity_a==identity)return;
    }
    const Handle who{attacker,threat.generation};
    if(victim.kind==EntityKind::Gnome){victim.entity_b=who;victim.timer_b=1200;}
    if(identity.slot<0)return;
    for(auto& e:game.entities) {
        if(e.kind==EntityKind::GnomeHouse && e.entity_a==identity){e.entity_b=who;e.timer_a=1200;}
        if(e.kind==EntityKind::Gnome){const auto* house=get_entity(game,e.entity_a);if(house && house->kind==EntityKind::GnomeHouse && house->entity_a==identity){e.entity_b=who;e.timer_b=1200;}}
    }
}
void step_gnome_house(Game& game,int slot) {
    auto& house=game.entities[static_cast<std::size_t>(slot)];if(!house.timer_a)house.entity_b={};
    bool inside=false;for(const auto& e:game.entities)if(hidden_gnome(e) && e.entity_a==Handle{slot,house.generation} && e.health>0)inside=true;
    house.sprite=static_cast<Sprite>(static_cast<int>(Sprite::GnomeHouse)+house.counter_a*2+(inside?1:0));
}
void step_gnome(Game& game,int slot) {
    auto& g=game.entities[static_cast<std::size_t>(slot)];auto* home=get_entity(game,g.entity_a);
    const bool housed=live_house(home);
    if(housed && home->timer_a){g.entity_b=home->entity_b;g.timer_b=1200;}
    const auto* threat=g.timer_b ? get_entity(game,g.entity_b) : nullptr;
    const bool angry=threat && threat->health>0;
    if(hidden_gnome(g)) {
        const bool burning=housed && (home->burn_ticks || home->scorch_ticks || game.stage.at_or_border(home->cell).surface.fire_ticks);
        if(!housed || angry || burning || (!g.timer_a && nearest_player(game,g.cell,8)<0))exit_home(game,slot,home);
        return;
    }
    if(g.label_a==GnomeTell) {
        if(g.cell!=g.point_a || g.toss.ticks){rest(g,45);return;}
        if(g.timer_a)return;
        const bool ranged=(g.counter_a==GnomeBow || g.counter_a==GnomeCrossbow) && g.counter_b>0 && distance(g.point_a,g.point_b)>1;
        if(ranged) {
            const ItemKind weapon=g.counter_a==GnomeBow ? ItemKind::Bow : ItemKind::Crossbow;
            if(launch_projectile(game,slot,make_item(weapon),g.facing,7,g.counter_a==GnomeBow ? 10 : 16))--g.counter_b;
            rest(g,g.counter_a==GnomeBow ? 90 : 120);
        } else {resolve_enemy_attack(game,slot,g.counter_a==GnomeRider ? 9 : 5,SoundId::BumpWood);rest(g,48);}
        g.sprite=gnome_pose(g,3);return;
    }
    if(g.label_a==GnomeRest && g.timer_a)return;
    if(angry && distance(g.cell,threat->cell)<22) {
        g.label_a=GnomeCalm;const int gap=distance(g.cell,threat->cell);
        const int melee=g.counter_a==GnomeRider ? 2 : 1;
        const bool ranged=g.counter_b>0 && (g.counter_a==GnomeBow || g.counter_a==GnomeCrossbow);
        if((gap<=melee && clear_attack_sight(game,g.cell,threat->cell)) || (ranged && lane(game,g.cell,threat->cell))) {
            g.point_a=g.cell;g.point_b=threat->cell;g.facing=cardinal_toward(g.cell,threat->cell,g.facing);
            g.label_a=GnomeTell;g.timer_a=ranged && gap>1 ? 45 : 36;g.sprite=gnome_pose(g,2);g.self_light={22,12,5};
            emit_sound(game,ranged && gap>1 ? SoundId::BowDraw : SoundId::StickWindup,g.cell);
        } else {
            if(ranged)seek_lane(game,slot,threat->cell);else pursue(game,slot,threat->cell);
            g.sprite=gnome_pose(g,g.move_wait && (game.tick/8)%2 ? 1 : 0);
        }
        return;
    }
    const int player=nearest_player(game,g.cell,6);
    if(player>=0 && clear_sight(game,g.cell,game.entities[static_cast<std::size_t>(player)].cell)) {
        if(g.label_a!=GnomeFlee){g.label_a=GnomeFlee;emit_sound(game,SoundId::GnomeSqueal,g.cell);}
        if(housed && !home->burn_ticks && !home->scorch_ticks) {
            if(g.cell==home->cell+Cell{0,1}) {g.cell=home->cell;g.impassable=false;g.label_a=GnomeHidden;g.timer_a=240;emit_sound(game,SoundId::GnomeDoor,g.cell);}
            else pursue(game,slot,home->cell+Cell{0,1});
        } else if(!g.move_wait)flee(game,slot,game.entities[static_cast<std::size_t>(player)].cell);
    } else {
        g.label_a=GnomeCalm;
        if(housed && distance(g.cell,home->cell)>4)pursue(game,slot,home->cell);
        else if((game.tick+static_cast<std::uint64_t>(slot)*13)%75==0)wander(game,slot);
    }
    g.sprite=gnome_pose(g,g.move_wait && (game.tick/8)%2 ? 1 : 0);
}
