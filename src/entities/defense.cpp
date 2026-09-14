#include "attacks.hpp"

int enemy_defense(Game& game, int slot, int damage, Cell source, bool blockable) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    if (enemy.kind != EntityKind::ThornSnail || !blockable) return damage;
    const int attacker_slot = entity_at(game, source, true);
    const Entity* attacker = attacker_slot >= 0 ? &game.entities[static_cast<std::size_t>(attacker_slot)] : nullptr;
    const bool penetrates = attacker != nullptr &&
        (attacker->inventory.held()->dig_power > 0 ||
         attacker->inventory.held()->attribute == ItemAttribute::Piercing ||
         has_artifact(*attacker, ArtifactKind::AllPiercing));
    // SHELL: A rear strike still works while withdrawn; fire never becomes a shield hit.
    const Cell incoming = cardinal_toward(enemy.cell, source, enemy.facing);
    const bool rear = incoming == Cell{-enemy.facing.x, -enemy.facing.y};
    enemy.label_a = 1;
    enemy.timer_a = 90;
    enemy.hard_blocker = true;
    enemy.sprite = Sprite::ThornSnailClosed;
    if (rear || penetrates) return damage;
    emit_sound(game, SoundId::ShellKnock, enemy.cell);
    return 0;
}
