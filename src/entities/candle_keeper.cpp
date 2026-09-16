#include "candle_keeper.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/candle.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {

// SLOTS: point_a territory, point_b guarded candle, label_b candle known.
// label_a phase, timer_a phase, timer_b lamp recharge; counter_a/b committed stance.
// entity_b witnessed thief/attacker, attack_wait grudge; c slots remain hearing-owned.
Cell stance(const Entity& keeper) { return {keeper.counter_a,keeper.counter_b}; }

void rest(Entity& keeper, int ticks) {
    keeper.label_a = KeeperRecover; keeper.timer_a = ticks;
    keeper.sprite = keeper.timer_b > 0 ? Sprite::KeeperDim : Sprite::CandleKeeper;
    keeper.self_light = {};
}

std::optional<Cell> threat(const Game& game, const Entity& keeper) {
    const Entity* culprit = get_entity(game,keeper.entity_b);
    if (keeper.attack_wait > 0 && culprit && culprit->health > 0 &&
        distance(keeper.point_a,culprit->cell) <= 9 && distance(keeper.cell,culprit->cell) <= 7 &&
        clear_attack_sight(game,keeper.cell,culprit->cell)) return culprit->cell;
    // GUARD: Intruders near a tended lamp matter; passers-by outside it can leave quietly.
    const auto chosen = enemy_target(game,keeper.cell,4);
    if (!chosen || !clear_attack_sight(game,keeper.cell,chosen->cell)) return std::nullopt;
    const Cell guard = keeper.label_b ? keeper.point_b : keeper.point_a;
    if (distance(guard,chosen->cell) > 2 || distance(keeper.point_a,chosen->cell) > 9) return std::nullopt;
    return chosen->cell;
}

void cast_flame(Game& game, int slot) {
    Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    const EnemyAttack attack = enemy_attack(keeper);
    std::array<bool,64> exposed{};
    // COVER: Breaking front cover cannot expose the second cell to this same cast.
    for (int i=0;i<attack.count;++i)
        exposed[static_cast<std::size_t>(i)] = clear_attack_sight(game,keeper.cell,attack.cells[static_cast<std::size_t>(i)],false);
    emit_sound(game,SoundId::KeeperCast,keeper.cell);
    for (int i=0;i<attack.count;++i) {
        const Cell cell = attack.cells[static_cast<std::size_t>(i)];
        if (!exposed[static_cast<std::size_t>(i)]) continue;
        // Pure flame lights a wick; it has no physical blow to smash the candle.
        if (game.stage.at_or_border(cell).prop.kind != PropKind::Candle)
            hit_prop(game,cell,10,keeper.cell);
        ignite_surface(game,cell);
        for (int victim=0;victim<max_entities;++victim) {
            Entity& actor = game.entities[static_cast<std::size_t>(victim)];
            if (victim == slot || actor.health <= 0 || actor.cell != cell) continue;
            const int health = actor.health;
            damage_entity(game,victim,10,keeper.cell);
            if (actor.health > 0 && actor.health < health && actor.move_interval > 0 &&
                !actor.hard_blocker && !surface_wet(game.stage.at_or_border(cell)))
                actor.scorch_ticks = std::max(actor.scorch_ticks,120);
        }
        emit_sound(game,SoundId::KeeperFlame,cell);
    }
    if (keeper.health <= 0) return;
    keeper.timer_b = 180; keeper.light = {};
    rest(keeper,54);
}

void tend(Game& game, int slot) {
    Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    // SEARCH: Movement beats bound the flood; a snuffed lamp can redirect the next beat.
    if (keeper.move_wait > 0) return;
    const auto candle = keeper_candle(game,slot);
    keeper.label_b = candle ? 1 : 0;
    if (!candle) {
        if (distance(keeper.cell,keeper.point_a) > 1) keeper_walk(game,slot,keeper.point_a);
        else keeper.move_wait = 30;
        return;
    }
    keeper.point_b = *candle;
    const Prop& prop = game.stage.at(*candle)->prop;
    if (distance(keeper.cell,*candle) > 1) { keeper_walk(game,slot,*candle); return; }
    if (candle_lit(prop) || keeper.timer_b > 0) { keeper.move_wait = 24; return; }
    keeper.facing = cardinal_toward(keeper.cell,*candle,keeper.facing);
    keeper.counter_a = keeper.cell.x; keeper.counter_b = keeper.cell.y;
    keeper.label_a = KeeperRelight; keeper.timer_a = 36;
    keeper.sprite = Sprite::KeeperTend;
    emit_sound(game,SoundId::KeeperTend,keeper.cell);
}

} // namespace

