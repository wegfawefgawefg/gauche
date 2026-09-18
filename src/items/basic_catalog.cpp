#include "basic_actions.hpp"
#include "action.hpp"
#include <algorithm>
namespace {
constexpr RegionalItem slap{"Slap","Get a grip.",Sprite::Slap,{1,1,0,1,24,PatternEffect::Damage},ItemAction::Melee,0,1,false,0,0,0,0,0,SoundId::SlapHit};
constexpr RegionalItem pan{"Parry Pan","Not for cooking.",Sprite::ParryPan,{1,1,0,0,42,PatternEffect::Utility},ItemAction::Material,0,1,false,0,0,0,0,0,SoundId::PanRaise};
constexpr RegionalItem jump{"Jump","Mind the landing.",Sprite::Jump,{2,2,0,0,36,PatternEffect::Utility},ItemAction::Material,0,1,false,0,0,0,0,0,SoundId::JumpLift};
constexpr RegionalItem grapple{"Grapple","You. Over there.",Sprite::Grapple,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,0,1,false,0,0,0,0,0,SoundId::GrappleLift};
constexpr RegionalItem shove{"Shove","Excuse me.",Sprite::Shove,{1,1,0,0,42,PatternEffect::Utility},ItemAction::Melee,0,1,false,0,0,0,0,0,SoundId::ShoveHit};
constexpr RegionalItem kick{"Kick","Boot first.",Sprite::Kick,{1,1,0,8,30,PatternEffect::Damage},ItemAction::Melee,0,1,false,0,0,0,0,0,SoundId::KickHit};
constexpr RegionalItem elbow{"Elbow","Behind you.",Sprite::Elbow,{1,1,0,12,30,PatternEffect::Damage,false,1},ItemAction::Melee,0,1,false,0,0,0,0,0,SoundId::ElbowHit};
constexpr RegionalItem god{"God Fist","An unreasonable hand.",Sprite::GodFist,{1,1,0,60,90,PatternEffect::Damage},ItemAction::Melee,0,1,false,0,0,0,0,0,SoundId::GodImpact};
constexpr RegionalItem balloon{"Balloon","Don't let go.",Sprite::Balloon,{0,0,0,0,0,PatternEffect::Utility},ItemAction::Material,24,1,false,0,0,0,0,0,SoundId::AirInflate};
constexpr RegionalItem shield{"Crushing Shield","Make room.",Sprite::CrushShield,{1,1,0,6,36,PatternEffect::Damage},ItemAction::Melee,75,1,false,0,0,0,0,0,SoundId::LanternRaise,300};
}
bool is_basic_action(ItemKind kind) {return std::find(basic_actions.begin(),basic_actions.end(),kind)!=basic_actions.end();}
const RegionalItem* basic_action_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::Slap:return &slap;case ItemKind::ParryPan:return &pan;
    case ItemKind::Jump:return &jump;case ItemKind::Grapple:return &grapple;
    case ItemKind::Shove:return &shove;case ItemKind::Kick:return &kick;
    case ItemKind::Elbow:return &elbow;case ItemKind::GodFist:return &god;
    case ItemKind::Balloon:return &balloon;case ItemKind::CrushShield:return &shield;
    default:return nullptr;
    }
}
void set_basic_action(Entity& player,ItemKind kind) {
    if (!is_basic_action(kind)) return;
    player.basic_action=kind;
    for (auto& item:player.inventory.slots) if (is_basic_action(item.kind)) {
        item=make_item(kind);cancel_item_action(player);player.block_ticks=0;return;
    }
    for (auto& item:player.inventory.slots) if (item.kind==ItemKind::None) {item=make_item(kind);return;}
    // A full test kit gives its first slot to the dedicated action selector.
    player.inventory.slots[0]=make_item(kind);cancel_item_action(player);player.block_ticks=0;
}
