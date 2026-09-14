#include "woodland_traps.hpp"
#include "../surfaces/interaction.hpp"

namespace {

constexpr RegionalItem snare{"Rope Snare", "Holds feet for 3s; attacks work. Allies can cut or shove you free. Recover after release.",
    Sprite::RopeSnare, {1, 1, 0, 0, 40, PatternEffect::Utility},
    ItemAction::Material, 9, 3, true, 0, 0, 0, 0, 0, SoundId::SnarePlace};
constexpr RegionalItem spring{"Spring Trap", "Launches a grounded actor 2 cells along its arrow. Hard walls crush. One shot; friends count.",
    Sprite::SpringTrap, {1, 1, 0, 0, 45, PatternEffect::Utility},
    ItemAction::Material, 20, 2, true, 0, 0, 0, 0, 0, SoundId::SpringSet};
constexpr RegionalItem acorn{"Acorn Mine", "Step or break to burst four splinter lanes. Walls and bodies stop them. Friends count.",
    Sprite::AcornMine, {1, 1, 2, 22, 45, PatternEffect::Damage, false, 0, 0, false, false, true},
    ItemAction::Material, 16, 3, true, 0, 0, 0, 0, 0, SoundId::AcornSet};

} // namespace

const RegionalItem* forest_trap_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::RopeSnare: return &snare;
    case ItemKind::SpringTrap: return &spring;
    case ItemKind::AcornMine: return &acorn;
    default: return nullptr;
    }
}

bool place_woodland_trap(Game& game, int owner_slot, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const Cell cell = owner.cell + direction;
    const Tile* tile = game.stage.at(cell);
    // PLACEMENT: No hidden trap under another fixture, actor or intact ground prop.
    if (tile == nullptr || !walkable(*tile) || surface_wet(*tile) || tile->kind == TileKind::Lava ||
        (tile->prop.kind != PropKind::None && !tile->prop.broken) || entity_at(game, cell) >= 0) return false;
    Entity* trap = get_entity(game, spawn_entity(game, EntityKind::Trap, cell));
    if (trap == nullptr) return false;
    trap->ground_item = *owner.inventory.held();
    trap->ground_item.count = 1;
    trap->ground_item.cooldown = 0;
    trap->sprite = item_sprite(trap->ground_item);
    trap->facing = direction;
    trap->owner = owner.owner;
    trap->health = trap->max_health = trap->ground_item.kind == ItemKind::SpringTrap ? 20 : 8;
    trap->timer_a = 20;
    return true;
}
