#include "../items/glow_slag.hpp"
#include "furnace_moth.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../items/fire.hpp"
#include "../items/flare.hpp"
#include "../items/heat_siphon.hpp"
#include "../lighting/shape.hpp"
#include "../props/growth.hpp"
#include "../props/stove.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
void rest(Entity& moth,int ticks=60) {
    moth.label_a=FurnaceRest;moth.timer_a=ticks;moth.move_interval=14;moth.label_b=0;
    moth.sprite=moth.counter_a>0 ? Sprite::FurnaceMothHot : Sprite::FurnaceMoth;
}
bool exposed_fuel(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile) return false;
    if (prop_has_flame(tile->prop) || tile->surface.fire_ticks>0) return true;
    for (const Entity& e:game.entities) {
        if (e.kind==EntityKind::None || e.cell!=cell || (e.health<=0 && e.kind!=EntityKind::GroundItem)) continue;
        if ((e.kind==EntityKind::Campfire && e.fire_tramples<5) || burning_flare(e) ||
            e.burn_ticks>0 || e.scorch_ticks>0) return true;
        const Item& item=e.kind==EntityKind::GroundItem ? e.ground_item : *e.inventory.held();
        if (item.flame_ticks>0 || glowing_slag(item)) return true;
    }
    return false;
}
void find_light(Game& game,Entity& moth) {
    int best=0;moth.entity_a={};moth.point_a=moth.cell;
    const auto consider=[&](Cell cell,LightEmitter light,Handle source,Cell facing=Cell{1,0}) {
        const int gap=distance(moth.cell,cell);
        if (gap>8 || gap>light.radius || light.strength<=0 || !clear_attack_sight(game,moth.cell,cell)) return;
        const int score=light.strength*light_direction_factor(light.shape,facing,moth.cell-cell)/(1000*(gap+1));
        if (score>best) {best=score;moth.point_a=cell;moth.entity_a=source;}
    };
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& e=game.entities[static_cast<std::size_t>(slot)];
        if (e.kind==EntityKind::None || e.kind==EntityKind::FurnaceMoth || (e.health<=0 && e.kind!=EntityKind::GroundItem)) continue;
        if (e.kind==EntityKind::Player && e.owner>=0 && has_player(game, e.owner) && !player_state(game, e.owner).online) continue;
        const Handle handle{slot,e.generation};
        consider(e.cell,e.light,handle,e.facing);
        consider(e.cell,item_light(e.kind==EntityKind::GroundItem ? e.ground_item : *e.inventory.held()),handle,e.facing);
    }
    for (int y=std::max(0,moth.cell.y-8);y<=std::min(game.stage.height-1,moth.cell.y+8);++y)
        for (int x=std::max(0,moth.cell.x-8);x<=std::min(game.stage.width-1,moth.cell.x+8);++x) {
            const Cell cell{x,y};const Tile& tile=*game.stage.at(cell);
            consider(cell,prop_light(tile.prop),{});
            if (tile.surface.fire_ticks>0) consider(cell,{3,500,{240,135,55}},{});
        }
    moth.timer_b=24;
}
void strike(Game& game,int slot,Cell cell) {
    Entity& moth=game.entities[static_cast<std::size_t>(slot)];
    const int heat=moth.label_b,damage=4+heat/30;
    const Handle source{slot,moth.generation};const Cell origin=moth.cell;
    const bool covered=prop_blocks(game.stage.at_or_border(cell).prop);
    hit_prop(game,cell,damage,origin);
    ignite_surface(game,cell);
    for (int i=0;i<max_entities;++i) {
        Entity& victim=game.entities[static_cast<std::size_t>(i)];
        if (covered || i==slot || victim.cell!=cell || victim.health<=0 || !victim.impassable) continue;
        const int before=victim.health;
        damage_entity(game,i,damage,origin,true,source);
        if (victim.health>0 && victim.health<before) {
            const int old=victim.scorch_ticks;ignite_struck_actor(game,i);
            if (victim.scorch_ticks>old) victim.scorch_ticks=std::max(old,std::min(180,heat));
        }
    }
    emit_sound(game,SoundId::FurnaceMothFlare,cell);
    if (moth.health>0) rest(moth,90);
}
}

