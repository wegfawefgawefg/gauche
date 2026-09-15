#include "rail_shunter.hpp"
#include "rail_cart.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

// SLOTS: entity_a assigned cart; point_a committed stance, point_b cart/strike cell.
// label_a work/bell/swing/recovery, timer_a phase. c slots retain hearing memory.
namespace {
void rest(Entity& actor,int ticks) {actor.label_a=ShunterRest;actor.timer_a=ticks;}
bool controlled(const Entity& actor) {
    return actor.sleep_ticks>0 || actor.stun_ticks>0 || actor.freeze_ticks>0 || actor.vitals.rooted>0 || actor.toss.ticks>0;
}
bool track(const Game& game,Cell cell) {return game.stage.at_or_border(cell).kind==TileKind::Rail && walkable(game.stage.at_or_border(cell));}
Entity* find_cart(Game& game,Entity& actor) {
    if (Entity* cart=get_entity(game,actor.entity_a);cart && cart->kind==EntityKind::RailCart && cart->health>0) return cart;
    actor.entity_a={};int best=11;
    for (int i=0;i<max_entities;++i) {
        const Entity& cart=game.entities[static_cast<std::size_t>(i)];
        if (cart.kind!=EntityKind::RailCart || cart.health<=0 || distance(actor.cell,cart.cell)>=best) continue;
        bool assigned=false;
        for (const Entity& other:game.entities)
            if (&other!=&actor && other.kind==EntityKind::RailShunter && other.health>0 && other.entity_a==Handle{i,cart.generation}) {assigned=true;break;}
        if (!assigned) {best=distance(actor.cell,cart.cell);actor.entity_a={i,cart.generation};}
    }
    return get_entity(game,actor.entity_a);
}
}
void init_rail_shunter(Entity& actor) {
    actor.health=actor.max_health=120;actor.impassable=true;actor.move_interval=24;actor.sprite=Sprite::RailShunter;
}
void interrupt_rail_shunter(Entity& actor) {
    if (actor.kind==EntityKind::RailShunter && (actor.label_a==ShunterBell || actor.label_a==ShunterSwing)) rest(actor,60);
}
void step_rail_shunter(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (controlled(actor)) {interrupt_rail_shunter(actor);return;}
    if (actor.label_a==ShunterRest) {if (!actor.timer_a) actor.label_a=ShunterWork;return;}
    if (actor.label_a!=ShunterWork) {
        Entity* cart=get_entity(game,actor.entity_a);
        if (actor.cell!=actor.point_a || (actor.label_a==ShunterBell && (!cart || cart->kind!=EntityKind::RailCart ||
            cart->health<=0 || cart->cell!=actor.point_b || cart->label_a!=0 || cart->timer_a>0))) {interrupt_rail_shunter(actor);return;}
        if (actor.timer_a>0) return;
        if (actor.label_a==ShunterBell) push_rail_cart(game,actor.entity_a.slot,actor.facing,{slot,actor.generation});
        else resolve_enemy_attack(game,slot,12,SoundId::ShunterSwing);
        rest(actor,60);return;
    }
    const auto target=enemy_target(game,actor.cell,1);
    if (target && distance(actor.cell,target->cell)==1) {
        actor.point_a=actor.cell;actor.point_b=target->cell;actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.label_a=ShunterSwing;actor.timer_a=36;emit_sound(game,SoundId::ShunterWarn,actor.cell);return;
    }
    Entity* cart=find_cart(game,actor);
    if (!cart) {if (const auto foe=enemy_target(game,actor.cell,8)) pursue(game,slot,foe->cell);return;}
    if (cart->label_a!=0 || cart->timer_a>0) return;
    // Try forward, reverse, then a corner; never start into an anchored body.
    for (Cell dir:{cart->facing,Cell{-cart->facing.x,-cart->facing.y},Cell{-cart->facing.y,cart->facing.x},Cell{cart->facing.y,-cart->facing.x}}) {
        const Cell ahead=cart->cell+dir,stance=cart->cell-dir;
        if (!track(game,ahead) || !walkable(game.stage.at_or_border(stance))) continue;
        const int obstruction=entity_at(game,ahead,true);
        if (obstruction>=0 && game.entities[static_cast<std::size_t>(obstruction)].hard_blocker) continue;
        if (actor.cell!=stance) {
            if (actor.move_wait>0) return;
            if (entity_at(game,stance,true)>=0) continue;
            if (const auto next=next_route_cell(game,slot,stance,512)) {willing_step(game,slot,*next);return;}
            continue;
        }
        actor.facing=dir;actor.point_a=actor.cell;actor.point_b=cart->cell;
        actor.label_a=ShunterBell;actor.timer_a=48;emit_sound(game,SoundId::ShunterBell,actor.cell);return;
    }
}
bool valid_rail_shunter(const Entity& actor) {
    return actor.kind!=EntityKind::RailShunter || (actor.label_a>=ShunterWork && actor.label_a<=ShunterRest && actor.timer_a<=60);
}
