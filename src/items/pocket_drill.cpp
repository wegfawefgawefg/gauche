#include "pocket_drill.hpp"
#include "action.hpp"
#include "sled.hpp"
#include "ice_anchor.hpp"
#include "../props/interaction.hpp"
#include "../traps/contact.hpp"
#include "../entities/hearing.hpp"

namespace {
constexpr RegionalItem drill{"Pocket Drill",
    "Hold still for 0.4s, then drill every 0.2s. Moving/turning restarts it. 30 battery beats, even in air; no ammo refill. Loud. Dig power 1.",
    Sprite::PocketDrill,{1,1,0,8,12,PatternEffect::Damage},
    ItemAction::Material,27,1,false,30,0,0,0,1,SoundId::PocketDrillWork};

void pulse(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& held=*user.inventory.held();
    const Item tool=held;
    const Handle instigator{slot,user.generation};
    const Cell origin=user.cell,contact=origin+user.facing;
    const ItemPattern pattern=active_item_pattern(tool,user);
    // Spend before contact. Retaliation/death cannot leave a consumed item
    // referenced by an active action or let the final battery beat repeat.
    held.cooldown=pattern.cooldown;user.use_flash=5;
    if (--held.uses==0) {held={};cancel_item_action(user);emit_sound(game,SoundId::PocketDrillEmpty,origin);}
    const Tile* tile=game.stage.at(contact);
    const bool wall=tile && !walkable(tile->kind);
    const bool prop=tile && prop_blocks(tile->prop);
    const int target=entity_at(game,contact,true);
    const bool freight=tile && tile->kind==TileKind::Rail && tile->hp>0 && !prop && target<0;
    const Handle victim=target<0 ? Handle{} : Handle{target,game.entities[static_cast<std::size_t>(target)].generation};
    emit_sound(game,wall || freight || prop || target>=0 ? SoundId::PocketDrillWork : SoundId::PocketDrillAir,origin);
    make_noise(game,origin,12);
    if (!tile) return;
    if (wall || freight) {hit_terrain(game,contact,origin,pattern.damage,tool.dig_power,TileImpact::Strike,false);return;}
    hit_prop(game,contact,pattern.damage,origin);
    if (prop) return; // This beat cannot hit through the cover it just broke.
    hit_ground_traps(game,contact,pattern.damage,origin);
    hit_sled(game,contact,pattern.damage,origin);
    hit_ice_anchor(game,contact,pattern.damage,origin);
    if (target!=slot && get_entity(game,victim))
        damage_entity(game,victim.slot,pattern.damage,origin,true,instigator);
}
}
const RegionalItem* pocket_drill_item(ItemKind kind) {return kind==ItemKind::PocketDrill ? &drill : nullptr;}
bool pocket_drill_active(const Entity& actor) {
    return actor.kind==EntityKind::Player && actor.label_b>drill_action_base && actor.label_b<=drill_action_base+quick_slots;
}
void interrupt_pocket_drill(Entity& actor) {if (pocket_drill_active(actor)) cancel_item_action(actor);}
// PLAYER SLOTS: label_b base+slot+1; counter_a prime progress; counter_b release
// latch; ground_item committed variant; point_a/point_b stationary cell/facing.
bool step_pocket_drill(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    const bool held=item.kind==ItemKind::PocketDrill;
    if (pocket_drill_active(user) && (!held || user.label_b!=drill_action_base+user.inventory.selected+1 ||
        user.ground_item.attribute!=item.attribute)) cancel_item_action(user);
    if (!held) return false;
    if (input.cancel_use || input.drop || input.interact || input.reload) {cancel_item_action(user);return true;}
    if (!input.use) {cancel_item_action(user);user.counter_b=0;return true;}
    if (!pocket_drill_active(user)) {
        if (user.label_b!=0) cancel_item_action(user);
        if (user.counter_b!=0 || item.cooldown>0 || item.uses<=0) return true;
        user.label_b=drill_action_base+user.inventory.selected+1;
        user.counter_a=0;user.ground_item=item;user.point_a=user.cell;user.point_b=user.facing;
        emit_sound(game,SoundId::PocketDrillStart,user.cell);
    }
    if (user.cell!=user.point_a || user.facing!=user.point_b) {
        user.counter_a=0;user.point_a=user.cell;user.point_b=user.facing;
    }
    if (user.counter_a<drill_prime_ticks && ++user.counter_a<drill_prime_ticks) return true;
    if (item.cooldown==0 && item.uses>0) pulse(game,slot);
    return true;
}
bool valid_pocket_drill(const Entity& actor) {
    if (!pocket_drill_active(actor)) return true;
    return actor.counter_a>=1 && actor.counter_a<=drill_prime_ticks && actor.counter_b==0 &&
        actor.ground_item.kind==ItemKind::PocketDrill && distance({},actor.point_b)==1 &&
        actor.label_b==drill_action_base+actor.inventory.selected+1 &&
        actor.inventory.held()->kind==ItemKind::PocketDrill && actor.inventory.held()->uses>0 &&
        actor.inventory.held()->attribute==actor.ground_item.attribute;
}