void init_candle_keeper(Entity& keeper) {
    keeper.health = keeper.max_health = 56;
    keeper.move_interval = 18; keeper.impassable = true;
    keeper.point_a = keeper.cell; keeper.sprite = Sprite::CandleKeeper;
    keeper.light = {3,850,{255,183,90}};
}

void interrupt_candle_keeper(Entity& keeper) {
    if (keeper.kind != EntityKind::CandleKeeper || keeper.health <= 0) return;
    rest(keeper,45);
}

bool douse_keeper_lamp(Game& game, Entity& keeper) {
    if (keeper.kind != EntityKind::CandleKeeper || keeper.health <= 0) return false;
    const bool lit = keeper.timer_b == 0;
    if (lit) emit_sound(game,SoundId::KeeperDouse,keeper.cell);
    keeper.timer_b = 180; keeper.light = {};
    if (keeper.label_a == KeeperStrike || keeper.label_a == KeeperRelight) rest(keeper,45);
    return lit;
}

void step_keeper_lamp(Game& game, int slot) {
    Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    if (keeper.kind != EntityKind::CandleKeeper) return;
    // WATER: Lamp state remains honest during sleep/stun too; no flame under a puddle.
    if (surface_wet(game.stage.at_or_border(keeper.cell))) douse_keeper_lamp(game,keeper);
    if (keeper.health <= 0 || keeper.timer_b > 0) {
        keeper.light = {};
        if (keeper.label_a == KeeperStrike || keeper.label_a == KeeperRelight) rest(keeper,45);
    } else {
        if (keeper.light.strength == 0) emit_sound(game,SoundId::KeeperRelight,keeper.cell);
        keeper.light = {3,850,{255,183,90}};
    }
    if (keeper.label_a == KeeperTend || keeper.label_a == KeeperRecover)
        keeper.sprite = keeper.timer_b > 0 ? Sprite::KeeperDim : Sprite::CandleKeeper;
}

void step_candle_keeper(Game& game, int slot) {
    Entity& keeper = game.entities[static_cast<std::size_t>(slot)];
    if (keeper.label_a == KeeperRecover) {
        if (keeper.timer_a == 0) keeper.label_a = KeeperTend;
        return;
    }
    if (keeper.label_a == KeeperStrike || keeper.label_a == KeeperRelight) {
        if (keeper.cell != stance(keeper) || keeper.vitals.rooted > 0 || keeper.timer_b > 0) { rest(keeper,45); return; }
        if (keeper.timer_a > 0) return;
        if (keeper.label_a == KeeperStrike) cast_flame(game,slot);
        else {
            if (distance(keeper.cell,keeper.point_b) <= 1) light_candle(game,keeper.point_b);
            rest(keeper,24);
        }
        return;
    }
    if (keeper.timer_b == 0)
        if (const auto target = threat(game,keeper)) {
            const Cell delta = *target-keeper.cell;
            if ((delta.x == 0 || delta.y == 0) && distance({},delta) >= 1 && distance({},delta) <= 2) {
                keeper.facing = cardinal_toward(keeper.cell,*target,keeper.facing);
                keeper.counter_a = keeper.cell.x; keeper.counter_b = keeper.cell.y;
                keeper.label_a = KeeperStrike; keeper.timer_a = 36;
                keeper.sprite = Sprite::KeeperStrike; keeper.self_light = {38,22,7};
                emit_sound(game,SoundId::KeeperWarn,keeper.cell);
            } else keeper_walk(game,slot,*target);
            return;
        }
    tend(game,slot);
}
