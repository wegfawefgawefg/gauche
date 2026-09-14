#include "root_drill.hpp"
#include "../props/interaction.hpp"

namespace {

void stop_drill(Game& game, int slot) {
    const Entity& drill = game.entities[static_cast<std::size_t>(slot)];
    emit_sound(game, SoundId::DrillStop, drill.cell);
    remove_entity(game, {slot, drill.generation});
}

} // namespace

bool root_drill_blocked(const Game& game, Cell cell, const Item& item) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr) return true;
    if (tile->kind == TileKind::Wall && (tile->break_rule == BreakRule::Unbreakable || tile->hp == 0 ||
        (tile->break_rule == BreakRule::DigRequired && tile->required_dig_power > item.dig_power))) return true;
    const int slot = entity_at(game, cell, true);
    if (slot < 0) return false;
    const EntityKind kind = game.entities[static_cast<std::size_t>(slot)].kind;
    return kind == EntityKind::Door || kind == EntityKind::EncounterGate || kind == EntityKind::Crusher;
}

// BORING: Six ticks per bite or clear-cell advance, with an independent six-second life.
void step_root_drill(Game& game, int slot) {
    Entity& drill = game.entities[static_cast<std::size_t>(slot)];
    if (drill.timer_a == 0 || drill.counter_a == 0) { stop_drill(game, slot); return; }
    if (drill.timer_b > 0) return;
    const Cell next = drill.cell + drill.facing;
    const Tile* tile = game.stage.at(next);
    if (root_drill_blocked(game, next, drill.ground_item)) { stop_drill(game, slot); return; }
    drill.timer_b = 6;
    drill.label_b = 0;
    if (tile->kind == TileKind::Wall) {
        hit_terrain(game, next, drill.cell, drill.counter_b * 2, drill.ground_item.dig_power);
        emit_sound(game, SoundId::DrillBite, next);
        if (tile->kind == TileKind::Wall) { drill.label_b = 1; return; }
    }
    hit_prop(game, next, drill.counter_b, drill.cell);
    if (prop_blocks(tile->prop)) { drill.label_b = 1; return; }
    const Cell source = drill.cell;
    drill.cell = next;
    --drill.counter_a;
    for (int index = 0; index < max_entities; ++index) {
        const Entity& actor = game.entities[static_cast<std::size_t>(index)];
        if (index == slot || actor.health <= 0 || !actor.impassable || actor.cell != next) continue;
        if (index == drill.entity_a.slot && actor.generation == drill.entity_a.generation) continue;
        // The burrowing tip pierces armor; this does not depend on what its owner now holds.
        damage_entity(game, index, drill.counter_b, source, false);
        emit_sound(game, SoundId::DrillBite, next);
    }
    if (drill.counter_a == 0) stop_drill(game, slot);
}