// SLOTS: counter_a stored heat (0..300); counter_b orbit index/dive steps left;
// label_a phase, label_b heat committed to a dive; timer_a phase, timer_b search.
// point_a chosen light/fuel cell; entity_a source handle; point_b committed origin
// or expected dive cell. Facing commits the dive axis. c slots remain unused.
void init_furnace_moth(Entity& moth) {
    moth.health=moth.max_health=18;moth.impassable=true;moth.move_interval=14;
    moth.counter_a=180;moth.point_a=moth.cell;moth.sprite=Sprite::FurnaceMothHot;
}
void interrupt_furnace_moth(Entity& moth) {
    if (moth.kind==EntityKind::FurnaceMoth && moth.label_a!=FurnaceOrbit && moth.label_a!=FurnaceRest) rest(moth);
}
bool cool_furnace_moth(Entity& moth) {
    if (moth.kind!=EntityKind::FurnaceMoth || moth.health<=0) return false;
    const bool changed=moth.counter_a>0 || moth.label_b>0;
    moth.counter_a=0;rest(moth,120);moth.light={};return changed;
}
void step_furnace_moth(Game& game,int slot) {
    Entity& moth=game.entities[static_cast<std::size_t>(slot)];
    if (moth.health<=0) return;
    if (moth.freeze_ticks>0) cool_furnace_moth(moth);
    if (moth.vitals.rooted>0) interrupt_furnace_moth(moth);
    const bool hot=moth.counter_a>0 || moth.label_b>0;
    moth.sprite=hot ? Sprite::FurnaceMothHot : Sprite::FurnaceMoth;
    moth.light=hot ? LightEmitter{2,moth.label_a==FurnaceWarn ? 300 : 100,{235,137,67}} : LightEmitter{};
    if (moth.label_a==FurnaceRest) {
        if (moth.timer_a==0) {moth.label_a=FurnaceOrbit;moth.counter_b=0;moth.timer_b=0;}
        return;
    }
    if (moth.label_a!=FurnaceOrbit && moth.cell!=moth.point_b) {rest(moth);return;}
    if (moth.label_a==FurnaceFeed) {
        if (moth.timer_a>0) return;
        const Entity* fuel=get_entity(game,moth.entity_a);
        if ((moth.entity_a.slot<0 || (fuel && fuel->cell==moth.point_a)) &&
            distance(moth.cell,moth.point_a)<=1 && clear_attack_sight(game,moth.cell,moth.point_a) && exposed_fuel(game,moth.point_a))
            moth.counter_a=extract_heat(game,moth.point_a,300);
        if (moth.counter_a>0) emit_sound(game,SoundId::FurnaceMothFeed,moth.point_a);
        rest(moth,24);return;
    }
    if (moth.label_a==FurnaceWarn) {
        if (moth.timer_a>0) return;
        moth.label_b=moth.counter_a;moth.counter_a=0;moth.label_a=FurnaceDive;
        moth.move_interval=6;moth.move_wait=0;emit_sound(game,SoundId::FurnaceMothDive,moth.cell);return;
    }
    if (moth.label_a==FurnaceDive) {
        if (moth.move_wait>0) return;
        const Cell next=moth.cell+moth.facing;
        if (!walkable(game.stage.at_or_border(next)) || entity_at(game,next,true)>=0) {strike(game,slot,next);return;}
        if (!move_entity(game,slot,next,false)) {rest(moth,90);return;}
        if (moth.health<=0 || moth.label_a!=FurnaceDive) return;
        if (moth.cell!=next) {rest(moth,90);return;}
        moth.point_b=moth.cell;
        if (--moth.counter_b<=0) {strike(game,slot,moth.cell);return;}
        return;
    }
    if (moth.timer_b==0) find_light(game,moth);
    if (const Entity* source=get_entity(game,moth.entity_a);source && (source->health>0 || source->kind==EntityKind::GroundItem)) moth.point_a=source->cell;
    const int gap=distance(moth.cell,moth.point_a);
    if (moth.counter_a==0 && gap<=1 && exposed_fuel(game,moth.point_a)) {
        moth.label_a=FurnaceFeed;moth.timer_a=30;moth.point_b=moth.cell;
        emit_sound(game,SoundId::FurnaceMothSip,moth.cell);return;
    }
    if (moth.counter_a>0 && gap>0 && gap<=5 &&
        (moth.cell.x==moth.point_a.x || moth.cell.y==moth.point_a.y) && clear_attack_sight(game,moth.cell,moth.point_a)) {
        moth.facing=cardinal_toward(moth.cell,moth.point_a,moth.facing);
        moth.counter_b=gap;moth.point_b=moth.cell;moth.label_a=FurnaceWarn;moth.timer_a=45;
        emit_sound(game,SoundId::FurnaceMothWarn,moth.cell);return;
    }
    if (gap>1) {approach(game,slot,moth.point_a);return;}
    if (moth.move_wait==0) {
        constexpr Cell orbit[]{{1,0},{0,1},{-1,0},{0,-1}};
        const Cell goal=moth.point_a+orbit[static_cast<std::size_t>(moth.counter_b%4)];
        const Cell previous=moth.cell;
        if (moth.cell!=goal) approach(game,slot,goal);
        if (moth.cell==goal || moth.cell==previous) moth.counter_b=(moth.counter_b+1)%4;
    }
}
bool valid_furnace_moth(const Entity& moth) {
    return moth.kind!=EntityKind::FurnaceMoth || (moth.label_a>=FurnaceOrbit && moth.label_a<=FurnaceRest &&
        moth.counter_a>=0 && moth.counter_a<=300 && moth.label_b>=0 && moth.label_b<=300 &&
        moth.counter_b>=0 && moth.counter_b<=5 && moth.timer_a>=0 && moth.timer_a<=120 &&
        moth.timer_b>=0 && moth.timer_b<=24 &&
        ((moth.label_a!=FurnaceWarn && moth.label_a!=FurnaceDive) ||
            (moth.counter_b>0 && distance({},moth.facing)==1)));
}
