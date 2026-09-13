#include "dispatch.hpp"

#include <algorithm>
#include <cstdlib>
#include <optional>

namespace {

std::optional<Cell> free_entrance_cell(const Game& game) {
    for (int radius = 0; radius <= 8; ++radius) {
        for (int dy = -radius; dy <= radius; ++dy) {
            const int dx = radius - std::abs(dy);
            for (int side = -1; side <= 1; side += 2) {
                const Cell cell = game.run.spawn + Cell{dx * side, dy};
                const Tile* tile = game.stage.at(cell);
                if (tile != nullptr && walkable(tile->kind) &&
                    entity_at(game, cell, true) < 0) return cell;
            }
        }
    }
    return std::nullopt;
}

} // namespace

void step_entity_timers(Game& game, int slot) {
    Entity& entity = game.entities[static_cast<std::size_t>(slot)];
    if (entity.kind == EntityKind::None) return;
    if (entity.freeze_ticks == 0 || game.tick % 2 == 0)
        entity.move_wait = std::max(0, entity.move_wait - 1);
    entity.attack_wait = std::max(0, entity.attack_wait - 1);
    entity.block_ticks = std::max(0, entity.block_ticks - 1);
    entity.use_flash = std::max(0, entity.use_flash - 1);

    // HAZARDS: Damage resolves before this tick's action, even on a fatal hit.
    if (entity.burn_ticks > 0) {
        --entity.burn_ticks;
        if (game.tick % 30 == 0)
            damage_entity(game, slot, 4, entity.cell + Cell{0, 1});
    }
    entity.freeze_ticks = std::max(0, entity.freeze_ticks - 1);
    entity.sleep_ticks = std::max(0, entity.sleep_ticks - 1);
    entity.stun_ticks = std::max(0, entity.stun_ticks - 1);
    const Tile* ground = game.stage.at(entity.cell);
    if (ground != nullptr && ground->kind == TileKind::Lava &&
        entity.kind != EntityKind::Ember && game.tick % 30 == 0)
        damage_entity(game, slot, 5, entity.cell + Cell{0, 1});

    // RESPAWN: A blocked entrance delays return instead of overlapping a fixture.
    if (entity.kind == EntityKind::Player && entity.health == 0 &&
        game.run.death_policy == DeathPolicy::Entrance &&
        game.run.phase == RunPhase::Playing) {
        entity.spawn_wait = std::max(0, entity.spawn_wait - 1);
        if (entity.spawn_wait == 0) {
            if (const auto cell = free_entrance_cell(game)) {
                entity.cell = *cell;
                entity.health = entity.max_health;
                entity.impassable = entity.owner < 0 || entity.owner >= 4 ||
                    game.run.online[static_cast<std::size_t>(entity.owner)];
                entity.sprite = Sprite::Player;
            } else entity.spawn_wait = 1;
        }
    }
    for (Item& item : entity.inventory.slots)
        item.cooldown = std::max(0, item.cooldown - 1);
}
