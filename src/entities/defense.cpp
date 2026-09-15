#include "slag_snail.hpp"
#include "walking_kiln.hpp"
#include "attacks.hpp"
#include "bell_diver.hpp"
#include "frozen_pilgrim.hpp"
#include "../item_pattern.hpp"

#include <algorithm>

int enemy_defense(Game& game, int slot, int damage, Cell source, bool blockable) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    if (!blockable) return damage;
    if (slag_snail_shelled(enemy)) {
        emit_sound(game,SoundId::SlagImpact,enemy.cell);
        return std::max(1,damage/3);
    }
    if (kiln_open(enemy)) return std::min(damage,1000000)*2;
    // DEPTH: Ordinary strikes pass above the swimmer. Area shocks still reach it.
    if (diver_submerged(enemy)) return 0;
    // CRUST: Thawing sheds protection. Heat and unblocked damage bypass the ice.
    if (pilgrim_crusted(enemy)) {
        emit_sound(game, SoundId::PilgrimShellHit, enemy.cell);
        return std::max(1, damage / 2);
    }
    if (enemy.kind == EntityKind::BurrowWorm && enemy.label_a == 1) {
        const int attacker = entity_at(game, source, true);
        const Entity* user = attacker < 0 ? nullptr : &game.entities[static_cast<std::size_t>(attacker)];
        const bool pierces = user != nullptr && (item_pattern(*user->inventory.held()).piercing ||
            has_artifact(*user, ArtifactKind::AllPiercing));
        return pierces ? damage : std::max(1, damage / 2);
    }
    if (enemy.kind != EntityKind::ThornSnail) return damage;
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
