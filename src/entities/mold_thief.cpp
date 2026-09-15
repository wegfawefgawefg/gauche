#include "mold_thief.hpp"
#include "casting_mold.hpp"
#include "behavior.hpp"
#include "../items/magnet.hpp"
#include "../items/sled.hpp"
#include "../world/ground_items.hpp"
#include <algorithm>

namespace {
bool carries(const Entity& actor) {return actor.ground_item.kind!=ItemKind::None && actor.ground_item.count>0;}
bool loose_metal(const Game& game,const Entity& loot) {
    const Item& item=loot.ground_item;
    return loot.kind==EntityKind::GroundItem && item.count>0 && magnetic_item(item.kind) &&
        item.flight.slot<0 && item.anchor.slot<0 && item.flame_ticks==0 && !sled_cargo(game,loot);
}
void rest(Entity& actor) {actor.label_a=MoldRest;actor.timer_a=60;actor.entity_a={};}
bool walk_to(Game& game,int slot,Cell target) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.cell==target) return true;
    if (actor.move_wait>0) return false;
    if (const auto next=next_route_cell(game,slot,target,512)) willing_step(game,slot,*next);
    else actor.move_wait=actor.move_interval;
    return false;
}
void find_metal(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    int nearest=9;Handle found;
    for (int i=0;i<max_entities;++i) {
        const Entity& loot=game.entities[static_cast<std::size_t>(i)];
        const int gap=distance(actor.cell,loot.cell);
        if (!loose_metal(game,loot) || gap>=nearest) continue;
        const Tile* tile=game.stage.at(loot.cell);const int occupant=entity_at(game,loot.cell,true);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || (occupant>=0 && occupant!=slot) ||
            (gap>0 && !next_route_cell(game,slot,loot.cell,512))) continue;
        nearest=gap;found={i,loot.generation};
    }
    actor.entity_a=found;actor.timer_b=30;
}
std::optional<Cell> mold_stance(const Game& game,int slot,const Entity& mold) {
    const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (!empty_casting_mold(mold)) return {};
    if (distance(actor.cell,mold.cell)==1) return actor.cell;
    for (Cell side:{Cell{1,0},{0,1},{-1,0},{0,-1}}) {
        const Cell cell=mold.cell+side;const Tile* tile=game.stage.at(cell);
        if (tile && walkable(*tile) && tile->kind!=TileKind::Lava && entity_at(game,cell,true)<0 &&
            next_route_cell(game,slot,cell,512)) return cell;
    }
    return {};
}
void find_mold(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    int nearest=13;Handle found;
    for (int i=0;i<max_entities;++i) {
        const Entity& mold=game.entities[static_cast<std::size_t>(i)];
        const int gap=distance(actor.cell,mold.cell);
        if (gap>=nearest || !empty_casting_mold(mold) || !mold_stance(game,slot,mold)) continue;
        nearest=gap;found={i,mold.generation};
    }
    actor.entity_b=found;actor.timer_b=30;
}
void spill_cargo(Game& game,Entity& actor) {
    if (!carries(actor)) return;
    Entity* loose=get_entity(game,spawn_entity(game,EntityKind::GroundItem,nearby_ground_item_cell(game,actor.cell)));
    if (!loose) return; // Full pool: keep the item visibly on its carrier.
    loose->ground_item=actor.ground_item;loose->sprite=item_sprite(loose->ground_item);actor.ground_item={};
    emit_sound(game,SoundId::MoldSpill,actor.cell);
}
}
// SLOTS: label_a phase, timer_a grab/seal/rest/flee; timer_b search throttle;
// entity_a exact loose item, entity_b exact destination mold; ground_item cargo;
// point_a committed stance, point_b last attacker. c slots retain hearing.
void init_mold_thief(Entity& actor) {
    actor.health=actor.max_health=58;actor.impassable=true;actor.move_interval=14;actor.sprite=Sprite::MoldThief;
}
void interrupt_mold_thief(Entity& actor) {
    if (actor.kind==EntityKind::MoldThief && (actor.label_a==MoldGrab || actor.label_a==MoldSeal)) rest(actor);
}
void hurt_mold_thief(Game& game,Entity& actor,Cell source) {
    if (actor.kind!=EntityKind::MoldThief || actor.health<=0) return;
    spill_cargo(game,actor);actor.entity_a={};actor.point_b=source;actor.label_a=MoldFlee;actor.timer_a=180;
}
void step_mold_thief(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    actor.move_interval=carries(actor) ? 8 : 14;
    if (actor.freeze_ticks || actor.vitals.rooted || actor.stun_ticks || actor.sleep_ticks || actor.toss.ticks) {
        interrupt_mold_thief(actor);return;
    }
    if (actor.label_a==MoldFlee) {
        if (!actor.timer_a) {rest(actor);return;}
        flee(game,slot,actor.point_b);return;
    }
    if (actor.label_a==MoldRest) {
        if (!actor.timer_a) actor.label_a=carries(actor) ? MoldReturn : MoldSeek;
        return;
    }
    if (actor.label_a==MoldGrab || actor.label_a==MoldSeal) {
        if (actor.cell!=actor.point_a) {spill_cargo(game,actor);rest(actor);return;}
        if (actor.timer_a>0) {
            if (actor.label_a==MoldSeal && actor.timer_a%60==0) emit_sound(game,SoundId::MoldCrank,actor.cell);
            return;
        }
        if (actor.label_a==MoldGrab) {
            Entity* loot=get_entity(game,actor.entity_a);
            if (loot && loose_metal(game,*loot) && loot->cell==actor.cell && !carries(actor)) {
                actor.ground_item=loot->ground_item;remove_entity(game,actor.entity_a);
                emit_sound(game,SoundId::MoldSnatch,actor.cell);
            }
            actor.entity_a={};actor.entity_b={};actor.timer_b=0;
        } else {
            Entity* mold=get_entity(game,actor.entity_b);
            if (mold && distance(actor.cell,mold->cell)==1) seal_casting_mold(game,*mold,actor.ground_item);
        }
        rest(actor);return;
    }
    if (carries(actor)) {
        actor.label_a=MoldReturn;
        Entity* mold=get_entity(game,actor.entity_b);
        if ((!mold || !empty_casting_mold(*mold)) && !actor.timer_b) {
            find_mold(game,slot);mold=get_entity(game,actor.entity_b);
        }
        if (!mold || !empty_casting_mold(*mold)) {wander(game,slot);return;}
        if (actor.move_wait>0) return;
        const auto stance=mold_stance(game,slot,*mold);
        if (!stance) {actor.entity_b={};return;}
        if (walk_to(game,slot,*stance)) {
            actor.point_a=actor.cell;actor.label_a=MoldSeal;actor.timer_a=180;
            actor.facing=cardinal_toward(actor.cell,mold->cell,actor.facing);
            emit_sound(game,SoundId::MoldCrank,actor.cell);
        }
        return;
    }
    actor.label_a=MoldSeek;
    if (!actor.timer_b) find_metal(game,slot);
    Entity* loot=get_entity(game,actor.entity_a);
    if (!loot || !loose_metal(game,*loot)) {actor.entity_a={};wander(game,slot);return;}
    if (walk_to(game,slot,loot->cell)) {
        actor.point_a=actor.cell;actor.label_a=MoldGrab;actor.timer_a=18;
        emit_sound(game,SoundId::MoldTongs,actor.cell);
    }
}
bool valid_mold_thief(const Entity& actor) {
    if (actor.kind!=EntityKind::MoldThief) return true;
    constexpr int limits[]{0,18,0,180,60,180};
    if (actor.label_a<MoldSeek || actor.label_a>MoldFlee || actor.timer_a<0 || actor.timer_a>limits[actor.label_a] ||
        actor.timer_b<0 || actor.timer_b>30 || actor.ground_item.flight.slot>=0 || actor.ground_item.anchor.slot>=0) return false;
    return (actor.label_a!=MoldGrab || !carries(actor)) && (actor.label_a!=MoldSeal || carries(actor));
}
