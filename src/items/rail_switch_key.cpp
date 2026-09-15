#include "rail_switch_key.hpp"
#include "../props/rail_points.hpp"
namespace {
constexpr RegionalItem key{"Rail Switch Key",
    "Turn adjacent rail points clockwise to the next track. Twelve turns. Cannot turn points under a cart. Cut or blocked exits stop carts.",
    Sprite::RailSwitchKey,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,9,1,false,12,0,0,0,0,SoundId::PointsTurn};
}
const RegionalItem* rail_switch_key_item(ItemKind kind) {return kind==ItemKind::RailSwitchKey ? &key : nullptr;}
bool use_rail_switch_key(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    return user.inventory.held()->kind==ItemKind::RailSwitchKey && distance({},direction)==1 &&
        turn_rail_points(game,user.cell+direction);
}
