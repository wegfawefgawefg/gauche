#include "ice_anchor.hpp"
#include "action.hpp"
#include "../entities/attacks.hpp"
#include "../entities/pocket_door.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {
constexpr RegionalItem kit{"Ice Anchor",
    "Place at your feet. Hold Use again to reel back up to 5 cells. Hazards apply. Secondary recovers nearby. Each step wears 1 HP.",
    Sprite::IceAnchor,{0,5,0,0,24,PatternEffect::Utility},
    ItemAction::Material,32,1,false,0,0,0,0,0,SoundId::AnchorSet,30};

Entity* endpoint(Game& game,const Item& item) {
    Entity* anchor=get_entity(game,item.anchor);
    return anchor && anchor->kind==EntityKind::IceAnchor ? anchor : nullptr;
}
void stop_reel(Entity& anchor) {
    anchor.entity_a={}; anchor.fixture_open=false; anchor.counter_a=0;
}
bool linked(const Game& game,Handle handle) {
    for (const Entity& holder : game.entities) {
        if (holder.kind==EntityKind::None) continue;
        if (holder.kind==EntityKind::GroundItem && holder.ground_item.kind==ItemKind::IceAnchor &&
            holder.ground_item.anchor==handle) return true;
        for (const Item& item : holder.inventory.slots)
            if (item.kind==ItemKind::IceAnchor && item.anchor==handle) return true;
    }
    return false;
}
bool anchor_support(const Tile* tile) {
    return tile && (buildable(tile->kind) || tile->kind==TileKind::Ice) && walkable(*tile);
}
bool place_anchor(Game& game,Entity& user,Item& item) {
    const Tile* tile=game.stage.at(user.cell);
    if (!anchor_support(tile) || surface_wet(*tile) || tile->surface.fire_ticks>0 ||
        (tile->prop.kind!=PropKind::None && !tile->prop.broken)) return false;
    for (const Entity& actor : game.entities)
        if (&actor!=&user && actor.kind!=EntityKind::None && actor.cell==user.cell) return false;
    const Handle handle=spawn_entity(game,EntityKind::IceAnchor,user.cell);
    Entity* anchor=get_entity(game,handle);
    if (!anchor) return false;
    anchor->health=item.durability; anchor->max_health=item.max_durability;
    item.anchor=handle;
    emit_sound(game,SoundId::AnchorSet,user.cell);
    return true;
}
}

const RegionalItem* ice_anchor_item(ItemKind kind) { return kind==ItemKind::IceAnchor ? &kit : nullptr; }

// FIXTURE SLOTS: entity_a reeling player, point_a expected player cell,
// counter_a remaining steps, timer_b three-tick movement beat; fixture_open is
// this tick's held-input permission, consumed by the fixture. Item.anchor owns it.
void init_ice_anchor(Entity& anchor) {
    anchor.sprite=Sprite::AnchorPoint;
    anchor.health=anchor.max_health=30;
    anchor.hard_blocker=true;
}

Cell anchor_next_cell(Cell from,Cell to) {
    const Cell delta=to-from;
    if (std::abs(delta.x)>=std::abs(delta.y) && delta.x!=0) return from+Cell{delta.x>0 ? 1 : -1,0};
    return delta.y!=0 ? from+Cell{0,delta.y>0 ? 1 : -1} : from;
}

bool anchor_path_clear(const Game& game,Cell from,Cell to) {
    if (distance(from,to)>5 || !pocket_passage_allowed(game,from,to)) return false;
    // ROPE: No routing around corners. Sight tests physical cover, not smoke.
    if (!clear_attack_sight(game,from,to,false)) return false;
    for (Cell cell=from;cell!=to;) {
        cell=anchor_next_cell(cell,to);
        const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile)) return false;
    }
    return true;
}

void fold_ice_anchor(Item& item) {
    if (item.kind==ItemKind::IceAnchor) { item.anchor={}; item.opened=false; }
}

void sync_ice_anchor(Game& game,Item& item) {
    if (item.kind!=ItemKind::IceAnchor || item.anchor.slot<0) return;
    const Entity* anchor=endpoint(game,item);
    if (!anchor || anchor->health<=0) { item={}; return; }
    item.durability=anchor->health;
}

