#include "tar_spit.hpp"
#include "../combat/parry.hpp"
#include "../surfaces/interaction.hpp"
#include <cstdlib>

// SLOTS: point_a origin, point_b immutable landing cell; entity_a shooter.
// counter_a steps left, attack_interval total, timer_b six-tick step, timer_a life.
namespace {
void finish(Game& game,int slot,bool splash) {
    const auto& shot=game.entities[static_cast<std::size_t>(slot)];
    if (splash) {pour_surface(game,shot.cell,LiquidKind::Tar,600);emit_sound(game,SoundId::TarSplash,shot.cell);}
    remove_entity(game,{slot,shot.generation});
}
}
Cell tar_spit_next(const Entity& shot) {
    const Cell delta=shot.point_b-shot.point_a;
    const int dx=std::abs(delta.x),dy=std::abs(delta.y);
    const int x=std::abs(shot.cell.x-shot.point_a.x),y=std::abs(shot.cell.y-shot.point_a.y);
    // Integer supercover walk: no diagonal wall skips and no reacquiring a moving target.
    if ((1+2*x)*dy<(1+2*y)*dx) return shot.cell+Cell{delta.x>0 ? 1 : -1,0};
    return shot.cell+Cell{0,delta.y>0 ? 1 : -1};
}
bool launch_tar_spit(Game& game,int owner,Cell target) {
    const auto& actor=game.entities[static_cast<std::size_t>(owner)];
    const int length=distance(actor.cell,target);
    if (length<1 || length>9) return false;
    auto* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,actor.cell));
    if (!shot) return false;
    shot->label_a=static_cast<int>(ProjectileKind::TarSpit);shot->sprite=Sprite::TarGob;
    shot->point_a=actor.cell;shot->point_b=target;shot->entity_a={owner,actor.generation};
    shot->counter_a=shot->attack_interval=length;shot->counter_b=6;
    shot->timer_a=length*6+1;shot->timer_b=6;shot->facing=tar_spit_next(*shot)-shot->cell;
    return true;
}
void step_tar_spit(Game& game,int slot) {
    auto& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.counter_a==0 || shot.cell==shot.point_b || shot.timer_a==0) {finish(game,slot,true);return;}
    if (shot.timer_b>0) return;
    const Cell next=tar_spit_next(shot);
    if (projectile_blocked(game,next,false,true)) {finish(game,slot,true);return;}
    shot.facing=next-shot.cell;shot.cell=next;--shot.counter_a;shot.timer_b=6;
    const int victim=entity_at(game,next,true);
    if (victim>=0 && Handle{victim,game.entities[static_cast<std::size_t>(victim)].generation}!=shot.entity_a) {
        if (parry_ranged_hit(game,victim,shot.facing)) {finish(game,slot,false);return;}
        damage_entity(game,victim,6,shot.cell-shot.facing,true,shot.entity_a);
        finish(game,slot,true);return;
    }
    if (shot.counter_a==0) {finish(game,slot,true);return;}
    shot.facing=tar_spit_next(shot)-shot.cell;
}
bool valid_tar_spit(const Entity& shot) {
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::TarSpit)) return true;
    return shot.attack_interval>=1 && shot.attack_interval<=9 && shot.counter_a>=0 && shot.counter_a<=shot.attack_interval &&
        shot.counter_b==6 && shot.timer_a<=55 && shot.timer_b<=6 &&
        distance(shot.point_a,shot.point_b)==shot.attack_interval && distance(shot.cell,shot.point_b)==shot.counter_a &&
        distance(shot.point_a,shot.cell)+shot.counter_a==shot.attack_interval;
}
