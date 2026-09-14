#include "materials.hpp"
#include "cold_flask.hpp"
#include "../surfaces/interaction.hpp"
#include "../projectiles/projectile.hpp"
#include "../world/ground_items.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>
#include <cstdlib>

void material_impact(Game& game, const Item& item, Cell center) {
    if (item.kind == ItemKind::ColdFlask) { cold_flask_impact(game, item, center); return; }
    const ItemPattern pattern = item_pattern(item);
    emit_sound(game, item.kind == ItemKind::MushroomSpores ? SoundId::ToadPuff : SoundId::BottleBreak, center);
    for (int dy = -pattern.blast_radius; dy <= pattern.blast_radius; ++dy)
        for (int dx = -pattern.blast_radius; dx <= pattern.blast_radius; ++dx) {
            if (std::abs(dx) + std::abs(dy) > pattern.blast_radius) continue;
            const Cell cell = center + Cell{dx, dy};
            Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !walkable(tile->kind) ||
                !clear_sight(game, center, cell, false)) continue;
            if (item.kind == ItemKind::OilFlask) pour_surface(game, cell, LiquidKind::Oil, 900);
            else if (item.kind == ItemKind::SapJar) pour_surface(game, cell, LiquidKind::Sap, 1200);
            else if (item.kind == ItemKind::SmokePot)
                tile->surface.smoke_ticks = std::max<std::uint16_t>(tile->surface.smoke_ticks, 300);
            else if (item.kind == ItemKind::MushroomSpores)
                tile->surface.sleep_ticks = std::max<std::uint16_t>(tile->surface.sleep_ticks, 180);
        }
}

bool use_material_item(Game& game, int slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Item item = *user.inventory.held();
    const ItemPattern pattern = item_pattern(item);
    if (item.kind == ItemKind::OilFlask || item.kind == ItemKind::SapJar ||
        item.kind == ItemKind::SmokePot || item.kind == ItemKind::MushroomSpores)
        return launch_projectile(game, slot, item, direction, pattern.maximum);
    if (item.kind == ItemKind::HoneyPot) {
        const Cell cell = user.cell + direction;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, false) >= 0) return false;
        Entity* bait = get_entity(game, spawn_entity(game, EntityKind::GroundItem, cell));
        if (bait == nullptr) return false;
        bait->ground_item = item;
        bait->ground_item.count = 1;
        bait->sprite = item_sprite(item);
        pour_surface(game, cell, LiquidKind::Honey, 900);
        return true;
    }
    const Cell side{-direction.y, direction.x};
    bool used = false;
    if (item.kind == ItemKind::Torch) { strike_melee(game, slot, direction, item); used = true; }
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
        for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
            const Cell cell = user.cell + Cell{direction.x * reach + side.x * lane,
                                               direction.y * reach + side.y * lane};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr) break;
            if (item.kind == ItemKind::WaterFlask) used |= pour_surface(game, cell, LiquidKind::Water, 360);
            else { ignite_surface(game, cell); used = true; }
            if (!walkable(*tile)) break;
        }
    return used;
}
