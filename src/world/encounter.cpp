#include "encounter.hpp"
#include "../entities/gate.hpp"
#include "ground_items.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool inside(const Entity& controller, Cell cell) {
    return cell.x >= controller.point_a.x && cell.y >= controller.point_a.y &&
           cell.x <= controller.point_b.x && cell.y <= controller.point_b.y;
}

struct Party { int inside = 0, outside = 0; };
Party party_at(const Game& game, const Entity& controller) {
    Party party;
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        const Entity* player = get_entity(game, game.players[owner]);
        if (!game.run.online[owner] || player == nullptr || player->health <= 0) continue;
        if (inside(controller, player->cell)) ++party.inside;
        else ++party.outside;
    }
    return party;
}

bool gates_clear(const Game& game, Handle controller) {
    for (const Entity& gate : game.entities)
        if (gate.kind == EntityKind::EncounterGate && gate.entity_a == controller)
            if (entity_at(game, gate.cell, true) >= 0 && !gate.impassable) return false;
    return true;
}

void gates(Game& game, Handle controller, bool open) {
    for (Entity& gate : game.entities) {
        if (gate.kind != EntityKind::EncounterGate || gate.entity_a != controller) continue;
        request_gate(game,gate,open);
    }
}

int living_enemies(const Game& game, Handle controller) {
    int count = 0;
    for (const Entity& enemy : game.entities)
        if (enemy.kind != EntityKind::None && enemy.encounter == controller && enemy.health > 0)
            ++count;
    return count;
}

void release(Game& game, int slot) {
    Entity& controller = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot, controller.generation};
    controller.label_a = static_cast<int>(EncounterPhase::Released);
    controller.fixture_open = true;
    controller.light = {4, 850, {120, 220, 145}};
    gates(game, handle, true);
    for (Entity& exit : game.entities)
        if (exit.kind == EntityKind::Exit && exit.entity_a == handle) {
            game.run.has_key = true;
            exit.light.color = {84, 255, 135};
        }
    place_ground_item(game, controller.cell, ItemKind::Medkit);
    emit_sound(game, SoundId::EncounterClear, controller.cell);
}

} // namespace

// SLOTS: point_a/b bound the room; label_a = phase; label_b = waves started.
// counter_a = total waves; counter_b = queued bodies; timer_a = phase/spawn beat.
// entity_a = vent currently announcing a spawn. Enemy membership has its own handle.
void init_encounter(Entity& entity) {
    entity.health = entity.max_health = 1;
    entity.sprite = Sprite::Switch;
    entity.light = {3, 450, {167, 139, 224}};
    entity.counter_a = 3;
}

bool request_encounter(Game& game, Entity& controller) {
    if (controller.label_a != static_cast<int>(EncounterPhase::Dormant)) return false;
    controller.label_a = static_cast<int>(EncounterPhase::Gathering);
    emit_sound(game, SoundId::EncounterBell, controller.cell);
    return true;
}

bool encounter_released(const Game& game, Handle handle) {
    if (handle.slot < 0) return true;
    const Entity* controller = get_entity(game, handle);
    return controller != nullptr && controller->kind == EntityKind::Encounter &&
           controller->label_a == static_cast<int>(EncounterPhase::Released);
}

void step_encounter(Game& game, int slot) {
    Entity& controller = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot, controller.generation};
    const auto phase = static_cast<EncounterPhase>(controller.label_a);
    if (phase == EncounterPhase::Dormant || phase == EncounterPhase::Released) return;
    const Party party = party_at(game, controller);
    if (phase == EncounterPhase::Gathering) {
        if (party.inside == 0 || party.outside > 0 || !gates_clear(game, handle)) return;
        controller.label_a = static_cast<int>(EncounterPhase::Countdown);
        controller.timer_a = 90;
        emit_sound(game, SoundId::EncounterBell, controller.cell);
        return;
    }
    if (phase == EncounterPhase::Countdown) {
        if (party.inside == 0 || party.outside > 0 || !gates_clear(game, handle)) {
            controller.label_a = static_cast<int>(EncounterPhase::Gathering);
            return;
        }
        if (controller.timer_a > 0) return;
        gates(game, handle, false);
        controller.label_a = static_cast<int>(EncounterPhase::Fighting);
        if (controller.counter_b == 0 && living_enemies(game, handle) == 0)
            spawn_encounter_wave(game, slot);
        return;
    }
    // EMPTY ROOM: Death or disconnect cannot leave returning players behind sealed gates.
    if (party.inside == 0) {
        gates(game, handle, true);
        if (Entity* vent = get_entity(game, controller.entity_a)) {
            vent->light.strength = 120;
            vent->timer_a = 0;
        }
        controller.entity_a = {};
        controller.label_a = static_cast<int>(EncounterPhase::Gathering);
        return;
    }
    if (phase == EncounterPhase::Intermission) {
        if (controller.timer_a == 0) spawn_encounter_wave(game, slot);
        return;
    }
    step_encounter_spawn(game, slot);
    if (controller.counter_b > 0 || living_enemies(game, handle) > 0) return;
    if (controller.label_b >= controller.counter_a) { release(game, slot); return; }
    controller.label_a = static_cast<int>(EncounterPhase::Intermission);
    controller.timer_a = 180;
    place_ground_item(game, controller.cell, ItemKind::Ammo);
    place_ground_item(game, controller.cell, ItemKind::Bandage, 2);
    emit_sound(game, SoundId::EncounterBell, controller.cell);
}

std::optional<Cell> encounter_join_cell(const Game& game) {
    for (const Entity& controller : game.entities) {
        if (controller.kind != EntityKind::Encounter ||
            controller.label_a < static_cast<int>(EncounterPhase::Countdown) ||
            controller.label_a >= static_cast<int>(EncounterPhase::Released)) continue;
        const Cell center = controller.cell;
        for (int radius = 1; radius <= 40; ++radius)
            for (int y = -radius; y <= radius; ++y) {
                const int dx = radius - std::abs(y);
                for (int side : {-1, 1}) {
                    const Cell cell = center + Cell{dx * side, y};
                    const Tile* tile = game.stage.at(cell);
                    if (inside(controller, cell) && tile != nullptr && walkable(*tile) &&
                        tile->kind != TileKind::Lava && entity_at(game, cell, true) < 0) return cell;
                }
            }
    }
    return std::nullopt;
}
