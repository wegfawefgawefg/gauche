#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"

#include <array>

// SLOTS: counter_a = living bodies including the base; label_a = toppling.
// timer_a spaces falling bodies. A blocked fall stays counted until space opens.
void init_zombie_stack(Game& game, Entity& entity) {
    init_zombie(game, entity);
    entity.counter_a = 3 + static_cast<int>(random_u32(game) % 3);
    entity.move_interval = 65;
    entity.attack_interval = 85;
}

bool topple_zombie_stack(Game& game, int slot) {
    Entity& stack = game.entities[static_cast<std::size_t>(slot)];
    if (stack.kind != EntityKind::ZombieStack || stack.counter_a <= 1) return false;
    --stack.counter_a;
    stack.health = stack.max_health;
    stack.label_a = 1;
    stack.timer_a = 12;
    stack.attack_wait = 60;
    emit_sound(game, SoundId::ZombieTopple, stack.cell);
    return true;
}

namespace {

void release_survivor(Game& game, Entity& stack) {
    constexpr std::array<Cell, 4> sides{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    for (int index = 0; index < 4; ++index) {
        const Cell direction = sides[static_cast<std::size_t>((index + stack.counter_a) % 4)];
        const Cell cell = stack.cell + direction;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0) continue;
        const Handle handle = spawn_entity(game, EntityKind::Zombie, cell);
        Entity* survivor = get_entity(game, handle);
        if (survivor == nullptr) return; // Full actor pool: keep the survivor in the stack.
        survivor->encounter = stack.encounter;
        survivor->burn_ticks = stack.burn_ticks;
        survivor->scorch_ticks = stack.scorch_ticks;
        survivor->freeze_ticks = stack.freeze_ticks;
        survivor->point_a = stack.cell;
        survivor->label_b = 1;
        survivor->timer_b = survivor->stun_ticks = 24;
        survivor->facing = direction;
        --stack.counter_a;
        return;
    }
}

} // namespace

void step_zombie_stack(Game& game, int slot) {
    Entity& stack = game.entities[static_cast<std::size_t>(slot)];
    if (stack.label_a == 0) {
        if (step_hearing(game, slot)) return;
        const auto target = enemy_target(game, stack.cell, stack.encounter.slot >= 0 ? 60 : 8);
        if (target.has_value()) {
            const Cell destination = target->cell;
            if (distance(stack.cell, destination) > 1) {
                if (stack.encounter.slot >= 0) pursue(game, slot, destination);
                else approach(game, slot, destination);
            }
        }
        else wander(game, slot);
        bite(game, slot, 8);
        maybe_growl(game, slot, SoundId::ZombieGrowl2);
        return;
    }

    // TOPPLE: One body falls at a time; crowded stacks shuffle until a neighbor is free.
    if (stack.timer_a > 0) return;
    if (stack.counter_a > 1) release_survivor(game, stack);
    stack.timer_a = 8;
    if (stack.counter_a > 1) { wander(game, slot); return; }
    stack.kind = EntityKind::Zombie;
    stack.counter_a = stack.label_a = stack.timer_a = 0;
    stack.move_interval = 48;
    stack.attack_interval = 60;
    stack.label_b = 1;
    stack.timer_b = stack.stun_ticks = 24;
    stack.point_a = stack.cell;
}
