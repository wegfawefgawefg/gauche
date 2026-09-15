#include "machine_fittings.hpp"
#include "pocket_pump.hpp"
#include "../game.hpp"
#include "catalog.hpp"
#include "heat_siphon.hpp"
#include "optics.hpp"
#include "../props/cloth.hpp"
#include "snow_tools.hpp"
#include "mixtures.hpp"
#include "firearms.hpp"
#include <algorithm>

namespace {
int magazine_size(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->magazine;
    switch (kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Shotgun: return 6;
    case ItemKind::SMG: return 30;
    case ItemKind::Musket: case ItemKind::RocketLauncher: return 1;
    default: return 0;
    }
}
}

bool reload_held_item(Game& game, int user_slot) {
    if (game.entities[static_cast<std::size_t>(user_slot)].inventory.held()->kind==ItemKind::NozzleElbow)
        return reverse_nozzle_elbow(game,user_slot);
    if (game.entities[static_cast<std::size_t>(user_slot)].inventory.held()->kind==ItemKind::PocketPump)
        return discharge_pocket_pump(game,user_slot);
    if (game.entities[static_cast<std::size_t>(user_slot)].inventory.held()->kind==ItemKind::HeatSiphon)
        return discharge_siphon(game,user_slot);
    if (uncover_optic(game, user_slot)) return true;
    if (rotate_mirror(game, user_slot)) return true;
    if (game.entities[static_cast<std::size_t>(user_slot)].inventory.held()->kind == ItemKind::SnowScoop)
        return pack_snowball(game, user_slot);
    if (game.entities[static_cast<std::size_t>(user_slot)].inventory.held()->kind == ItemKind::RottenFruit)
        return eat_rotten_fruit(game, user_slot);
    Item& item = *game.entities[static_cast<std::size_t>(user_slot)].inventory.held();
    const int capacity = magazine_size(item.kind);
    if (capacity == 0 || item.loaded >= capacity || item.spare <= 0 || item.cooldown > 0)
        return false;
    const int transfer = std::min(capacity - item.loaded, item.spare);
    item.loaded += transfer;
    item.spare -= transfer;
    const RegionalItem* spec = regional_item(item.kind);
    item.cooldown = spec != nullptr ? spec->reload : item.kind == ItemKind::Pistol ? 45 : 60;
    emit_sound(game, firearm_reload_sound(item.kind), game.entities[static_cast<std::size_t>(user_slot)].cell);
    return true;
}
