#include "doorstop.hpp"
#include "interaction.hpp"
#include "../item_attribute.hpp"

bool doorstop_present(const Prop& prop) {
    return prop.kind == PropKind::Doorstop && prop.hp > 0 && !prop.broken;
}

bool place_doorstop(Game& game, Cell cell, const Item& item) {
    Tile* tile = game.stage.at(cell);
    if (item.kind != ItemKind::EmergencyDoorstop || item.durability <= 0 || !tile ||
        !walkable(*tile) || (tile->prop.kind != PropKind::None && !tile->prop.broken)) return false;
    bool gate_open = false;
    for (const Entity& gate : game.entities)
        if (gate.kind == EntityKind::EncounterGate && gate.cell == cell && gate.fixture_open)
            gate_open = true;
    // LOCKS: Ordinary key doors and already-closed gates cannot accept a wedge.
    if (!gate_open) return false;
    tile->prop = {PropKind::Doorstop,static_cast<std::uint8_t>(item.durability),
        static_cast<std::uint8_t>(item.attribute == ItemAttribute::Durable),false};
    return true;
}

Item recoverable_doorstop(const Prop& prop) {
    if (!doorstop_present(prop)) return {};
    Item item = make_item(ItemKind::EmergencyDoorstop,1,prop.variant == 1 ? ItemAttribute::Durable : ItemAttribute::None);
    item.durability = prop.hp;
    return item;
}

int release_doorstop(Game& game, Cell cell, Cell destination) {
    Tile* tile = game.stage.at(cell);
    if (!tile) return -1;
    const Item item = recoverable_doorstop(tile->prop);
    if (item.kind == ItemKind::None) return -1;
    // RECOVERY: Allocate first. A full entity pool must not eat the real placed item.
    const Handle handle = spawn_entity(game,EntityKind::GroundItem,destination);
    Entity* loose = get_entity(game,handle);
    if (!loose) return -1;
    loose->ground_item = item; loose->sprite = Sprite::EmergencyDoorstop;
    tile->prop = {};
    emit_sound(game,SoundId::WedgeLift,cell);
    return handle.slot;
}
