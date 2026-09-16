#include "ice_melee.hpp"

namespace {
// Ordinary fighting tools: the axe opens ice routes; the pike rewards spacing
// near slippery banks, chasms and walls. Neither needs fuel or a matching machine.
constexpr RegionalItem axe{"Ice Axe", "Cleave three cells for 16. Double damage to ice walls and blocks; dig power 1. 120 swings. Glue repairs it.",
    Sprite::IceAxe,{1,1,0,16,32,PatternEffect::Damage,false,1},
    ItemAction::Melee,24,1,false,0,0,0,0,1,SoundId::IceAxeSwing,120};
constexpr RegionalItem pike{"Tusk Pike", "Thrust two cells for 18; the first body stops it. Only a tip hit shoves one tile. Walls crush; pits swallow. 100 swings; repairable.",
    Sprite::TuskPike,{1,2,0,18,30,PatternEffect::Damage},
    ItemAction::Melee,23,1,false,0,0,0,0,0,SoundId::TuskPikeThrust,100};
}

const RegionalItem* ice_melee_item(ItemKind kind) {
    if (kind==ItemKind::IceAxe) return &axe;
    if (kind==ItemKind::TuskPike) return &pike;
    return nullptr;
}
