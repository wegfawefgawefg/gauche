#include "game.hpp"

#include <algorithm>

Item* Inventory::held() { return &slots[static_cast<std::size_t>(selected)]; }
const Item* Inventory::held() const { return &slots[static_cast<std::size_t>(selected)]; }

Item make_item(ItemKind kind, int count) {
    Item item{kind, count};
    switch (kind) {
    case ItemKind::Buckler: item.durability = 30; break;
    case ItemKind::Pistol: item.loaded = 12; item.spare = 48; break;
    case ItemKind::Musket: item.loaded = 1; item.spare = 12; break;
    case ItemKind::Bow: item.loaded = 1; item.spare = 20; break;
    case ItemKind::RocketLauncher: item.loaded = 1; item.spare = 4; break;
    case ItemKind::Shotgun: item.loaded = 6; item.spare = 30; break;
    case ItemKind::SMG: item.loaded = 30; item.spare = 120; break;
    default: break;
    }
    return item;
}

Sprite item_sprite(ItemKind kind) {
    switch (kind) {
    case ItemKind::Wall: return Sprite::Wall;
    case ItemKind::Medkit: return Sprite::Medkit;
    case ItemKind::Bandage: return Sprite::Bandage;
    case ItemKind::Bandaid: return Sprite::Bandaid;
    case ItemKind::ConductorHat: return Sprite::ConductorHat;
    case ItemKind::Buckler: return Sprite::Buckler;
    case ItemKind::Pistol: return Sprite::Pistol;
    case ItemKind::Musket: return Sprite::Musket;
    case ItemKind::Bow: return Sprite::Bow;
    case ItemKind::RocketLauncher: return Sprite::RocketLauncher;
    case ItemKind::Ammo: return Sprite::Ammo;
    case ItemKind::Bomb: return Sprite::Bomb;
    case ItemKind::SleepMeds: return Sprite::SleepMeds;
    case ItemKind::Stick: return Sprite::Stick;
    case ItemKind::Shotgun: return Sprite::Shotgun;
    case ItemKind::SMG: return Sprite::SMG;
    case ItemKind::BearTrap: return Sprite::BearTrap;
    case ItemKind::Mine: return Sprite::Mine;
    case ItemKind::Pickaxe: return Sprite::Pickaxe;
    case ItemKind::RawMeat: return Sprite::RawMeat;
    case ItemKind::CookedMeat: return Sprite::CookedMeat;
    default: return Sprite::Fist;
    }
}

const char* item_name(ItemKind kind) {
    switch (kind) {
    case ItemKind::None: return "Empty";
    case ItemKind::Wall: return "Wall";
    case ItemKind::Medkit: return "Medkit";
    case ItemKind::Bandage: return "Bandage";
    case ItemKind::Bandaid: return "Bandaid";
    case ItemKind::Fist: return "Fist";
    case ItemKind::ConductorHat: return "Conductor Hat";
    case ItemKind::Buckler: return "Buckler";
    case ItemKind::Pistol: return "Pistol";
    case ItemKind::Musket: return "Musket";
    case ItemKind::Bow: return "Bow";
    case ItemKind::RocketLauncher: return "Rocket Launcher";
    case ItemKind::Ammo: return "Ammo";
    case ItemKind::Bomb: return "Bomb";
    case ItemKind::SleepMeds: return "Sleep Meds";
    case ItemKind::Stick: return "Stick";
    case ItemKind::Shotgun: return "Shotgun";
    case ItemKind::SMG: return "SMG";
    case ItemKind::BearTrap: return "Bear Trap";
    case ItemKind::Mine: return "Mine";
    case ItemKind::Pickaxe: return "Pickaxe";
    case ItemKind::RawMeat: return "Raw Meat";
    case ItemKind::CookedMeat: return "Cooked Meat";
    }
    return "Unknown";
}

bool insert_item(Inventory& inventory, Item item) {
    if (item.kind == ItemKind::None || item.count <= 0) return false;
    const Inventory original = inventory;
    const bool stackable = item.kind == ItemKind::Wall || item.kind == ItemKind::Medkit ||
        item.kind == ItemKind::Bandage || item.kind == ItemKind::Bandaid ||
        item.kind == ItemKind::Bomb || item.kind == ItemKind::SleepMeds ||
        item.kind == ItemKind::BearTrap || item.kind == ItemKind::Mine ||
        item.kind == ItemKind::RawMeat || item.kind == ItemKind::CookedMeat;
    const int maximum = item.kind == ItemKind::Wall ? 99 : 20;
    if (stackable) {
        for (Item& slot : inventory.slots) {
            if (slot.kind != item.kind || slot.count >= maximum) continue;
            const int transfer = std::min(maximum - slot.count, item.count);
            slot.count += transfer;
            item.count -= transfer;
            if (item.count == 0) return true;
        }
    }
    for (Item& slot : inventory.slots) {
        if (slot.kind == ItemKind::None) {
            slot = item;
            return true;
        }
    }
    inventory = original;
    return false;
}
