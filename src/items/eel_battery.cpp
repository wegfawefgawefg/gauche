#include "eel_battery.hpp"
#include "../surfaces/conduction.hpp"

namespace {

constexpr RegionalItem battery{"Eel Battery", "Touch to shock. Wet paths carry weaker arcs. Friends and you can be hit. Eels are immune.",
    Sprite::EelBattery, {1, 1, 4, 14, 45, PatternEffect::Damage, false, 0, 0, false, false, false, false, true},
    ItemAction::Material, 24, 1, false, 3, 0, 0, 0, 0, SoundId::BatteryZap};

} // namespace

const RegionalItem* eel_battery_item(ItemKind kind) {
    return kind == ItemKind::EelBattery ? &battery : nullptr;
}

bool use_eel_battery(Game& game, int slot, Cell direction) {
    const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const ItemPattern pattern = item_pattern(*actor.inventory.held());
    const Cell contact = actor.cell + direction;
    if (wet_wave(game, contact, 0).count == 0) return false;
    discharge_water(game, contact, actor.cell, pattern.damage, pattern.blast_radius);
    return true;
}
