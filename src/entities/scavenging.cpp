#include "scavenging.hpp"
#include "behavior.hpp"
#include "../world/ground_items.hpp"

bool scavenger_food(ItemKind kind) {
    return kind == ItemKind::RawMeat || kind == ItemKind::CookedMeat ||
        kind == ItemKind::Egg || kind == ItemKind::FriedEgg || kind == ItemKind::FungalBread || kind == ItemKind::RottenFruit;
}

Handle find_scavenge(const Game& game, int slot, bool food_only, int radius) {
    const Entity& scavenger = game.entities[static_cast<std::size_t>(slot)];
    Handle best;
    for (int i = 0; i < max_entities; ++i) {
        const Entity& loose = game.entities[static_cast<std::size_t>(i)];
        const Item& item = loose.ground_item;
        if (loose.kind != EntityKind::GroundItem || item.count <= 0 ||
            item.kind == ItemKind::None || item.kind == ItemKind::Fist ||
            item.flame_ticks > 0 || (food_only && !scavenger_food(item.kind))) continue;
        const int gap = distance(scavenger.cell, loose.cell);
        if (gap > radius) continue;
        const int occupied = entity_at(game, loose.cell, true);
        if (occupied >= 0 && occupied != slot) continue;
        // REACHABILITY: Food behind a sealed wall must not occupy a scavenger forever.
        if (gap > 0 && !next_route_cell(game, slot, loose.cell, 256)) continue;
        best = {i, loose.generation}; radius = gap;
    }
    return best;
}

bool collect_scavenge(Game& game, int slot, Handle handle, bool one) {
    Entity& scavenger = game.entities[static_cast<std::size_t>(slot)];
    Entity* loose = get_entity(game, handle);
    Item& held = *scavenger.inventory.held();
    if (held.count > 0 || loose == nullptr || loose->kind != EntityKind::GroundItem ||
        loose->cell != scavenger.cell || loose->ground_item.count <= 0) return false;
    held = loose->ground_item;
    if (one) held.count = 1;
    loose->ground_item.count -= held.count;
    if (loose->ground_item.count == 0) remove_entity(game, handle);
    emit_sound(game, SoundId::ScavengeTake, scavenger.cell);
    return true;
}

void drop_scavenged_items(Game& game, const Entity& scavenger) {
    for (const Item& item : scavenger.inventory.slots) {
        if (item.kind == ItemKind::None || item.count <= 0) continue;
        Entity* ground = get_entity(game, spawn_entity(game, EntityKind::GroundItem,
            nearby_ground_item_cell(game, scavenger.cell)));
        if (ground == nullptr) return;
        // INSTANCE: Preserve ammunition, wear, modifiers and remaining charges.
        ground->ground_item = item;
        ground->sprite = item_sprite(item);
    }
}
