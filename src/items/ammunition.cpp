#include "ammunition.hpp"
#include "catalog.hpp"
#include "../item_attribute.hpp"

#include <algorithm>
#include <limits>

int ammunition_refill(ItemKind kind) {
    switch (kind) {
    case ItemKind::SMG: return 18;
    case ItemKind::RocketLauncher: return 1;
    case ItemKind::Shotgun: return 4;
    case ItemKind::Pistol: return 18;
    case ItemKind::Musket: return 5;
    case ItemKind::Bow: return 12;
    case ItemKind::Crossbow: return 8;
    case ItemKind::Blunderbuss: return 3;
    case ItemKind::LensCarbine: return 6;
    case ItemKind::HarpoonGun: return 4;
    case ItemKind::RivetGun: return 12;
    default: break;
    }
    // FUTURE GUNS: A single magazine is a conservative default until tuned.
    const auto* spec=regional_item(kind);
    return spec && spec->action==ItemAction::Gun ? std::max(1,spec->magazine) : 0;
}

bool supply_ammunition(Inventory& inventory) {
    bool supplied=false;
    for (Item& weapon:inventory.slots) {
        if (!item_is_gun(weapon.kind)) continue;
        int& reserve=weapon.kind==ItemKind::Bow ? weapon.loaded : weapon.spare;
        const int amount=std::min(ammunition_refill(weapon.kind),
            std::numeric_limits<int>::max()-reserve);
        if (amount<=0) continue;
        reserve+=amount;
        supplied=true;
    }
    return supplied;
}
