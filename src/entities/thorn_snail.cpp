#include "behavior.hpp"
#include "dispatch.hpp"

// SLOTS: label_a crawling/withdrawn; timer_a shell duration; facing is its armored front.
void init_thorn_snail(Entity& snail) {
    snail.sprite = Sprite::ThornSnail;
    snail.health = snail.max_health = 42;
    snail.move_interval = 45;
    snail.attack_interval = 75;
    snail.impassable = true;
}

void step_thorn_snail(Game& game, int slot) {
    Entity& snail = game.entities[static_cast<std::size_t>(slot)];
    if (snail.label_a == 1) {
        if (snail.timer_a == 0) {
            snail.label_a = 0;
            snail.hard_blocker = false;
            snail.sprite = Sprite::ThornSnail;
        }
        return;
    }
    const int target = nearest_player(game, snail.cell, 5);
    if (target < 0) { wander(game, slot); return; }
    const Cell cell = game.entities[static_cast<std::size_t>(target)].cell;
    if (distance(snail.cell, cell) > 1) approach(game, slot, cell);
    else if (snail.attack_wait == 0) {
        snail.facing = cardinal_toward(snail.cell, cell, snail.facing);
        damage_entity(game, target, 6, snail.cell);
        snail.attack_wait = snail.attack_interval;
        emit_sound(game, SoundId::ShellScrape, snail.cell);
    }
}
