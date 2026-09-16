#include "steel_toe.hpp"
#include "sled.hpp"
#include "action.hpp"
#include "../combat/shove.hpp"
#include "../entities/rail_cart.hpp"
#include "../projectiles/projectile.hpp"
#include "../surfaces/slip.hpp"
#include "../world/floating_items.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem cap{"Steel Toe Cap",
    "Twelve kicks. Shove a creature or loose item two tiles; 3 contact damage, 6 more against a wall. Punt landed bombs without resetting their fuse. Starts carts; brakes and anchored bodies resist. Durable: 24 kicks.",
    Sprite::SteelToe,{0,1,0,3,36,PatternEffect::Damage},
    ItemAction::Material,15,1,false,12,0,0,0,0,SoundId::ToeKick};
bool kickable_bomb(const Entity& shot) {
    if (shot.kind!=EntityKind::Projectile || shot.counter_a!=0 || shot.timer_a<=0) return false;
    const auto kind=static_cast<ProjectileKind>(shot.label_a);
    return kind==ProjectileKind::Bomb || kind==ProjectileKind::Firecracker ||
        (kind==ProjectileKind::Mixture && shot.ground_item.kind==ItemKind::PitchBomb);
}
bool punt_bomb(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    // Prefer the shortest fuse in front, then underfoot. Reuse the real shot:
    // no new fuse, item, blast strength or pool slot is created by a kick.
    for (Cell cell:{user.cell+direction,user.cell}) {
        int chosen=-1,shortest=10000;
        for (int i=0;i<max_entities;++i) {
            const Entity& shot=game.entities[static_cast<std::size_t>(i)];
            if (shot.cell==cell && kickable_bomb(shot) && shot.timer_a<shortest) {
                chosen=i;shortest=shot.timer_a;
            }
        }
        if (chosen<0 || projectile_blocked(game,cell+direction)) continue;
        Entity& shot=game.entities[static_cast<std::size_t>(chosen)];
        shot.facing=direction;shot.counter_a=shot.attack_interval=2;
        shot.timer_b=projectile_step_ticks(shot);
        shot.point_a=user.cell;shot.entity_a={slot,user.generation};
        emit_sound(game,SoundId::ToePunt,cell);return true;
    }
    return false;
}
bool kickable_body(const Game& game,const Entity& body) {
    if (body.hard_blocker || body.toss.ticks || body.vitals.grip || body.vitals.rooted ||
        body.kind==EntityKind::Sled || ridden_sled(game,body)) return false;
    if (body.kind==EntityKind::GroundItem)
        return body.ground_item.count>0 && body.ground_item.flight.slot<0 && !sled_cargo(game,body);
    return body.health>0 && body.move_interval>0 && body.kind!=EntityKind::Train;
}
}
const RegionalItem* steel_toe_item(ItemKind kind) {return kind==ItemKind::SteelToeCap ? &cap : nullptr;}
bool use_steel_toe(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::SteelToeCap || item.uses<=0 || item.cooldown>0 || distance({},direction)!=1) return false;
    const Cell source=user.cell,front=source+direction;
    const Handle owner{slot,user.generation};
    if (punt_bomb(game,slot,direction)) return true;
    int target=entity_at(game,front,true);
    if (target<0) for (int i=0;i<max_entities;++i)
        if (game.entities[static_cast<std::size_t>(i)].kind==EntityKind::GroundItem && game.entities[static_cast<std::size_t>(i)].cell==front) {target=i;break;}
    if (target<0) return false;
    Entity& body=game.entities[static_cast<std::size_t>(target)];
    if (body.kind==EntityKind::RailCart) return push_rail_cart(game,target,direction,owner);
    if (!kickable_body(game,body)) return false;
    const Handle victim{target,body.generation};
    const bool loose=body.kind==EntityKind::GroundItem,blocked=blocks_facing(body,source);
    if (!loose) damage_entity(game,target,item_pattern(item).damage,source,true,owner);
    if (blocked) return true;
    if (body.kind==EntityKind::Player) cancel_item_action(body);
    bool moved=false;
    for (int step=0;step<2;++step) {
        Entity* actor=get_entity(game,victim);
        if (!actor || (!loose && actor->health<=0)) break;
        const Cell next=actor->cell+direction;
        const Tile* tile=game.stage.at(next);
        const int occupant=entity_at(game,next,true);
        if (!tile || !walkable(*tile) || occupant>=0) {
            // A wall bruises, rather than invoking the ordinary lethal crush.
            // Another creature stops the kick without becoming a crusher.
            if (!loose && (!tile || !walkable(*tile))) {
                damage_entity(game,target,6,source,false,owner);
                emit_sound(game,SoundId::ToeWall,actor->cell);
            }
            break;
        }
        bool occupied=false;
        if (loose) for (const Entity& other:game.entities)
            if (other.kind==EntityKind::GroundItem && other.cell==next) {occupied=true;break;}
        if (occupied) break;
        if (loose) stop_item_float(game,*actor);
        actor->cell=next;actor->vitals.slide_momentum=0;moved=true;
        enter_actor_cell(game,target);
        actor=get_entity(game,victim);
        if (!actor || (!loose && actor->health<=0) || actor->cell!=next || actor->toss.ticks || actor->vitals.rooted) break;
        actor->move_wait=std::max(actor->move_wait,actor->move_interval);
    }
    if (moved && !loose) if (const Entity* actor=get_entity(game,victim);actor && actor->health>0 && !actor->toss.ticks)
        slip_on_surface(game,target,direction);
    return moved || !loose;
}
