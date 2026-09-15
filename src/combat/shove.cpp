#include "../entities/strikebreaker.hpp"
#include "shove.hpp"

bool shove_in_front(Game& game, int user_slot, Cell direction) {
    const Cell front = game.entities[static_cast<std::size_t>(user_slot)].cell + direction;
    int target_slot = entity_at(game, front, true);
    if (target_slot < 0) {
        for (int slot = 0; slot < max_entities; ++slot) {
            const Entity& candidate = game.entities[static_cast<std::size_t>(slot)];
            if (candidate.kind == EntityKind::GroundItem && candidate.cell == front) {
                target_slot = slot;
                break;
            }
        }
    }
    if (target_slot < 0 || target_slot == user_slot) return false;
    return shove_actor(game, target_slot, direction,
                       game.entities[static_cast<std::size_t>(user_slot)].cell);
}

bool shove_actor(Game& game, int target_slot, Cell direction, Cell source) {
    if (target_slot < 0 || target_slot >= max_entities || distance({}, direction) != 1) return false;
    Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    if (target.hard_blocker || target.vitals.grip > 0) return false;
    const Cell destination = target.cell + direction;
    const Tile* tile = game.stage.at(destination);
    const int blocker_slot = entity_at(game, destination, true);
    const bool hard_tile = tile == nullptr || !walkable(*tile);
    const bool hard_actor = blocker_slot >= 0 &&
        game.entities[static_cast<std::size_t>(blocker_slot)].hard_blocker;
    if (hard_tile || hard_actor) {
        if (target.kind == EntityKind::GroundItem)
            remove_entity(game, {target_slot, target.generation});
        else crush_entity(game, target_slot, source);
        return true;
    }
    // Ordinary teammates and loose items do not turn a shove into a crush.
    if (blocker_slot >= 0) return false;
    if (target.kind == EntityKind::GroundItem) {
        for (const Entity& other : game.entities)
            if (&other != &target && other.kind == EntityKind::GroundItem &&
                other.cell == destination) return false;
    }
    target.cell = destination;
    enter_actor_cell(game, target_slot);
    target.move_wait = target.move_interval;
    return true;
}

bool blocks_facing(const Entity& actor, Cell attacker) {
    if (breaker_blocks(actor,attacker)) return true;
    const Item& held = *actor.inventory.held();
    const bool shield = held.kind == ItemKind::Buckler || held.kind == ItemKind::ShieldLantern;
    return actor.health > 0 && actor.block_ticks > 0 &&
        actor.guard_slot == actor.inventory.selected && shield && held.count > 0 && held.durability > 0 &&
        actor.facing == cardinal_toward(actor.cell, attacker, actor.facing);
}
