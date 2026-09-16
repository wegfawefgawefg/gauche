#include "ground_items.hpp"

Handle place_ground_item(Game& game, Cell cell, ItemKind kind, int count) {
    const Handle handle = spawn_entity(game, EntityKind::GroundItem,
                                       nearby_ground_item_cell(game, cell));
    if (Entity* entity = get_entity(game, handle)) {
        entity->ground_item = make_item(kind, count);
        entity->sprite = item_sprite(kind);
    }
    return handle;
}
