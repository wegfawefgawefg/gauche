#include "arc_torch.hpp"
#include "action.hpp"
#include "../surfaces/conduction.hpp"
#include "../props/interaction.hpp"

namespace {
constexpr RegionalItem torch{"Arc Torch",
    "Hold 0.3s, then 7-damage arcs every 0.25s. 32 pulses. Cuts metal grates. Water/wire spread shocks, including back to you; grounding sinks a pulse.",
    Sprite::ArcTorch,{1,1,3,7,15,PatternEffect::Damage,false,0,0,false,false,false,false,true},
    ItemAction::Material,38,1,false,32,0,0,0,0,SoundId::ArcPulse};

void pulse(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& held=*user.inventory.held();
    const Cell source=user.cell,contact=source+user.facing;
    const ItemPattern pattern=item_pattern(held);
    const ConductedShock shock=trace_conducted_shock(game,contact,pattern.blast_radius);
    const Tile* tile=game.stage.at(contact);
    const bool metal=tile && prop_cuttable_metal(tile->prop);
    // Capture the circuit before cutting cover; this pulse cannot pass newly
    // destroyed bars. Ready grounding spikes sink the complete electrical pulse.
    if (metal && shock.wave.ground_node<0) hit_prop(game,contact,pattern.damage*2,source);
    if (shock.wave.count>0) apply_conducted_shock(game,shock,source,pattern.damage);
    else if (game.shot_count<static_cast<int>(game.shots.size()))
        game.shots[static_cast<std::size_t>(game.shot_count++)]={source,contact,true,false,false,true};
    emit_sound(game,metal ? SoundId::ArcCut : SoundId::ArcPulse,source);
    user.use_flash=5;
    held.cooldown=pattern.cooldown;
    // Even an open-air arc consumes battery. Stopping during the prime does not.
    if (--held.uses==0) {
        held={};cancel_item_action(user);
        emit_sound(game,SoundId::ArcEmpty,source);
    }
}
}
const RegionalItem* arc_torch_item(ItemKind kind) { return kind==ItemKind::ArcTorch ? &torch : nullptr; }
bool arc_torch_active(const Entity& actor) {
    return actor.kind==EntityKind::Player && actor.label_b>arc_action_base && actor.label_b<=arc_action_base+quick_slots;
}
// PLAYER SLOTS: label_b is base+held slot+1; counter_a prime progress; counter_b
// is the shared cancellation latch. ground_item records the committed variant.
// Aim stays live between pulses; releasing or switching requires another prime.
bool step_arc_torch(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    const bool held=item.kind==ItemKind::ArcTorch;
    if (arc_torch_active(user) && (!held || user.label_b!=arc_action_base+user.inventory.selected+1 ||
        user.ground_item.attribute!=item.attribute)) cancel_item_action(user);
    if (!held) return false;
    if (input.cancel_use || input.drop || input.interact || input.reload) { cancel_item_action(user); return true; }
    if (!input.use) { cancel_item_action(user);user.counter_b=0;return true; }
    if (!arc_torch_active(user)) {
        if (user.label_b!=0) cancel_item_action(user);
        if (user.counter_b!=0 || item.cooldown>0 || item.uses<=0) return true;
        user.label_b=arc_action_base+user.inventory.selected+1;
        user.counter_a=0;user.ground_item=item;
        emit_sound(game,SoundId::ArcPrime,user.cell);
    }
    if (user.counter_a<arc_prime_ticks && ++user.counter_a<arc_prime_ticks) return true;
    if (item.cooldown==0 && item.uses>0) pulse(game,slot);
    return true;
}
bool valid_arc_torch(const Entity& actor) {
    if (!arc_torch_active(actor)) return true;
    return actor.counter_a>=1 && actor.counter_a<=arc_prime_ticks && actor.counter_b==0 &&
        actor.ground_item.kind==ItemKind::ArcTorch &&
        actor.label_b==arc_action_base+actor.inventory.selected+1 &&
        actor.inventory.held()->kind==ItemKind::ArcTorch &&
        actor.inventory.held()->attribute==actor.ground_item.attribute;
}
