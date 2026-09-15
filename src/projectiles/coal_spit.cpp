#include "coal_spit.hpp"
#include "projectile.hpp"
#include "../combat/parry.hpp"
#include "../items/fire.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../props/interaction.hpp"

namespace {
void cool(Entity& shot) {
    shot.label_b=0; shot.counter_b=4; shot.sprite=Sprite::CoalSpitCold; shot.light={};
}
void finish(Game& game,int slot,Cell cell) {
    emit_sound(game,SoundId::StokerImpact,cell);
    remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
}
void ignite(Game& game,const Entity& shot,Cell cell) {
    if (shot.label_b==1) ignite_surface(game,cell);
}
}
bool douse_coal_spit(Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::CoalSpit) || actor.label_b==0) return false;
    cool(actor); return true;
}
bool launch_coal_spit(Game& game,int owner,Cell direction,bool hot) {
    const auto& actor=game.entities[static_cast<std::size_t>(owner)];
    auto* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,actor.cell));
    if (!shot) return false;
    shot->label_a=static_cast<int>(ProjectileKind::CoalSpit); shot->label_b=hot ? 1 : 0;
    shot->sprite=Sprite::CoalSpit; shot->counter_a=shot->attack_interval=8;
    shot->counter_b=12; shot->timer_b=5; shot->timer_a=41; shot->timer_c=164;
    shot->point_a=actor.cell; shot->entity_a={owner,actor.generation}; shot->facing=direction;
    shot->light={2,340,{255,148,51}};
    if (!hot) cool(*shot);
    return true;
}
void step_coal_spit(Game& game,int slot) {
    auto& shot=game.entities[static_cast<std::size_t>(slot)];
    if (surface_wet(game.stage.at_or_border(shot.cell))) cool(shot);
    if (shot.timer_a==0 || shot.timer_c==0 || shot.counter_a==0) { ignite(game,shot,shot.cell); finish(game,slot,shot.cell); return; }
    if (shot.timer_b>0) return;
    const Cell next=shot.cell+shot.facing;
    if (surface_wet(game.stage.at_or_border(next))) cool(shot);
    const bool blocked=projectile_blocked(game,next);
    if (blocked) {
        hit_prop(game,next,shot.counter_b,shot.cell);
        hit_terrain(game,next,shot.cell,shot.counter_b,0);
        ignite(game,shot,next); finish(game,slot,next); return;
    }
    const int victim=entity_at(game,next,true);
    if (victim>=0) {
        auto& target=game.entities[static_cast<std::size_t>(victim)];
        if (Handle{victim,target.generation}!=shot.entity_a) {
            if (parry_ranged_hit(game,victim,shot.facing)) { reflect_projectile(shot,target,victim); return; }
            const int hp=target.health;
            damage_entity(game,victim,shot.counter_b,shot.cell,true,shot.entity_a);
            if (shot.label_b==1 && target.health<hp) ignite_struck_actor(game,victim);
            ignite(game,shot,next); finish(game,slot,next); return;
        }
    }
    shot.cell=next; --shot.counter_a; shot.timer_b=5;
    if (shot.counter_a==0) { ignite(game,shot,next); finish(game,slot,next); }
}
