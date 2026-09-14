#include "item_attribute.hpp"

bool item_is_gun(ItemKind kind) {
    return kind == ItemKind::Pistol || kind == ItemKind::Musket ||
           kind == ItemKind::Bow || kind == ItemKind::RocketLauncher ||
           kind == ItemKind::Shotgun || kind == ItemKind::SMG;
}

bool item_is_melee(ItemKind kind) {
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

bool item_accepts_attribute(ItemKind kind, ItemAttribute attribute) {
    const bool weapon = item_is_gun(kind) || item_is_melee(kind);
    const bool condition = kind == ItemKind::Stick || kind == ItemKind::Pickaxe;
    switch (attribute) {
    case ItemAttribute::None: return true;
    case ItemAttribute::Strong: case ItemAttribute::Agile:
    case ItemAttribute::Heavy: return weapon;
    case ItemAttribute::Durable: return condition || kind == ItemKind::Buckler;
    case ItemAttribute::Fragile: return condition;
    case ItemAttribute::Big:
        return item_is_melee(kind) || kind == ItemKind::Bomb ||
               kind == ItemKind::RocketLauncher;
    case ItemAttribute::Long: return weapon;
    case ItemAttribute::Piercing:
        return item_is_gun(kind) && kind != ItemKind::RocketLauncher;
    case ItemAttribute::Restorative:
        return kind == ItemKind::Medkit || kind == ItemKind::Bandage ||
               kind == ItemKind::Bandaid || kind == ItemKind::CookedMeat;
    }
    return false;
}
