#include "pocket_door.hpp"
#include "../entities/pocket_door.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem door{"Pocket Door", "Place two linked thresholds. Step through; others can follow. Cast across ordinary walls. Sealed arenas hold.",
    Sprite::PocketDoor, {1, 3, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 45, 1, false, 2, 0, 0, 0, 0, SoundId::PocketPlace};
}

const RegionalItem* forest_pocket_door(ItemKind kind) {
    return kind == ItemKind::PocketDoor ? &door : nullptr;
}

std::optional<Cell> pocket_door_landing(const Game& game, Cell from, Cell direction, int reach) {
    std::optional<Cell> landing;
    Cell cell = from;
    for (int step = 1; step <= reach; ++step) {
        cell = cell + direction;
        const Tile* tile = game.stage.at(cell);
        if (!tile || (tile->kind == TileKind::Wall && tile->break_rule == BreakRule::Unbreakable)) break;
        bool sealed = false;
        for (const Entity& fixture : game.entities)
            if (fixture.kind == EntityKind::EncounterGate && fixture.impassable && fixture.cell == cell) sealed = true;
        if (sealed || !pocket_passage_allowed(game, from, cell)) break;
        // UNFOLD: The furthest clear dry cell wins; never bury a prop, item or fixture.
        if (buildable(tile->kind) && !surface_wet(*tile) && tile->surface.fire_ticks == 0 &&
            (tile->prop.kind == PropKind::None || tile->prop.broken) && entity_at(game, cell) < 0)
            landing = cell;
    }
    return landing;
}

bool place_pocket_door(Game& game, int slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *user.inventory.held();
    const auto cell = pocket_door_landing(game, user.cell, direction, item_pattern(item).maximum);
    if (!cell) return false;
    Entity* first = get_entity(game, item.anchor);
    if (first && (first->kind != EntityKind::PocketDoor || first->fixture_open)) return false;
    if (!first && item.anchor.slot >= 0) fold_unused_door(item);
    const Handle handle = spawn_entity(game, EntityKind::PocketDoor, *cell);
    Entity* next = get_entity(game, handle);
    if (!next) return false;
    if (!first) {
        item.anchor = handle;
        return true;
    }
    first->entity_a = handle;
    next->entity_a = item.anchor;
    for (Entity* endpoint : {first, next}) {
        endpoint->fixture_open = true;
        endpoint->sprite = Sprite::PocketThresholdOpen;
        endpoint->light = {3, 280, {125, 177, 237}};
        emit_sound(game, SoundId::PocketLink, endpoint->cell);
    }
    item.anchor = {};
    return true;
}

void fold_unused_door(Item& item) {
    if (item.kind == ItemKind::PocketDoor && item.anchor.slot >= 0)
        item.uses = std::min(item.max_uses, item.uses + 1);
    item.anchor = {};
}
