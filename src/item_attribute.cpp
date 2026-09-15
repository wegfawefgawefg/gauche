#include "item_attribute.hpp"
#include "items/catalog.hpp"

bool item_is_gun(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->action == ItemAction::Gun;
    return kind == ItemKind::Pistol || kind == ItemKind::Musket ||
           kind == ItemKind::Bow || kind == ItemKind::RocketLauncher ||
           kind == ItemKind::Shotgun || kind == ItemKind::SMG;
}

bool item_is_melee(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->action == ItemAction::Melee;
    return kind == ItemKind::Fist || kind == ItemKind::Stick ||
           kind == ItemKind::Pickaxe;
}

const char* item_attribute_name(ItemAttribute attribute) {
    switch (attribute) {
    case ItemAttribute::None: return "";
    case ItemAttribute::Strong: return "Strong";
    case ItemAttribute::Agile: return "Agile";
    case ItemAttribute::Durable: return "Durable";
    case ItemAttribute::Fragile: return "Fragile";
    case ItemAttribute::Heavy: return "Heavy";
    case ItemAttribute::Big: return "Big";
    case ItemAttribute::Long: return "Long";
    case ItemAttribute::Piercing: return "Piercing";
    case ItemAttribute::Restorative: return "Restorative";
    }
    return "";
}

const char* item_attribute_effect(ItemAttribute attribute) {
    switch (attribute) {
    case ItemAttribute::None: return "";
    case ItemAttribute::Strong: return "DMG +25%";
    case ItemAttribute::Agile: return "COOLDOWN -25%";
    case ItemAttribute::Durable: return "CONDITION X2";
    case ItemAttribute::Fragile: return "DMG +40%, HALF CONDITION";
    case ItemAttribute::Heavy: return "DMG +33%, CD +50%";
    case ItemAttribute::Big: return "WIDER EFFECT AREA";
    case ItemAttribute::Long: return "LONGER REACH";
    case ItemAttribute::Piercing: return "SHOTS PIERCE ACTORS";
    case ItemAttribute::Restorative: return "HEAL +50%, CD +25%";
    }
    return "";
}

std::string item_display_name(const Item& item) {
    const std::string name = item.flame_ticks > 0 ? "Lit Stick" : item_name(item.kind);
    if (item.attribute == ItemAttribute::None) return name;
    return std::string{item_attribute_name(item.attribute)} + " " + name;
}

bool item_accepts_attribute(ItemKind kind, ItemAttribute attribute) {
    if ((kind==ItemKind::ArcTorch || kind==ItemKind::PocketDrill) && attribute==ItemAttribute::Long) return false;
    if (kind == ItemKind::EmergencyFoam || kind == ItemKind::QuarryCharge || kind == ItemKind::GroundingSpike) return attribute == ItemAttribute::None;
    const RegionalItem* spec = regional_item(kind);
    const bool weapon = item_is_gun(kind) || item_is_melee(kind) ||
        (spec != nullptr && spec->pattern.damage > 0);
    const bool condition = kind == ItemKind::Stick || kind == ItemKind::Pickaxe ||
        (spec != nullptr && (spec->uses > 0 || spec->durability > 0));
    switch (attribute) {
    case ItemAttribute::None: return true;
    case ItemAttribute::Strong: case ItemAttribute::Agile:
    case ItemAttribute::Heavy: return weapon;
    case ItemAttribute::Durable: return condition || kind == ItemKind::Buckler || (spec && spec->durability > 0);
    case ItemAttribute::Fragile: return condition && weapon;
    case ItemAttribute::Big:
        return kind == ItemKind::HeatSiphon || kind == ItemKind::SteamKettle || kind == ItemKind::SnowScoop || kind == ItemKind::GritPouch || kind == ItemKind::ThrowingNet || kind == ItemKind::ThornCaltrops || item_is_melee(kind) || (spec != nullptr && spec->pattern.blast_radius > 0) || kind == ItemKind::Bomb ||
               kind == ItemKind::RocketLauncher;
    case ItemAttribute::Long: return kind != ItemKind::ThawCharge && (kind == ItemKind::ChainHook || kind == ItemKind::HorseshoeMagnet || kind == ItemKind::ForemanWhistle || kind == ItemKind::EchoPebble || kind == ItemKind::SignalFlare || kind == ItemKind::FishingLine || kind == ItemKind::ColdFlask || kind == ItemKind::WolfWhistle || kind == ItemKind::Firecracker || kind == ItemKind::StinkBomb || kind == ItemKind::RottenFruit || (weapon && kind != ItemKind::AcornMine && kind != ItemKind::EelBattery) || kind == ItemKind::RopeHook || kind == ItemKind::BlinkSeed || kind == ItemKind::ThrowingNet);
    case ItemAttribute::Piercing:
        return item_is_gun(kind) && kind != ItemKind::RocketLauncher;
    case ItemAttribute::Restorative:
        return kind == ItemKind::Medkit || kind == ItemKind::Bandage ||
               kind == ItemKind::Bandaid || kind == ItemKind::CookedMeat ||
               (spec != nullptr && spec->action == ItemAction::Food && kind != ItemKind::SaltedKelp);
    }
    return false;
}
