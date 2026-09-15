#include "rivet_gunner.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"
#include "../projectiles/projectile.hpp"
#include <array>
#include <vector>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
void reload(Game& game,Entity& actor) {
    actor.label_a=GunnerReload; actor.timer_a=90; actor.counter_a=0;
    actor.sprite=Sprite::GunnerReload; emit_sound(game,SoundId::RivetReload,actor.cell);
}
bool lane(const Game& game,Cell from,Cell to) {
    const Cell delta=to-from;
    return distance(from,to)>0 && distance(from,to)<=8 && (delta.x==0 || delta.y==0) && clear_shot_sight(game,from,to);
}
// A bounded local search finds reachable firing lanes. One grate immediately
// ahead is useful cover; the gunner never treats a grate as a walkable shortcut.
std::optional<Cell> firing_step(const Game& game,int slot,Cell target) {
    const Cell origin=game.entities[static_cast<std::size_t>(slot)].cell;
    struct Node { Cell cell,first; int steps; };
    std::vector<Node> queue{{origin,origin,0}};
    std::array<bool,169> seen{};
    std::vector<bool> occupied(game.stage.tiles.size(),false);
    for (const Entity& actor:game.entities)
        if (actor.impassable && actor.health>0 && game.stage.at(actor.cell))
            occupied[static_cast<std::size_t>(actor.cell.y*game.stage.width+actor.cell.x)]=true;
    std::optional<Cell> best; int best_score=-1000;
    for (std::size_t i=0;i<queue.size();++i) {
        const Node n=queue[i];
        if (n.steps>0 && lane(game,n.cell,target)) {
            const Cell facing=cardinal_toward(n.cell,target,{});
            const auto& cover=game.stage.at_or_border(n.cell+facing).prop;
            const int score=20-n.steps*3+(distance(n.cell,target)>=3 ? 4 : 0)+
                (prop_blocks(cover) && prop_shoot_through(cover) ? 5 : 0);
            if (score>best_score) { best_score=score; best=n.first; }
        }
        if (n.steps==6) continue;
        for (Cell side:sides) {
            const Cell next=n.cell+side,offset=next-origin;
            if (distance(next,origin)>6) continue;
            const auto index=static_cast<std::size_t>((offset.y+6)*13+offset.x+6);
            if (seen[index]) continue;
            seen[index]=true;
            const Tile* tile=game.stage.at(next);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava ||
                occupied[static_cast<std::size_t>(next.y*game.stage.width+next.x)]) continue;
            queue.push_back({next,n.steps==0 ? next : n.first,n.steps+1});
        }
    }
    return best;
}
}
// SLOTS: label_a phase, timer_a phase/shot clock, counter_a remaining burst
// rounds; point_a brace origin, point_b committed direction. c slots = hearing.
// The enemy carries a replenishing feed; the dropped player's gun uses finite ammo.
void init_rivet_gunner(Entity& actor) {
    actor.sprite=Sprite::RivetGunner; actor.health=actor.max_health=64;
    actor.impassable=true; actor.move_interval=16;
}
void interrupt_rivet_gunner(Entity& actor) {
    if (actor.kind!=EntityKind::RivetGunner || (actor.label_a!=GunnerBrace && actor.label_a!=GunnerBurst)) return;
    actor.label_a=GunnerReload; actor.timer_a=90; actor.counter_a=0; actor.sprite=Sprite::GunnerReload;
}
void step_rivet_gunner(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.label_a==GunnerReload) {
        if (actor.timer_a==0) { actor.label_a=GunnerReady; actor.sprite=Sprite::RivetGunner; }
        return;
    }
    if (actor.label_a==GunnerBrace || actor.label_a==GunnerBurst) {
        if (actor.cell!=actor.point_a || actor.vitals.rooted>0) { interrupt_rivet_gunner(actor); return; }
        if (actor.timer_a>0) return;
        // The lane stays committed even if the target moves or cover appears.
        if (!launch_projectile(game,slot,make_item(ItemKind::RivetGun),actor.point_b,8)) { reload(game,actor); return; }
        make_noise(game,actor.cell,8); actor.use_flash=6;
        if (--actor.counter_a==0) { reload(game,actor); return; }
        actor.label_a=GunnerBurst; actor.timer_a=9; actor.sprite=Sprite::GunnerFire; return;
    }
    const auto target=enemy_target(game,actor.cell,10);
    if (!target) { if (!step_hearing(game,slot)) wander(game,slot); return; }
    if (lane(game,actor.cell,target->cell)) {
        actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.point_a=actor.cell; actor.point_b=actor.facing;
        actor.label_a=GunnerBrace; actor.timer_a=30; actor.counter_a=3;
        actor.sprite=Sprite::GunnerBrace; emit_sound(game,SoundId::GunnerBrace,actor.cell); return;
    }
    if (actor.move_wait>0) return;
    if (const auto step=firing_step(game,slot,target->cell)) willing_step(game,slot,*step);
    else approach(game,slot,target->cell);
}
bool valid_rivet_gunner(const Entity& actor) {
    if (actor.kind!=EntityKind::RivetGunner) return true;
    if (actor.label_a<GunnerReady || actor.label_a>GunnerReload || actor.timer_a>90 || actor.counter_a<0 || actor.counter_a>3) return false;
    return (actor.label_a!=GunnerBrace && actor.label_a!=GunnerBurst) || (actor.counter_a>0 && distance({},actor.point_b)==1);
}
