#include "seal_thief.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "scavenging.hpp"
#include "../item_pattern.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a phase; timer_a phase; timer_b search; counter_a satiety.
// point_a bank/stance origin, point_b bite cell, entity_a loose food.
// The actual stolen item stays in held inventory until eaten or dropped on death.
bool carrying(const Entity& seal) { return seal.inventory.held()->count > 0; }

void rest(Entity& seal) {
    seal.label_a = SealRecover; seal.timer_a = 48;
    seal.sprite = Sprite::SealRest;
}

std::optional<Cell> nearby_threat(const Game& game, const Entity& seal) {
    const auto target = enemy_target(game,seal.cell,5);
    if (target && clear_attack_sight(game,seal.cell,target->cell)) return target->cell;
    return {};
}

void forage(Game& game, int slot) {
    Entity& seal = game.entities[static_cast<std::size_t>(slot)];
    if (seal.counter_a > 0) { wander(game,slot); return; }
    if (seal.timer_b == 0) {
        const SealRoute route = seal_route(game,slot,true);
        seal.entity_a = route.food;
        seal.timer_b = 30;
    }
    Entity* food = get_entity(game,seal.entity_a);
    if (!food || food->kind != EntityKind::GroundItem || !seal_food(food->ground_item.kind) ||
        food->ground_item.count <= 0 || food->ground_item.flame_ticks > 0) {
        seal.entity_a = {};
        wander(game,slot);
        return;
    }
    if (collect_scavenge(game,slot,seal.entity_a,true,SoundId::SealSnatch)) {
        seal.entity_a = {};
        seal.label_a = SealRetreat;
        seal.sprite = Sprite::SealCarry;
        seal.timer_b = 0;
        return;
    }
    if (seal.move_wait > 0 || seal.vitals.rooted > 0) return;
    const SealRoute route = seal_route(game,slot,true);
    // MOVED FOOD: Retarget from current state instead of chasing a stale location.
    seal.entity_a = route.food;
    if (route.found && route.next != seal.cell) willing_step(game,slot,route.next);
    else seal.move_wait = seal.move_interval;
}

void retreat(Game& game, int slot, std::optional<Cell> threat) {
    Entity& seal = game.entities[static_cast<std::size_t>(slot)];
    seal.sprite = Sprite::SealCarry;
    if (seal.move_wait > 0 || seal.timer_b > 0) return;
    const SealRoute route = seal_route(game,slot,false,threat);
    if (!route.found) {
        // DRY ROOM: Keep the stolen food recoverable; wait for an escape to reopen.
        seal.timer_b = 30;
        if (threat) flee(game,slot,*threat);
        return;
    }
    seal.point_a = route.end;
    if (route.next != seal.cell) { willing_step(game,slot,route.next); return; }
    if (threat && distance(seal.cell,*threat) <= 2) { flee(game,slot,*threat); return; }
    seal.label_a = SealEat; seal.timer_a = 180;
    seal.sprite = Sprite::SealEat;
    emit_sound(game,SoundId::SealSettle,seal.cell);
}

void eat(Game& game, Entity& seal, std::optional<Cell> threat) {
    if (seal.cell != seal.point_a || !seal_bank(game,seal.cell) ||
        (threat && distance(seal.cell,*threat) <= 2)) {
        seal.label_a = SealRetreat; seal.timer_b = 0;
        seal.sprite = Sprite::SealCarry;
        return;
    }
    if (seal.timer_a > 0) return;
    const Item meal = *seal.inventory.held();
    seal.health = std::min(seal.max_health,seal.health+item_pattern(meal).heal);
    *seal.inventory.held() = {};
    seal.counter_a = 480; seal.label_a = SealForage;
    seal.sprite = Sprite::SealThief;
    emit_sound(game,meal.kind == ItemKind::SmokedFish ? SoundId::FishNibble : SoundId::SealEat,seal.cell);
}

} // namespace

void init_seal_thief(Entity& seal) {
    seal.health = seal.max_health = 56;
    seal.impassable = true;
    seal.move_interval = 14;
    seal.sprite = Sprite::SealThief;
}

void interrupt_seal_thief(Entity& seal) {
    if (seal.kind != EntityKind::SealThief || seal.health <= 0) return;
    rest(seal);
    seal.timer_b = 0;
}

void step_seal_thief(Game& game, int slot) {
    Entity& seal = game.entities[static_cast<std::size_t>(slot)];
    seal.counter_a = std::max(0,seal.counter_a-1);
    seal.move_interval = carrying(seal) ? 7 : 14;
    if (seal.label_a == SealBark) {
        if (seal.cell != seal.point_a) { rest(seal); return; }
        if (seal.timer_a == 0) { resolve_enemy_attack(game,slot,16,SoundId::SealBite); rest(seal); }
        return;
    }
    if (seal.label_a == SealRecover) {
        if (seal.timer_a == 0) {
            seal.label_a = carrying(seal) ? SealRetreat : SealForage;
            seal.sprite = carrying(seal) ? Sprite::SealCarry : Sprite::SealThief;
        }
        return;
    }
    const auto threat = nearby_threat(game,seal);
    if (threat && distance(seal.cell,*threat) == 1) {
        seal.point_a = seal.cell; seal.point_b = *threat;
        seal.facing = *threat-seal.cell;
        seal.label_a = SealBark; seal.timer_a = 30;
        seal.sprite = Sprite::SealBark;
        emit_sound(game,SoundId::SealBark,seal.cell);
        return;
    }
    if (carrying(seal)) {
        if (seal.label_a == SealEat) eat(game,seal,threat);
        else { seal.label_a = SealRetreat; retreat(game,slot,threat); }
    } else { seal.label_a = SealForage; seal.sprite = Sprite::SealThief; forage(game,slot); }
}
