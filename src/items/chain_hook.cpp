#include "chain_hook.hpp"
#include "action.hpp"
#include "../projectiles/chain_hook.hpp"

namespace {
constexpr RegionalItem hook{"Chain Hook",
    "Hold to haul crates, gear, sleds or rail carts. Fixed anchors pull you. Release pauses; secondary cuts. 24 casts.",
    Sprite::ChainHook,{1,5,0,0,36,PatternEffect::Utility,true},
    ItemAction::Material,22,1,false,24,0,0,0,0,SoundId::ChainCast};
}
const RegionalItem* chain_hook_item(ItemKind kind) {return kind==ItemKind::ChainHook ? &hook : nullptr;}
void release_held_chain(Game& game,Entity& owner) {
    const Item& item=*owner.inventory.held();
    if (item.kind==ItemKind::ChainHook) release_chain_hook(game,item.flight);
}
bool step_chain_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::ChainHook) return false;
    if (user.label_b!=0) cancel_item_action(user);
    const bool cut=input.reload && !item.opened;item.opened=input.reload;
    if (!input.use) user.counter_b=0;
    if (input.cancel_use || input.drop || input.interact || cut) {
        release_chain_hook(game,item.flight);user.counter_b=1;return true;
    }
    if (input.reload) return true;
    if (Entity* line=get_entity(game,item.flight)) {line->fixture_open=input.use;return true;}
    item.flight={};
    if (!input.use || user.counter_b || item.cooldown>0) return true;
    if (launch_chain_hook(game,slot,user.facing)) user.counter_b=1;
    return true;
}