bool step_anchor_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::IceAnchor) return false;
    if (user.label_b!=0) cancel_item_action(user);
    const bool pressed=input.use && !item.opened;
    item.opened=input.use;
    Entity* anchor=endpoint(game,item);
    if (anchor && (anchor->health<=0 || item.durability<=0)) { item={}; return true; }
    if (input.cancel_use || input.pickup || input.drop || input.interact || input.move!=Cell{} ||
        user.health<=0 || user.sleep_ticks>0 || user.stun_ticks>0 || user.vitals.rooted>0) {
        if (anchor) stop_reel(*anchor);
        return true;
    }
    if (input.reload) {
        if (anchor) stop_reel(*anchor);
        if (anchor && item.cooldown==0 && distance(user.cell,anchor->cell)<=1 &&
            anchor_path_clear(game,user.cell,anchor->cell)) {
            item.durability=anchor->health;
            const Handle handle=item.anchor;
            fold_ice_anchor(item); item.cooldown=24; item.opened=input.use;
            remove_entity(game,handle);
            emit_sound(game,SoundId::AnchorRecover,user.cell);
        }
        return true;
    }
    if (anchor && anchor->entity_a==Handle{slot,user.generation}) {
        anchor->fixture_open=input.use;
        return true;
    }
    if (!pressed || item.cooldown>0) return true;
    item.cooldown=24;
    if (!anchor) {
        if (item.anchor.slot>=0) { item={}; return true; }
        if (!place_anchor(game,user,item)) emit_sound(game,SoundId::AnchorBlocked,user.cell);
        return true;
    }
    if (user.cell==anchor->cell || !anchor_path_clear(game,user.cell,anchor->cell)) {
        emit_sound(game,SoundId::AnchorBlocked,user.cell); return true;
    }
    anchor->entity_a={slot,user.generation}; anchor->point_a=user.cell;
    anchor->counter_a=distance(user.cell,anchor->cell); anchor->timer_b=3;
    anchor->fixture_open=true;
    emit_sound(game,SoundId::AnchorTension,user.cell);
    return true;
}

void step_ice_anchor(Game& game,int slot) {
    Entity& anchor=game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot,anchor.generation};
    if (!linked(game,handle)) { remove_entity(game,handle); return; }
    if (!anchor_support(game.stage.at(anchor.cell))) {
        emit_sound(game,SoundId::AnchorBreak,anchor.cell); remove_entity(game,handle); return;
    }
    Entity* user=get_entity(game,anchor.entity_a);
    const bool pulling=anchor.fixture_open;
    anchor.fixture_open=false;
    if (!user || user->kind!=EntityKind::Player || user->health<=0 || user->sleep_ticks>0 || user->stun_ticks>0 ||
        user->vitals.rooted>0 || user->cell!=anchor.point_a || !pulling ||
        user->inventory.held()->kind!=ItemKind::IceAnchor || user->inventory.held()->anchor!=handle) {
        stop_reel(anchor); return;
    }
    if (anchor.timer_b>0) return;
    const Cell next=anchor_next_cell(user->cell,anchor.cell);
    if (next==user->cell || anchor.counter_a<=0) { stop_reel(anchor); return; }
    const Cell facing=user->facing;
    if (!anchor_path_clear(game,user->cell,anchor.cell) || !move_entity(game,anchor.entity_a.slot,next)) {
        emit_sound(game,SoundId::AnchorBlocked,user->cell); stop_reel(anchor); return;
    }
    user->facing=facing; user->use_flash=6;
    // Contact can detonate a trap and destroy the point during this very step.
    if (get_entity(game,handle)!=&anchor || anchor.health<=0) {
        sync_ice_anchor(game,*user->inventory.held());
        return;
    }
    --anchor.health; --anchor.counter_a; anchor.timer_b=3;
    user->inventory.held()->durability=anchor.health;
    emit_sound(game,SoundId::AnchorReel,user->cell);
    if (anchor.health==0) {
        *user->inventory.held()={};
        emit_sound(game,SoundId::AnchorBreak,anchor.cell);
        stop_reel(anchor); remove_entity(game,handle); return;
    }
    if (user->cell!=next || user->health<=0 || user->vitals.rooted>0 || user->cell==anchor.cell) {
        stop_reel(anchor); return;
    }
    anchor.point_a=user->cell;
}

bool valid_ice_anchor(const Entity& anchor) {
    return anchor.kind!=EntityKind::IceAnchor || (anchor.counter_a>=0 && anchor.counter_a<=5 &&
        anchor.timer_b>=0 && anchor.timer_b<=3 && anchor.health<=anchor.max_health &&
        (anchor.max_health==30 || anchor.max_health==60) && anchor.hard_blocker && !anchor.impassable);
}

bool hit_ice_anchor(Game& game,Cell cell,int damage,Cell source) {
    bool hit=false;
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& anchor=game.entities[static_cast<std::size_t>(slot)];
        if (anchor.kind!=EntityKind::IceAnchor || anchor.cell!=cell || anchor.health<=0 || damage<=0) continue;
        damage_entity(game,slot,damage,source); hit=true;
    }
    return hit;
}
