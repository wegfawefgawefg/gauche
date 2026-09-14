#include "swap.hpp"
#include "../items/displacement.hpp"
#include "../items/action.hpp"

#include <algorithm>

namespace {

bool legal_landing(const Game& game, Cell cell, int first, int second) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || !walkable(*tile)) return false;
    for (int slot = 0; slot < max_entities; ++slot) {
        if (slot == first || slot == second) continue;
        const Entity& other = game.entities[static_cast<std::size_t>(slot)];
        if (other.kind != EntityKind::None && other.impassable && other.cell == cell) return false;
    }
    return true;
}

bool swap_actors(Game& game, Handle source, Handle victim) {
    Entity* user = get_entity(game, source);
    Entity* target = get_entity(game, victim);
    if (source.slot == victim.slot || user == nullptr || target == nullptr ||
        user->health <= 0 || target->health <= 0 || !user->impassable || !target->impassable ||
        !movable_by_tool(*user) || !movable_by_tool(*target) ||
        !legal_landing(game, user->cell, source.slot, victim.slot) ||
        !legal_landing(game, target->cell, source.slot, victim.slot)) return false;
    // ATOMIC: Neither landing sees the other actor still occupying its destination.
    std::swap(user->cell, target->cell);
    user->move_wait = std::max(user->move_wait, user->move_interval);
    target->move_wait = std::max(target->move_wait, target->move_interval);
    if (user->kind == EntityKind::Player) cancel_item_action(*user);
    if (target->kind == EntityKind::Player) cancel_item_action(*target);
    emit_sound(game, SoundId::SwapFold, user->cell);
    emit_sound(game, SoundId::SwapFold, target->cell);
    enter_actor_cell(game, source.slot);
    enter_actor_cell(game, victim.slot);
    return true;
}

void fade_seed(Game& game, int slot) {
    const Entity& seed = game.entities[static_cast<std::size_t>(slot)];
    emit_sound(game, SoundId::SwapFade, seed.cell);
    remove_entity(game, {slot, seed.generation});
}

} // namespace

void step_swap_seed(Game& game, int slot) {
    Entity& seed = game.entities[static_cast<std::size_t>(slot)];
    const Entity* owner = get_entity(game, seed.entity_a);
    if (seed.timer_a == 0 || seed.counter_a == 0 || owner == nullptr || owner->health <= 0) {
        fade_seed(game, slot); return;
    }
    if (seed.timer_b > 0) return;
    const Cell next = seed.cell + seed.facing;
    if (projectile_blocked(game, next)) { fade_seed(game, slot); return; }
    seed.cell = next;
    --seed.counter_a;
    seed.timer_b = 4;
    const int actor = entity_at(game, next, true);
    if (actor >= 0 && actor != seed.entity_a.slot) {
        const Handle target{actor, game.entities[static_cast<std::size_t>(actor)].generation};
        if (swap_actors(game, seed.entity_a, target)) remove_entity(game, {slot, seed.generation});
        else fade_seed(game, slot);
        return;
    }
    if (seed.counter_a == 0) fade_seed(game, slot);
}
