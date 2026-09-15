#include "blink.hpp"
#include "../items/action.hpp"
#include "../items/displacement.hpp"
#include "../entities/pocket_door.hpp"

#include <algorithm>

namespace {

bool legal_landing(const Game& game, const Entity& seed, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    const int occupant = entity_at(game, cell, true);
    return tile != nullptr && walkable(*tile) &&
        (occupant < 0 || occupant == seed.entity_a.slot);
}

void land_seed(Game& game, int slot) {
    const Entity seed = game.entities[static_cast<std::size_t>(slot)];
    Entity* user = get_entity(game, seed.entity_a);
    // IMPACT: Recheck the near-side flight path; another actor or new wall may
    // have claimed the landing while the seed traveled. Never push that actor.
    Cell landing = seed.cell;
    bool clear = legal_landing(game, seed, landing);
    for (int back = 0; !clear && back < seed.attack_interval && landing != seed.point_a; ++back) {
        landing = landing - seed.facing;
        clear = legal_landing(game, seed, landing);
    }
    const bool can_blink = user != nullptr && user->health > 0 && movable_by_tool(*user) &&
        user->vitals.rooted == 0 && clear && user->cell != landing &&
        pocket_passage_allowed(game, user->cell, landing);
    remove_entity(game, {slot, seed.generation});
    if (!can_blink) { emit_sound(game, SoundId::SwapFade, seed.cell); return; }
    const Cell origin = user->cell;
    user->cell = landing;
    user->vitals.slide_momentum = 0;
    user->move_wait = std::max(user->move_wait, user->move_interval);
    if (user->kind == EntityKind::Player) cancel_item_action(*user);
    emit_sound(game, SoundId::SwapFold, origin);
    emit_sound(game, SoundId::SwapFold, landing);
    // Ground contact is real: water, fire, traps, springs and linked doors apply.
    enter_actor_cell(game, seed.entity_a.slot);
}

} // namespace

void step_blink_seed(Game& game, int slot) {
    Entity& seed = game.entities[static_cast<std::size_t>(slot)];
    const Entity* owner = get_entity(game, seed.entity_a);
    if (owner == nullptr || owner->health <= 0 || seed.timer_a == 0 || seed.counter_a == 0) {
        land_seed(game, slot); return;
    }
    if (seed.timer_b > 0) return;
    const Cell next = seed.cell + seed.facing;
    const int occupant = entity_at(game, next, true);
    if (projectile_blocked(game, next) || (occupant >= 0 && occupant != seed.entity_a.slot)) {
        land_seed(game, slot); return;
    }
    seed.cell = next;
    --seed.counter_a;
    seed.timer_b = 6;
    if (seed.counter_a == 0) land_seed(game, slot);
}
