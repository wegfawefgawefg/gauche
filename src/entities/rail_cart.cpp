#include "rail_cart.hpp"
#include "../props/rail_points.hpp"
#include "../items/sled.hpp"
#include "../status/effects.hpp"

// SLOTS: label_a stopped/rolling; timer_a brake, timer_b wheel beat.
// entity_a last pusher for collision attribution; ground_item is the actual cargo.
namespace {
bool rail(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && tile->kind==TileKind::Rail && walkable(*tile);
}
void stop(Game& game,Entity& cart) {
    if (cart.label_a!=0) emit_sound(game,SoundId::CartStop,cart.cell);
    cart.label_a=0;cart.timer_b=0;
}
void gather(Game& game,Entity& cart) {
    if (cart.ground_item.kind!=ItemKind::None) return;
    for (int i=0;i<max_entities;++i) {
        Entity& cargo=game.entities[static_cast<std::size_t>(i)];
        if (cargo.kind!=EntityKind::GroundItem || cargo.cell!=cart.cell || cargo.ground_item.kind==ItemKind::None ||
            cargo.ground_item.count<=0 || cargo.ground_item.flight.slot>=0 || cargo.ground_item.anchor.slot>=0 ||
            cargo.toss.ticks>0 || sled_cargo(game,cargo)) continue;
        cart.ground_item=cargo.ground_item;remove_entity(game,{i,cargo.generation});
        emit_sound(game,SoundId::CartCargo,cart.cell);return;
    }
}
}
void init_rail_cart(Entity& cart) {
    cart.health=cart.max_health=64;cart.impassable=cart.hard_blocker=true;cart.sprite=Sprite::RailCart;
    cart.facing={1,0};
}
bool push_rail_cart(Game& game,int slot,Cell direction,Handle pusher) {
    Entity& cart=game.entities[static_cast<std::size_t>(slot)];
    if (cart.kind!=EntityKind::RailCart || cart.health<=0 || cart.timer_a>0 || distance({},direction)!=1 ||
        !rail(game,cart.cell) || !rail(game,cart.cell+direction)) return false;
    cart.facing=direction;cart.label_a=1;cart.timer_b=8;cart.entity_a=pusher;
    emit_sound(game,SoundId::CartPush,cart.cell);return true;
}
bool haul_rail_cart(Game& game,int slot,Cell direction) {
    Entity& cart=game.entities[static_cast<std::size_t>(slot)];
    const Cell next=cart.cell+direction;
    if (cart.kind!=EntityKind::RailCart || cart.health<=0 || cart.timer_a>0 || distance({},direction)!=1 ||
        !rail(game,next) || entity_at(game,next,true)>=0) return false;
    stop(game,cart);cart.facing=direction;cart.cell=next;gather(game,cart);return true;
}
bool brake_rail_cart(Game& game,Cell cell) {
    for (Entity& cart:game.entities) {
        if (cart.kind!=EntityKind::RailCart || cart.health<=0 || cart.cell!=cell || cart.timer_a>0) continue;
        stop(game,cart);cart.timer_a=360;emit_sound(game,SoundId::BrakeFit,cell);return true;
    }
    return false;
}
void hurt_rail_cart(Game& game,Entity& cart,int damage) {
    if (cart.kind==EntityKind::RailCart && cart.timer_a>0 && damage>=16) {
        cart.timer_a=0;emit_sound(game,SoundId::BrakeSnap,cart.cell);
    }
}
void step_rail_cart(Game& game,int slot) {
    Entity& cart=game.entities[static_cast<std::size_t>(slot)];
    if (cart.timer_a>0 || !rail(game,cart.cell)) {stop(game,cart);return;}
    if (cart.label_a==0) {gather(game,cart);return;}
    if (cart.timer_b>0) return;
    cart.facing=rail_exit(game.stage,cart.cell,cart.facing);
    const Cell next=cart.cell+cart.facing;
    if (!rail(game,next)) {stop(game,cart);return;}
    const int victim=entity_at(game,next,true);
    if (victim>=0) {
        // One impact, then a full stop. No overlap, wall crush or repeated contact damage.
        damage_entity(game,victim,18,cart.cell,true,cart.entity_a);
        emit_sound(game,SoundId::CartImpact,next);stop(game,cart);return;
    }
    cart.cell=next;cart.timer_b=8;gather(game,cart);
    emit_sound(game,SoundId::CartRoll,cart.cell);
}
Item recoverable_cart_item(const Game& game,Cell cell) {
    for (const Entity& cart:game.entities)
        if (cart.kind==EntityKind::RailCart && cart.health>0 && cart.label_a==0 && cart.cell==cell) return cart.ground_item;
    return {};
}
int release_cart_item(Game& game,Cell source,Cell destination) {
    for (Entity& cart:game.entities) {
        if (cart.kind!=EntityKind::RailCart || cart.health<=0 || cart.label_a!=0 || cart.cell!=source ||
            cart.ground_item.kind==ItemKind::None || cart.ground_item.count<=0) continue;
        Entity* loose=get_entity(game,spawn_entity(game,EntityKind::GroundItem,destination));
        if (!loose) return -1;
        loose->ground_item=cart.ground_item;loose->sprite=item_sprite(loose->ground_item);cart.ground_item={};
        emit_sound(game,SoundId::CartCargo,source);return static_cast<int>(loose-game.entities.data());
    }
    return -1;
}
bool finish_cart_death(Game& game,int slot) {
    Entity& cart=game.entities[static_cast<std::size_t>(slot)];
    if (cart.kind!=EntityKind::RailCart || cart.health>0) return false;
    const Entity dead=cart;remove_entity(game,{slot,cart.generation});
    if (dead.ground_item.kind!=ItemKind::None && dead.ground_item.count>0) {
        // Reuse the dead body's slot so a full entity pool cannot eat the real cargo.
        cart.kind=EntityKind::GroundItem;cart.generation=dead.generation+1;cart.birth_tick=game.tick;
        cart.cell=dead.cell;cart.ground_item=dead.ground_item;cart.sprite=item_sprite(cart.ground_item);
    }
    return true;
}
bool valid_rail_cart(const Entity& cart) {
    return cart.kind!=EntityKind::RailCart || (cart.health>0 && cart.health<=64 && cart.max_health==64 &&
        cart.impassable && cart.hard_blocker && cart.move_interval==0 && cart.label_a>=0 && cart.label_a<=1 &&
        cart.timer_a<=360 && cart.timer_b<=8 && distance({},cart.facing)==1 &&
        cart.ground_item.flight.slot<0 && cart.ground_item.anchor.slot<0);
}
