// Permanent hand actions. Shove crushes against masonry; Kick only displaces.
#include "basic_actions.hpp"
#include "action.hpp"
#include "../entities/attacks.hpp"
#include "cooking.hpp"
#include "../combat/shove.hpp"
#include "../artifacts/powers.hpp"
#include "../props/interaction.hpp"
#include "../world/chasm.hpp"
#include "../world/water.hpp"
#include <algorithm>
#include <cmath>

namespace {
int target_at(const Game& game,Cell cell,int user) {
    const int solid=entity_at(game,cell,true);
    if (solid>=0 && solid!=user) return solid;
    for (int i=0;i<max_entities;++i) if (i!=user) {
        const auto& actor=game.entities[static_cast<std::size_t>(i)];
        if (actor.kind==EntityKind::GroundItem && actor.cell==cell && actor.basic.carried_by.slot<0) return i;
    }
    return -1;
}
bool movable(const Entity& target) {
    return !target.hard_blocker && !target.vitals.grip && target.basic.carried_by.slot<0 &&
        (target.kind==EntityKind::GroundItem || (target.health>0 && target.move_interval>0));
}
void push(Game& game,int slot,Cell direction,Cell source,int steps,bool crush) {
    const Handle handle{slot,game.entities[static_cast<std::size_t>(slot)].generation};
    for (int n=0;n<steps;++n) {
        Entity* target=get_entity(game,handle);
        if (!target || !movable(*target)) break;
        const Cell next=target->cell+direction;
        const auto& tile=game.stage.at_or_border(next);
        const int blocker=entity_at(game,next,true);
        // A second body receives an impact, never the wall-crush rule.
        if (blocker>=0 && !game.entities[static_cast<std::size_t>(blocker)].hard_blocker) {
            damage_entity(game,blocker,4,source);
            if (target->health>0) damage_entity(game,slot,4,source);
            break;
        }
        if (!crush && ((!walkable(tile) && !open_drop(tile.kind)) || blocker>=0)) break;
        if (!shove_actor(game,slot,direction,source)) break;
    }
}
bool push_prop(Game& game,Cell cell,Cell direction,Cell source,bool crush) {
    Tile* from=game.stage.at(cell);
    if (!from || from->prop.broken || !from->prop.hp) return false;
    switch (from->prop.kind) {
    case PropKind::Crate:case PropKind::ClayPot:case PropKind::IceBlock:
    case PropKind::SnowCache:case PropKind::Pallet:break;
    default:return false;
    }
    Tile* to=game.stage.at(cell+direction);
    if (!to || (!walkable(*to) && !open_drop(to->kind))) {
        if (crush) hit_prop(game,cell,255,source);
        return true;
    }
    const int victim=entity_at(game,cell+direction,true);
    if (victim>=0) {
        push(game,victim,direction,source,1,crush);
        if (entity_at(game,cell+direction,true)>=0) return true;
    }
    if (to->prop.kind!=PropKind::None) return true;
    if (open_drop(to->kind)) emit_sound(game,SoundId::ChasmFall,cell+direction);
    else to->prop=from->prop;
    from->prop={};emit_sound(game,SoundId::BlockLand,cell+direction);
    return true;
}
bool start_jump(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (user.vitals.rooted || user.basic.jump_ticks || user.toss.ticks) return false;
    const int reach=2+(has_artifact(user,ArtifactKind::Oversized) ? 1 : 0);
    Cell destination=user.cell;
    for (int i=0;i<reach;++i) {
        const Cell next=destination+direction;
        const Tile* tile=game.stage.at(next);
        if (!tile || (!walkable(*tile) && !open_drop(tile->kind) && tile->kind!=TileKind::Water) || entity_at(game,next,true)>=0) break;
        destination=next;
    }
    if (destination==user.cell) {emit_sound(game,SoundId::BumpStone,user.cell);return false;}
    user.basic.jump_origin=user.cell; user.basic.jump_destination=destination; user.basic.jump_ticks=18;
    return true;
}
}

void basic_contact(Game& game,Entity& user,Cell cell) {
    if (!has_artifact(user,ArtifactKind::Chef)) return;
    for (auto& actor:game.entities) {
        if (actor.kind!=EntityKind::GroundItem || actor.cell!=cell) continue;
        const auto cooked=cooked_item(actor.ground_item.kind);
        if (cooked==ItemKind::None) continue;
        actor.ground_item=make_item(cooked,actor.ground_item.count);
        actor.sprite=item_sprite(cooked);emit_sound(game,SoundId::CookingSizzle,cell);
    }
}

