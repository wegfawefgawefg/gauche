#include "catalog.hpp"
namespace {
struct Definition { const char* name; const char* description; const char* summary; Sprite icon; };
constexpr Definition definitions[]{
    {"None", "", "", Sprite::Fist},
    {"All Piercing", "Straight through.", "Shots pierce actors and armor", Sprite::PowerPierce},
    {"Reflector", "Return to sender.", "25%: return half a survived hit", Sprite::PowerReflect},
    {"Hearth", "Enough to share.", "Cooked meal: nearby friends +3 HP", Sprite::PowerHearth},
    {"Fleet Feet", "Places to be.", "Movement speed +5% / stack", Sprite::PowerSpeed},
    {"Strong Arms", "Put your back into it.", "Damage +25%; action time +15%", Sprite::PowerStrong},
    {"Quick Hands", "Light work.", "Use speed +20%; damage /1.10", Sprite::PowerQuick},
    {"HP Up", "More of you.", "Maximum HP +20 / stack", Sprite::PowerHealth},
    {"Dodge", "Missed me.", "Dodge: stacks / (stacks + 19)", Sprite::PowerDodge},
    {"Regeneration", "Still ticking.", "1 HP / 20 seconds / stack", Sprite::PowerRegen},
    {"Critical Chance", "Found a soft spot.", "Critical chance +5% / stack", Sprite::PowerCrit},
    {"Critical Power", "Right in the soft spot.", "Critical multiplier +1x / stack", Sprite::PowerCritPower},
    {"Armor", "A thicker skin.", "Damage taken: 10 / (10 + stacks)", Sprite::PowerArmor},
    {"Technical", "Good as new. Better, even.", "Picked-up tool condition +50%", Sprite::PowerTechnical},
    {"Medic", "Doctor-ish.", "Healing items +25% / stack", Sprite::PowerMedic},
    {"Spare Throw", "Another one.", "Keep throw: stacks / (stacks + 4)", Sprite::PowerReuse},
    {"Golddigger", "Shiny.", "Gold pickup value +25% / stack", Sprite::PowerGold},
    {"Oversized", "A bit much.", "Basic action: reach / parry window", Sprite::PowerBig},
    {"Sweeping", "Room for everyone.", "Basic action: wider contact", Sprite::PowerSweep},
    {"Iron", "Clang.", "Basic contact +4; harder impacts", Sprite::PowerIron},
    {"Chef's Touch", "Well done.", "Basic actions cook raw food", Sprite::PowerChef},
    {"God Fist", "An unreasonable hand.", "Replaces basic action; huge launch", Sprite::GodFist},
};
static_assert(std::size(definitions)==static_cast<std::size_t>(ArtifactKind::Count));
const Definition& definition(ArtifactKind kind) {
    const auto i=static_cast<std::size_t>(kind);
    return definitions[i<std::size(definitions) ? i : 0];
}
}
const char* artifact_name(ArtifactKind kind) {return definition(kind).name;}
const char* artifact_description(ArtifactKind kind) {return definition(kind).description;}
const char* artifact_summary(ArtifactKind kind) {return definition(kind).summary;}
Sprite artifact_icon(ArtifactKind kind) {return definition(kind).icon;}
