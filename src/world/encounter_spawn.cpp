#include "encounter.hpp"

#include <array>
#include <optional>

namespace {

std::optional<Cell> vent_space(const Game& game, const Entity& vent) {
    constexpr Cell offsets[]{{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (Cell offset : offsets) {
        const Cell cell = vent.cell + offset;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0) continue;
        bool close = false;
        for (const auto& [owner, participant] : game.players) {
            const Entity* player = get_entity(game, player_state(game, owner).controlled);
            if (player_state(game, owner).online && player != nullptr && player->health > 0 &&
                distance(cell, player->cell) < 3) close = true;
        }
        if (!close) return cell;
    }
    return std::nullopt;
}

} // namespace

void spawn_encounter_wave(Game& game, int slot) {
    Entity& controller = game.entities[static_cast<std::size_t>(slot)];
    if (controller.label_b >= controller.counter_a) return;
    ++controller.label_b;
    int players = 0;
    for (const auto& [owner, participant] : game.players)
        if (player_state(game, owner).online)
            if (const Entity* player = get_entity(game, player_state(game, owner).controlled))
                if (player->health > 0) ++players;
    controller.counter_b = 3 + controller.label_b * 2 + players * 2;
    controller.label_a = static_cast<int>(EncounterPhase::Fighting);
    controller.timer_a = 30;
    controller.entity_a = {};
    emit_sound(game, SoundId::EncounterBell, controller.cell);
}

void step_encounter_spawn(Game& game, int slot) {
    Entity& controller = game.entities[static_cast<std::size_t>(slot)];
    if (controller.counter_b <= 0 || controller.timer_a > 0) return;
    const Handle handle{slot, controller.generation};
    if (Entity* vent = get_entity(game, controller.entity_a)) {
        if (const auto cell = vent_space(game, *vent)) {
            const EntityKind kind = controller.label_b >= 2 && controller.counter_b % 5 == 0 ?
                                    EntityKind::ZombieStack : EntityKind::Zombie;
            Entity* enemy = get_entity(game, spawn_entity(game, kind, *cell));
            if (enemy != nullptr) {
                enemy->encounter = handle;
                enemy->move_interval = kind == EntityKind::Zombie ? 25 : 45;
                enemy->move_wait = 24;
                --controller.counter_b;
                emit_sound(game, SoundId::GraveRise, vent->cell);
            }
        }
        vent->light.strength = 120;
        vent->timer_a = 0;
        controller.entity_a = {};
        controller.timer_a = 30;
        return;
    }
    std::array<Handle, 16> choices{};
    int count = 0;
    for (int candidate = 0; candidate < max_entities && count < 16; ++candidate) {
        const Entity& vent = game.entities[static_cast<std::size_t>(candidate)];
        if (vent.kind == EntityKind::WaveVent && vent.entity_a == handle && vent_space(game, vent))
            choices[static_cast<std::size_t>(count++)] = {candidate, vent.generation};
    }
    if (count == 0) { controller.timer_a = 15; return; }
    controller.entity_a = choices[random_u32(game) % static_cast<std::uint32_t>(count)];
    Entity& vent = *get_entity(game, controller.entity_a);
    vent.timer_a = controller.timer_a = 36;
    vent.light.strength = 1000;
    emit_sound(game, SoundId::GraveRise, vent.cell);
}