bool use_basic_action(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& held=*user.inventory.held();
    const ItemKind kind=held.kind;
    if (kind==ItemKind::Balloon) return false; // Selected hand supplies lift; no activation or refresh.
    if (kind==ItemKind::Jump) return start_jump(game,slot,direction);
    if (kind==ItemKind::Grapple) return use_grapple(game,slot,direction);
    if (kind==ItemKind::ParryPan) {
        user.block_ticks=has_artifact(user,ArtifactKind::Oversized) ? 9 : 6;user.guard_slot=user.inventory.selected;
        for (int i=-static_cast<int>(has_artifact(user,ArtifactKind::Sweeping));i<=static_cast<int>(has_artifact(user,ArtifactKind::Sweeping));++i)
            basic_contact(game,user,user.cell+direction+Cell{-direction.y*i,direction.x*i});
        return true;
    }
    if (kind==ItemKind::Elbow) direction={-direction.x,-direction.y};
    const auto pattern=active_item_pattern(held,user);
    const Cell side{-direction.y,direction.x};
    std::array<bool,max_entities> hit_once{};
    bool contact=false;
    if (game.sweep_count<static_cast<int>(game.sweeps.size()))
        game.sweeps[static_cast<std::size_t>(game.sweep_count++)]={user.cell,direction,pattern.maximum,pattern.half_width};
    for (int lane=-pattern.half_width;lane<=pattern.half_width;++lane)
        for (int reach=1;reach<=pattern.maximum;++reach) {
            const Cell cell=user.cell+Cell{direction.x*reach+side.x*lane,direction.y*reach+side.y*lane};
            if (!clear_attack_sight(game,user.cell,cell,false,true)) break;
            basic_contact(game,user,cell);
            if ((kind==ItemKind::Shove || kind==ItemKind::Kick || kind==ItemKind::CrushShield) &&
                push_prop(game,cell,direction,user.cell,kind!=ItemKind::Kick)) {contact=true;break;}
            const int damage=pattern.damage+(has_artifact(user,ArtifactKind::Iron) ? 4 : 0);
            const int prop_damage=power_damage(user,std::max(4,damage))*(has_artifact(user,ArtifactKind::Iron) ? 2 : 1);
            contact |= hit_prop(game,cell,prop_damage,user.cell);
            contact |= hit_terrain(game,cell,user.cell,prop_damage,0);
            const int victim=target_at(game,cell,slot);
            if (victim<0 || hit_once[static_cast<std::size_t>(victim)]) continue;
            hit_once[static_cast<std::size_t>(victim)]=true;contact=true;
            auto& target=game.entities[static_cast<std::size_t>(victim)];
            const bool blocked=blocks_facing(target,user.cell);
            if (damage>0) damage_entity(game,victim,damage,user.cell,true,{slot,user.generation});
            if (blocked) continue;
            if (kind==ItemKind::Slap && target.health>0 && !target.hard_blocker)
                target.facing={-target.facing.y,target.facing.x};
            if ((kind==ItemKind::Shove || kind==ItemKind::CrushShield || kind==ItemKind::GodFist || kind==ItemKind::Kick) && movable(target))
                push(game,victim,direction,user.cell,kind==ItemKind::GodFist ? 8 : 2,kind!=ItemKind::Kick);
        }
    if (contact) emit_sound(game,kind==ItemKind::Fist ? SoundId::Punch1 : basic_action_item(kind)->sound,user.cell);
    if (has_artifact(user,ArtifactKind::Iron)) emit_sound(game,SoundId::PanReflect,user.cell);
    if (kind==ItemKind::CrushShield) {user.block_ticks=12;user.guard_slot=user.inventory.selected;}
    return true;
}

bool balloon_floating(const Entity& actor) {
    const Item& held=*actor.inventory.held();
    return actor.health>0 && held.kind==ItemKind::Balloon && held.count>0 && held.loaded>0;
}
bool basic_airborne(const Entity& actor) {
    return actor.basic.jump_ticks>0 || actor.basic.carried_by.slot>=0 || balloon_floating(actor);
}
float basic_jump_height(const Entity& actor) {
    return actor.basic.jump_ticks>0 ? std::sin(static_cast<float>(18-actor.basic.jump_ticks)*3.14159265F/18)*.8F : 0;
}
void step_basic_state(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    step_grapple(game,slot);
    if (user.kind!=EntityKind::Player) return;
    Item& held=*user.inventory.held();
    if (balloon_floating(user)) {
        if (--held.loaded==0) {held={};emit_sound(game,SoundId::BalloonPop,user.cell);}
    }
    if (user.basic.jump_ticks<=0) return;
    --user.basic.jump_ticks;
    const auto motion=user.basic;
    if (user.health<=0) {user.basic.jump_ticks=0;return;}
    const int span=distance(motion.jump_origin,motion.jump_destination);
    const int progress=(18-motion.jump_ticks)*span/18;
    const Cell direction=cardinal_toward(motion.jump_origin,motion.jump_destination,user.facing);
    const Cell next=motion.jump_origin+Cell{direction.x*progress,direction.y*progress};
    const auto* tile=game.stage.at(next);
    const int blocker=entity_at(game,next,true);
    if (!tile || (blocker>=0 && blocker!=slot) || (!walkable(*tile) && !open_drop(tile->kind) && tile->kind!=TileKind::Water)) {
        user.basic.jump_ticks=0;user.move_wait=8;
    } else user.cell=next;
    if (user.basic.jump_ticks) return;
    user.basic.jump_origin=user.basic.jump_destination={};
    emit_sound(game,SoundId::JumpLand,user.cell);
    enter_actor_cell(game,slot);
    if (user.health<=0) return;
    const int radius=has_artifact(user,ArtifactKind::Sweeping) ? 1 : 0;
    for (int y=-radius;y<=radius;++y) for (int x=-radius;x<=radius;++x) basic_contact(game,user,user.cell+Cell{x,y});
    if (has_artifact(user,ArtifactKind::Iron)) {
        for (int i=0;i<max_entities;++i) if (i!=slot && distance(user.cell,game.entities[static_cast<std::size_t>(i)].cell)<=1)
            damage_entity(game,i,8,user.cell,true,{slot,user.generation});
        emit_sound(game,SoundId::PanReflect,user.cell);
    }
}
