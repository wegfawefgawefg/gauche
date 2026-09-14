#include "interaction.hpp"
#include "../world/ground_items.hpp"
#include "../world/loot.hpp"

#include <algorithm>

namespace {

void drop_contents(Game& game, Cell cell, PropKind kind) {
    // LOOT: Empty clutter is normal; nests and containers have contextual finds.
    ItemKind item = ItemKind::None;
    const std::uint32_t roll = random_u32(game) % 100;
    switch (kind) {
    case PropKind::Nest: if (roll < 18) item = ItemKind::Egg; break;
    case PropKind::Crate:
        if (roll < 35) item = ItemKind::Ammo;
        else if (roll < 60) item = ItemKind::Bandage;
        else if (roll < 70) item = ItemKind::SleepMeds;
        else if (roll < 90) place_coins(game, cell, 4 + static_cast<int>(random_u32(game) % 7));
        break;
    case PropKind::ClayPot:
        if (roll < 15) item = ItemKind::Bandaid;
        else if (roll < 45) place_coins(game, cell, 2 + static_cast<int>(random_u32(game) % 5));
        break;
    default: break;
    }
    if (item == ItemKind::None) return;
    const Cell destination = nearby_ground_item_cell(game, cell);
    Entity* dropped = get_entity(game, spawn_entity(game, EntityKind::GroundItem, destination));
    if (dropped == nullptr) return;
    dropped->ground_item = make_item(item);
    dropped->sprite = item_sprite(item);
}

void break_prop(Game& game, Cell cell, Cell source, Prop& prop) {
    prop.hp = 0;
    prop.broken = true;
    prop.growth_ticks = 0;
    const PropSpec spec = prop_spec(prop.kind);
    emit_sound(game, spec.sound, cell);
    if (game.impact_count < static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)] =
            {cell, source, spec.sprite, spec.health, true, prop.kind};
    if (prop.kind == PropKind::Puffball) {
        // SPORES: Breaking a mushroom beside an enemy can buy a short escape.
        for (Entity& actor : game.entities)
            if (actor.health > 0 && actor.move_interval > 0 && !actor.hard_blocker &&
                distance(actor.cell, cell) <= 1)
                actor.sleep_ticks = std::max(actor.sleep_ticks, 75);
    }
    if (prop.kind == PropKind::Nest || prop.kind == PropKind::Crate ||
        prop.kind == PropKind::ClayPot) drop_contents(game, cell, prop.kind);
}

} // namespace

bool place_prop(Stage& stage, Cell cell, PropKind kind, std::uint8_t variant) {
    Tile* tile = stage.at(cell);
    if (tile == nullptr || !walkable(tile->kind) || tile->prop.kind != PropKind::None)
        return false;
    tile->prop = {kind, static_cast<std::uint8_t>(prop_spec(kind).health), variant, false};
    return true;
}

bool hit_prop(Game& game, Cell cell, int damage, Cell source) {
    Tile* tile = game.stage.at(cell);
    if (tile == nullptr || damage <= 0 || tile->prop.kind == PropKind::None ||
        tile->prop.broken) return false;
    Prop& prop = tile->prop;
    prop.hp = static_cast<std::uint8_t>(std::max(0, static_cast<int>(prop.hp) - damage));
    if (prop.hp == 0) break_prop(game, cell, source, prop);
    else emit_sound(game, SoundId::WoodCrack, cell);
    return true;
}

void step_on_prop(Game& game, int actor_slot) {
    const Entity& actor = game.entities[static_cast<std::size_t>(actor_slot)];
    Tile* tile = game.stage.at(actor.cell);
    if (tile != nullptr && !tile->prop.broken &&
        prop_spec(tile->prop.kind).breaks_on_step)
        break_prop(game, actor.cell, actor.cell, tile->prop);
}
