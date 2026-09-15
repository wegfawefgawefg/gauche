#include "rivet_gun.hpp"
#include "action.hpp"
#include "firearms.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem gun{"Rivet Gun", "Three traveling rivets, 9 damage each, 0.15s apart. Burst locks aim. Reload a 12-round magazine.",
    Sprite::RivetGun,{1,8,0,9,36,PatternEffect::Damage,true},
    ItemAction::Gun,30,1,false,0,12,24,90,0,SoundId::RivetFire};
void finish(Entity& user) { user.label_b=0; user.counter_a=0; user.counter_b=0; user.ground_item={}; }
}
bool rivet_burst_active(const Entity& user) { return user.kind==EntityKind::Player && user.label_b>rivet_action_base && user.label_b<=rivet_action_base+quick_slots; }
const RegionalItem* rivet_gun_item(ItemKind kind) { return kind==ItemKind::RivetGun ? &gun : nullptr; }

// PLAYER SLOTS: label_b = base + committed slot + 1; counter_a inter-shot ticks,
// counter_b unfired rounds (1..2). Outside a burst counter_b is the shared cancel
// latch. point_b locks direction; ground_item preserves the committed variant.
bool step_rivet_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    const bool active=rivet_burst_active(user), held=item.kind==ItemKind::RivetGun;
    if (active && (!held || user.label_b!=rivet_action_base+user.inventory.selected+1 ||
        user.ground_item.attribute!=item.attribute)) cancel_item_action(user);
    if (!held) return false;
    if (input.cancel_use || input.drop || input.interact) { cancel_item_action(user); return true; }
    if (input.reload) {
        if (rivet_burst_active(user)) cancel_item_action(user);
        reload_held_item(game,slot); return true;
    }
    if (rivet_burst_active(user)) {
        if (--user.counter_a>0) return true;
        if (!fire_weapon(game,slot,user.point_b,item) || --user.counter_b==0) finish(user);
        else user.counter_a=9;
        return true;
    }
    if (user.label_b!=0) cancel_item_action(user);
    if (!input.use) { user.counter_b=0; return true; }
    if (user.counter_b!=0 || item.cooldown>0) return true;
    const int rounds=std::min(3,item.loaded);
    if (!fire_weapon(game,slot,user.facing,item) || rounds<=1) return true;
    user.label_b=rivet_action_base+user.inventory.selected+1;
    user.counter_a=9; user.counter_b=rounds-1;
    user.point_b=user.facing; user.ground_item=item;
    return true;
}
bool valid_rivet_action(const Entity& actor) {
    if (actor.kind!=EntityKind::Player || !rivet_burst_active(actor)) return true;
    return actor.counter_a>=1 && actor.counter_a<=9 && actor.counter_b>=1 && actor.counter_b<=2 &&
        actor.ground_item.kind==ItemKind::RivetGun && distance({},actor.point_b)==1;
}
