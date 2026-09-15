#include "insulated_boots.hpp"
namespace {
constexpr RegionalItem boots{"Insulated Boots",
    "Block shocks carried through water and wire for 4s; steps take twice as long. Direct arc contact still hurts. Six activations; can stow after fitting.",
    Sprite::InsulatedBoots,{0,0,0,0,45,PatternEffect::Utility},ItemAction::Material,20,1,false,6,0,0,0,0,SoundId::InsulatorFit};
}
const RegionalItem* insulated_boots_item(ItemKind kind) {return kind==ItemKind::InsulatedBoots ? &boots : nullptr;}
bool use_insulated_boots(Entity& user) {
    if (user.health<=0 || user.vitals.floor_insulation>0) return false;
    user.vitals.floor_insulation=240;return true;
}
