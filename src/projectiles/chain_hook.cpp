#include "chain_hook.hpp"
#include "../items/sled.hpp"
#include "../world/floating_items.hpp"
#include "../combat/shove.hpp"
#include <algorithm>

namespace {
bool movable_prop(const Prop& prop) {
    if (prop.broken || !prop.hp || prop.growth_ticks) return false;
    switch (prop.kind) {
    case PropKind::Crate: case PropKind::ClayPot: case PropKind::ScrapBin:
    case PropKind::OreBin: case PropKind::FishingCreel: case PropKind::FrozenLunchTin: return true;
    default: return false;
    }
}
int signature(const Prop& prop) {return prop.hp+(prop.covered ? 256 : 0)+static_cast<int>(prop.variant)*512;}
bool cargo(const Game& game,const Entity& actor) {
    if (actor.kind==EntityKind::Sled) return actor.health>0 && actor.toss.ticks==0;
    return actor.kind==EntityKind::GroundItem && actor.ground_item.kind!=ItemKind::None &&
        actor.ground_item.count>0 && actor.ground_item.flight.slot<0 && actor.toss.ticks==0 && !sled_cargo(game,actor);
}
bool aligned(const Game& game,const Entity& owner,const Entity& line,Cell endpoint) {
    const Cell delta=endpoint-owner.cell;const int along=delta.x*line.facing.x+delta.y*line.facing.y;
    if (along<1 || along>line.attack_interval || delta.x*line.facing.y!=delta.y*line.facing.x) return false;
    for (int i=1;i<along;++i)
        if (projectile_blocked(game,owner.cell+Cell{line.facing.x*i,line.facing.y*i})) return false;
    return true;
}
bool vacant(const Game& game,Cell cell,bool prop) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || (prop && tile->prop.kind!=PropKind::None)) return false;
    for (const Entity& e:game.entities)
        if (e.kind!=EntityKind::None && e.cell==cell && (e.impassable || e.kind==EntityKind::GroundItem || e.kind==EntityKind::Sled)) return false;
    return true;
}
void latch(Game& game,Entity& line,ChainPhase phase) {
    line.label_b=phase;line.counter_a=0;line.point_b=line.cell;line.timer_b=10;
    emit_sound(game,SoundId::ChainLatch,line.cell);
}
bool target_valid(const Game& game,const Entity& line) {
    const Tile* tile=game.stage.at(line.point_b);
    if (!tile) return false;
    if (line.label_b==ChainWall) return tile->kind==TileKind::Wall;
    if (line.label_b==ChainProp) return movable_prop(tile->prop) &&
        static_cast<int>(tile->prop.kind)==line.counter_b && signature(tile->prop)==line.counter_c;
    if (line.label_b==ChainFixedProp) return prop_blocks(tile->prop) && !movable_prop(tile->prop) &&
        static_cast<int>(tile->prop.kind)==line.counter_b;
    const Entity* target=get_entity(game,line.entity_b);
    if (!target || target->cell!=line.point_b) return false;
    return line.label_b==ChainCargo ? cargo(game,*target) : target->impassable && target->hard_blocker;
}
void cast_step(Game& game,Handle handle,const Entity& owner) {
    Entity& line=*get_entity(game,handle);const Cell next=line.cell+line.facing;
    const Tile* tile=game.stage.at(next);
    if (!tile || !aligned(game,owner,line,next)) {release_chain_hook(game,handle);return;}
    line.cell=next;line.timer_b=4;--line.counter_a;
    if (tile->kind==TileKind::Wall) {latch(game,line,ChainWall);return;}
    if (movable_prop(tile->prop) || prop_blocks(tile->prop)) {
        line.counter_b=static_cast<int>(tile->prop.kind);line.counter_c=signature(tile->prop);
        latch(game,line,movable_prop(tile->prop) ? ChainProp : ChainFixedProp);return;
    }
    // A loaded sled takes priority over its loose-item passenger in the same cell.
    for (EntityKind kind:{EntityKind::Sled,EntityKind::GroundItem}) {
        if (kind==EntityKind::GroundItem) {
            const int body=entity_at(game,next,true);
            if (body>=0) {
                const Entity& caught=game.entities[static_cast<std::size_t>(body)];
                if (!caught.hard_blocker) {release_chain_hook(game,handle);return;}
                line.entity_b={body,caught.generation};latch(game,line,ChainFixture);return;
            }
        }
        for (int i=0;i<max_entities;++i) {
            Entity& found=game.entities[static_cast<std::size_t>(i)];
            if (found.kind!=kind || found.cell!=next || !cargo(game,found)) continue;
            if (kind==EntityKind::Sled) {found.label_a=0;found.timer_b=0;found.point_a=found.cell;}
            else stop_item_float(game,found);
            line.entity_b={i,found.generation};latch(game,line,ChainCargo);return;
        }
    }
    if (!line.counter_a) release_chain_hook(game,handle);
}
void reel(Game& game,Handle handle,Entity& owner) {
    Entity& line=*get_entity(game,handle);
    if (!target_valid(game,line) || !aligned(game,owner,line,line.cell)) {release_chain_hook(game,handle);return;}
    const bool pulling=line.fixture_open;line.fixture_open=false;
    if (distance(owner.cell,line.cell)<=1) {release_chain_hook(game,handle);return;}
    if (!pulling || line.timer_b>0) return;
    line.timer_b=10;
    if (line.label_b==ChainProp) {
        const Cell next=line.cell-line.facing;
        if (!vacant(game,next,true)) {release_chain_hook(game,handle);return;}
        Tile& from=*game.stage.at(line.cell);Tile& to=*game.stage.at(next);
        to.prop=from.prop;from.prop={};line.cell=line.point_b=next;
    } else if (line.label_b==ChainCargo) {
        Entity& target=*get_entity(game,line.entity_b);const Cell next=target.cell-line.facing;
        if (!vacant(game,next,false)) {release_chain_hook(game,handle);return;}
        if (target.kind==EntityKind::Sled) {
            if (!haul_sled(game,line.entity_b.slot,{-line.facing.x,-line.facing.y})) {release_chain_hook(game,handle);return;}
        } else target.cell=next;
        line.cell=line.point_b=next;
    } else {
        const Cell next=owner.cell+line.facing;
        if (owner.vitals.grip || owner.vitals.rooted || ridden_sled(game,owner) || !vacant(game,next,false) ||
            !shove_actor(game,line.entity_a.slot,line.facing,owner.cell)) {release_chain_hook(game,handle);return;}
    }
    emit_sound(game,SoundId::ChainHaul,line.cell);
}
}
// Slots: label_b phase, entity_a owner, entity_b exact cargo/fixture; point_b
// expected endpoint. counter_b prop kind, counter_c hp/cover/variant signature.
// timer_a finite four-second lifetime, timer_b flight/haul beat; fixture_open held use.
bool launch_chain_hook(Game& game,int owner_slot,Cell direction) {
    Entity& owner=game.entities[static_cast<std::size_t>(owner_slot)];Item& item=*owner.inventory.held();
    if (item.kind!=ItemKind::ChainHook || item.uses<=0 || item.cooldown>0 || item.flight.slot>=0 || distance({},direction)!=1) return false;
    for (const Entity& other:game.entities) if (other.kind==EntityKind::Projectile &&
        other.label_a==static_cast<int>(ProjectileKind::ChainHook) && other.entity_a==Handle{owner_slot,owner.generation}) return false;
    const Handle handle=spawn_entity(game,EntityKind::Projectile,owner.cell);Entity* line=get_entity(game,handle);
    if (!line) return false;
    line->label_a=static_cast<int>(ProjectileKind::ChainHook);line->sprite=Sprite::HookHead;
    line->entity_a={owner_slot,owner.generation};line->point_a=owner.cell;line->facing=direction;
    line->counter_a=line->attack_interval=item_pattern(item).maximum;
    line->timer_a=240;line->timer_b=4;line->ground_item=item;
    item.flight=handle;--item.uses;item.cooldown=item_pattern(item).cooldown;owner.use_flash=6;
    emit_sound(game,SoundId::ChainCast,owner.cell);return true;
}
void release_chain_hook(Game& game,Handle handle) {
    Entity* line=get_entity(game,handle);
    if (!line || line->kind!=EntityKind::Projectile || line->label_a!=static_cast<int>(ProjectileKind::ChainHook)) return;
    if (Entity* owner=get_entity(game,line->entity_a)) for (Item& item:owner->inventory.slots) if (item.flight==handle) {
        item.flight={};
        if (item.uses==0) {item={};emit_sound(game,SoundId::ChainSpent,owner->cell);}
    }
    emit_sound(game,SoundId::ChainRelease,line->cell);remove_entity(game,handle);
}
void step_chain_hook(Game& game,int slot) {
    Entity& line=game.entities[static_cast<std::size_t>(slot)];const Handle handle{slot,line.generation};
    Entity* owner=get_entity(game,line.entity_a);
    if (!owner || owner->health<=0 || owner->sleep_ticks || owner->stun_ticks || owner->toss.ticks || !line.timer_a ||
        owner->inventory.held()->kind!=ItemKind::ChainHook || owner->inventory.held()->flight!=handle) {release_chain_hook(game,handle);return;}
    if (line.label_b!=ChainOut) reel(game,handle,*owner);
    else {line.fixture_open=false;if (!line.timer_b) cast_step(game,handle,*owner);}
}
bool valid_chain_hook(const Entity& line) {
    if (line.kind!=EntityKind::Projectile || line.label_a!=static_cast<int>(ProjectileKind::ChainHook)) return true;
    return line.ground_item.kind==ItemKind::ChainHook && line.ground_item.flight.slot<0 &&
        line.label_b>=ChainOut && line.label_b<=ChainFixture && line.counter_a>=0 && line.counter_a<=line.attack_interval &&
        line.attack_interval==item_pattern(line.ground_item).maximum && line.timer_a<=240 && line.timer_b<=10 &&
        line.counter_b>=0 && line.counter_b<static_cast<int>(PropKind::Count) && line.counter_c>=0 && line.counter_c<131072 &&
        distance({},line.facing)==1;
}
