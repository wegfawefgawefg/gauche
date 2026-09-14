#include "avalanche_ram.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../combat/shove.hpp"
#include "../props/interaction.hpp"
#include "../world/snow.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase, timer_a phase, counter_a remaining two-cell lunge.
// point_a expected position, point_b committed heading. c-slots remain hearing.
void recover(Entity& ram, bool collision) {
    ram.label_a = collision ? RamStagger : RamRecover;
    ram.timer_a = collision ? 90 : 48;
    ram.move_interval = 20;
    ram.sprite = collision ? Sprite::RamStagger : Sprite::AvalancheRam;
    if (collision) ram.stun_ticks = std::max(ram.stun_ticks,75);
}

void collide(Game& game, Entity& ram) {
    emit_sound(game,SoundId::RamBonk,ram.cell);
    recover(ram,true);
}

void headbutt(Game& game, int slot) {
    Entity& ram = game.entities[static_cast<std::size_t>(slot)];
    if (ram.move_wait > 0) return;
    const Cell next = ram.cell+ram.point_b;
    const Tile* tile = game.stage.at(next);
    if (!tile) { collide(game,ram); return; }
    // PLOUGH: Clear real snow and its hidden inhabitants. Stone and timber resist.
    if (clear_snow(game,next)) emit_sound(game,SoundId::RamPlough,next);
    if (tile->prop.kind == PropKind::IceBlock && !tile->prop.broken)
        hit_prop(game,next,prop_max_health(tile->prop),ram.cell);
    if (!walkable(*tile)) { collide(game,ram); return; }
    const int victim_slot = entity_at(game,next,true);
    if (victim_slot >= 0) {
        Entity& victim = game.entities[static_cast<std::size_t>(victim_slot)];
        if (victim.hard_blocker) { collide(game,ram); return; }
        const bool guard = blocks_facing(victim,ram.cell);
        damage_entity(game,victim_slot,12,ram.cell);
        if (ram.health <= 0) return;
        if (!guard && victim.health > 0) shove_actor(game,victim_slot,ram.point_b,ram.cell);
        emit_sound(game,SoundId::RamHit,next);
        // GUARD: A facing buckler catches the horns; no hidden shove through a block.
        if (guard) collide(game,ram);
        else recover(ram,false);
        return;
    }
    if (!move_entity(game,slot,next,false)) { collide(game,ram); return; }
    if (ram.health <= 0) return;
    ram.point_a = next;
    // DISPLACEMENT: A spring, slide or portal owns the landing and ends this lunge.
    if (ram.cell != next || ram.label_a != RamLunge || --ram.counter_a <= 0) recover(ram,false);
}

} // namespace

void init_avalanche_ram(Entity& ram) {
    ram.health = ram.max_health = 76;
    ram.move_interval = 20;
    ram.impassable = true;
    ram.sprite = Sprite::AvalancheRam;
}

void interrupt_avalanche_ram(Entity& ram) {
    if (ram.kind == EntityKind::AvalancheRam && ram.health > 0 &&
        (ram.label_a == RamPaw || ram.label_a == RamLunge)) recover(ram,false);
}

void step_avalanche_ram(Game& game, int slot) {
    Entity& ram = game.entities[static_cast<std::size_t>(slot)];
    if (ram.label_a == RamRecover || ram.label_a == RamStagger) {
        if (ram.timer_a == 0) { ram.label_a = RamRoam; ram.sprite = Sprite::AvalancheRam; }
        return;
    }
    if (ram.label_a == RamPaw || ram.label_a == RamLunge) {
        if (ram.cell != ram.point_a || ram.vitals.rooted > 0) { recover(ram,false); return; }
        ram.facing = ram.point_b;
        if (ram.label_a == RamPaw) {
            if (ram.timer_a == 0) {
                ram.label_a = RamLunge; ram.sprite = Sprite::RamLunge;
                ram.move_interval = 6; ram.move_wait = 0;
                emit_sound(game,SoundId::RamRush,ram.cell);
            }
        } else headbutt(game,slot);
        return;
    }
    if (step_hearing(game,slot)) return;
    const auto target = enemy_target(game,ram.cell,6);
    if (!target || !clear_attack_sight(game,ram.cell,target->cell)) { wander(game,slot); return; }
    const Cell cell = target->cell;
    if (distance(ram.cell,cell) <= 2 && (cell.x == ram.cell.x || cell.y == ram.cell.y)) {
        ram.facing = cardinal_toward(ram.cell,cell,ram.facing);
        ram.point_a = ram.cell; ram.point_b = ram.facing;
        ram.label_a = RamPaw; ram.timer_a = 36; ram.counter_a = 2;
        ram.sprite = Sprite::RamPaw;
        emit_sound(game,SoundId::RamPaw,ram.cell);
    } else pursue(game,slot,cell);
}
