#include "doorstop.hpp"

namespace {
constexpr RegionalItem wedge{"Emergency Doorstop",
    "Jam an OPEN gate until broken or picked up. Cannot open locks. Recover from your feet or ahead.",
    Sprite::EmergencyDoorstop,{1,1,0,0,24,PatternEffect::Utility},
    ItemAction::Material,21,1,true,0,0,0,0,0,SoundId::WedgeFit,25};
}
const RegionalItem* doorstop_item(ItemKind kind) {
    return kind == ItemKind::EmergencyDoorstop ? &wedge : nullptr;
}
