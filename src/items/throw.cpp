#include "catalog.hpp"
#include "../props/interaction.hpp"
#include "../world/ground_items.hpp"

bool throw_rock(Game& game, int user_slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(user_slot)];
    const Item item = *user.inventory.held();
    // CAPACITY: Reserve the recoverable object before dealing any damage.
    const Handle rock_handle = spawn_entity(game, EntityKind::GroundItem, user.cell);
    Entity* rock = get_entity(game, rock_handle);
    if (rock == nullptr) return false;
    rock->ground_item = item;
    rock->ground_item.count = 1;
    rock->ground_item.cooldown = item_pattern(item).cooldown;
    rock->sprite = item_sprite(item);
    const ItemPattern pattern = item_pattern(item);
    Cell landing = user.cell;
    for (int reach = 1; reach <= pattern.maximum; ++reach) {
        const Cell cell = user.cell + Cell{direction.x * reach, direction.y * reach};
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr) break;
        const bool blocked = !walkable(*tile);
        hit_prop(game, cell, pattern.damage, user.cell);
        if (blocked) {
            hit_terrain(game, cell, user.cell, pattern.damage, item.dig_power);
            break;
        }
        landing = cell;
        const int target = entity_at(game, cell, true);
        if (target >= 0 && target != user_slot) {
            damage_entity(game, target, pattern.damage, user.cell);
            if (!pattern.piercing && !has_artifact(user, ArtifactKind::AllPiercing)) break;
        }
    }
    rock->cell = nearby_ground_item_cell(game, landing);
    emit_sound(game, SoundId::RockImpact, landing);
    return true;
}
