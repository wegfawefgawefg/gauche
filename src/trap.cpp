#include "game.hpp"

#include <algorithm>

void step_traps(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& trap = game.entities[static_cast<std::size_t>(slot)];
        if (trap.kind != EntityKind::Trap || trap.birth_tick == game.tick) continue;
        const int victim = entity_at(game, trap.cell, true);
        if (victim < 0) continue;
        const Cell cell = trap.cell;
        const ItemKind kind = trap.ground_item.kind;
        const std::uint32_t generation = trap.generation;
        remove_entity(game, {slot, generation});
        if (kind == ItemKind::Mine) {
            blast_area(game, cell, 2, 55, cell);
        } else if (kind == ItemKind::BearTrap) {
            Entity& caught = game.entities[static_cast<std::size_t>(victim)];
            const int before = caught.health;
            damage_entity(game, victim, 18, cell);
            if (caught.health > 0 && caught.health < before)
                caught.stun_ticks = std::max(caught.stun_ticks, 90);
            emit_sound(game, SoundId::HitBlock1, cell);
        }
    }
}
