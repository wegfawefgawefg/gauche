#include "dispatch.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "bird_feeding.hpp"
#include "../props/interaction.hpp"

namespace {

void stop_drilling(Game& game, Entity& bird, bool stuck) {
    bird.label_a = 3; bird.timer_a = stuck ? 90 : 45;
    bird.move_interval = 18;
    bird.sprite = Sprite::Woodpecker;
    if (stuck) emit_sound(game, SoundId::BeakStuck, bird.cell);
}

void drill(Game& game, int slot) {
    Entity& bird = game.entities[static_cast<std::size_t>(slot)];
    if (bird.move_wait > 0) return;
    if (bird.counter_a <= 0) { stop_drilling(game, bird, false); return; }
    const Cell next = bird.cell + bird.facing;
    const int victim = entity_at(game, next, true);
    if (victim >= 0) {
        damage_entity(game, victim, 10, bird.cell);
        emit_sound(game, SoundId::WoodpeckerDrill, bird.cell);
        stop_drilling(game, bird, true);
        return;
    }
    const bool chipped_prop = hit_prop(game, next, 18, bird.cell);
    const bool chipped_tile = hit_terrain(game, next, bird.cell, 18, 1);
    if (chipped_prop || chipped_tile) emit_sound(game, SoundId::WoodpeckerDrill, next);
    const Tile* tile = game.stage.at(next);
    if (tile != nullptr && walkable(*tile)) {
        if (!move_entity(game, slot, next)) { stop_drilling(game, bird, true); return; }
        --bird.counter_a; bird.counter_b = 0;
    } else if (!(chipped_prop || chipped_tile) || ++bird.counter_b >= 6) stop_drilling(game, bird, true);
    else bird.move_wait = 6;
}

} // namespace

// SLOTS: point_a territory; label_a wander/rattle/drill/recover; timer_a phase;
// counter_a run length, counter_b pecks at this obstacle. Facing stays committed.
void init_woodpecker(Entity& bird) {
    bird.sprite = Sprite::Woodpecker;
    bird.health = bird.max_health = 30;
    bird.move_interval = 18;
    bird.impassable = true;
    bird.point_a = bird.cell;
}

void step_woodpecker(Game& game, int slot) {
    Entity& bird = game.entities[static_cast<std::size_t>(slot)];
    if (bird.label_a == 1) {
        if (bird.timer_a == 0) {
            bird.label_a = 2; bird.move_interval = 5; bird.move_wait = 0;
            bird.sprite = Sprite::WoodpeckerDrilling;
        }
        return;
    }
    if (bird.label_a == 2) { drill(game, slot); return; }
    if (bird.label_a == 3) {
        if (bird.timer_a == 0) bird.label_a = 0;
        return;
    }
    if (feed_on_bird_seed(game, slot)) return;
    const int target = nearest_player(game, bird.cell, 6);
    if (target < 0) {
        if (distance(bird.cell, bird.point_a) > 4) pursue(game, slot, bird.point_a);
        else if (game.tick % 30 == 0) wander(game, slot);
        return;
    }
    const Cell cell = game.entities[static_cast<std::size_t>(target)].cell;
    if (cell.x != bird.cell.x && cell.y != bird.cell.y) { approach(game, slot, cell); return; }
    // TARGETING: The beak can chip ordinary obstacles along a seen lane. After
    // commitment, newly placed cover does not magically change its direction.
    if (!clear_sight(game, bird.cell, cell)) return;
    bird.facing = cardinal_toward(bird.cell, cell, bird.facing);
    bird.label_a = 1; bird.timer_a = 30;
    bird.counter_a = 6; bird.counter_b = 0;
    emit_sound(game, SoundId::WoodpeckerRattle, bird.cell);
}
