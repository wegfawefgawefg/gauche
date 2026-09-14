#include "game.hpp"

#include <algorithm>

namespace {

bool player_near(const Game& game, Cell cell) {
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        if (!game.run.online[owner]) continue;
        const Entity* player = get_entity(game, game.players[owner]);
        if (player != nullptr && player->health > 0 && distance(player->cell, cell) <= 6)
            return true;
    }
    return false;
}

void step_crusher(Game& game, int slot) {
    Entity& crusher = game.entities[static_cast<std::size_t>(slot)];
    if (crusher.script_tick == 0) {
        if (!player_near(game, crusher.cell)) return;
        crusher.script_tick = 1;
        crusher.move_wait = 24;
        emit_sound(game, SoundId::SturdyBlockBouncedOn, crusher.cell);
        return;
    }
    if (crusher.move_wait > 0) return;
    const Cell next = crusher.cell + crusher.facing;
    const Tile* tile = game.stage.at(next);
    const int victim = entity_at(game, next, true);
    if (tile == nullptr || !walkable(*tile) ||
        (victim >= 0 && game.entities[static_cast<std::size_t>(victim)].hard_blocker)) {
        crusher.facing = {-crusher.facing.x, -crusher.facing.y};
        crusher.move_wait = crusher.move_interval;
        emit_sound(game, SoundId::SturdyBlockBouncedOn, crusher.cell);
        return;
    }
    const Cell pushed = next + crusher.facing;
    const Tile* behind = game.stage.at(pushed);
    const bool blocked = behind == nullptr || !walkable(*behind) ||
                         entity_at(game, pushed, true) >= 0;
    if (victim >= 0) {
        if (blocked) crush_entity(game, victim, crusher.cell);
        else {
            Entity& actor = game.entities[static_cast<std::size_t>(victim)];
            actor.cell = pushed;
            enter_actor_cell(game, victim);
            actor.move_wait = std::max(actor.move_wait, 6);
        }
    }
    for (int item_slot = 0; item_slot < max_entities; ++item_slot) {
        Entity& item = game.entities[static_cast<std::size_t>(item_slot)];
        if (item.kind != EntityKind::GroundItem || item.cell != next) continue;
        if (blocked) remove_entity(game, {item_slot, item.generation});
        else item.cell = pushed;
    }
    crusher.cell = next;
    crusher.move_wait = crusher.move_interval;
    emit_sound(game, SoundId::BlockLand, next);
}

} // namespace

void step_traps(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& trap = game.entities[static_cast<std::size_t>(slot)];
        if (trap.kind == EntityKind::Crusher && trap.birth_tick != game.tick) {
            step_crusher(game, slot);
            continue;
        }
        if (trap.kind != EntityKind::Trap || trap.birth_tick == game.tick) continue;
        const int victim = entity_at(game, trap.cell, true);
        if (victim < 0) continue;
        const Cell cell = trap.cell;
        const ItemKind kind = trap.ground_item.kind;
        if (kind == ItemKind::Mine) {
            remove_entity(game, {slot, trap.generation});
            blast_area(game, cell, 2, 55, cell);
        } else if (kind == ItemKind::BearTrap) {
            // What if the trap is sprung? Leave its closed jaws to recover.
            trap.kind = EntityKind::GroundItem;
            trap.ground_item.opened = false;
            trap.sprite = Sprite::BearTrap;
            Entity& caught = game.entities[static_cast<std::size_t>(victim)];
            const int before = caught.health;
            damage_entity(game, victim, 100, cell);
            if (caught.health > 0 && caught.health < before)
                caught.stun_ticks = std::max(caught.stun_ticks, 90);
            emit_sound(game, SoundId::HitBlock1, cell);
        }
    }
}
