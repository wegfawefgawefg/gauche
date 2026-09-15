#include "ice_equipment.hpp"

namespace {
constexpr RegionalItem blade{"Skate Blade", "Sweep three cells across. A slip adds one forward tip for 0.2s. No digging. Glue repairs the edge.",
    Sprite::SkateBlade, {1,1,0,12,24,PatternEffect::Damage,false,1,0,false,false,false,false,false,1},
    ItemAction::Melee,16,1,false,0,0,0,0,0,SoundId::SkateSlash,36};
constexpr RegionalItem crampons{"Crampons", "Grip ice for 5s; steps take twice as long. Oil still slips; shoves still move you. Six activations.",
    Sprite::Crampons,{0,0,0,0,45,PatternEffect::Utility},
    ItemAction::Material,17,1,false,6,0,0,0,0,SoundId::CramponsFit};
}
const RegionalItem* ice_equipment_item(ItemKind kind) {
    if (kind==ItemKind::SkateBlade) return &blade;
    if (kind==ItemKind::Crampons) return &crampons;
    return nullptr;
}

bool use_crampons(Entity& user) {
    if (user.vitals.traction>0 || user.vitals.grip>0) return false;
    user.vitals.traction=300;
    user.vitals.slide_momentum=0;
    return true;
}
