#include "snow_effigy.hpp"
#include "../items/effigy_mask.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

// SLOTS: label_a phase, label_b awakened, timer_a strike/recovery.
// counter_a watched last tick, counter_b exposed frame; point_a stance, point_b strike.
// Sleep/stun keep the shell's thermal update in the timer phase.
Sprite resting_sprite(const Entity& effigy) {
    return effigy.counter_b ? Sprite::EffigyFrame : Sprite::SnowEffigy;
}

void rest(Entity& effigy, int ticks) {
    effigy.label_a = EffigyRecover; effigy.timer_a = ticks;
    effigy.sprite = resting_sprite(effigy); effigy.self_light = {};
}

bool watched(const Game& game, const Entity& effigy) {
    for (std::size_t owner=0;owner<game.players.size();++owner) {
        if (!game.run.online[owner]) continue;
        const Entity* player = get_entity(game,game.players[owner]);
        if (player && player->health > 0 && player->sleep_ticks == 0 &&
            (observer_faces_cell(game,*player,effigy.cell) || (effigy_mask_active(*player) &&
             observer_faces_direction(game,*player,effigy.cell,Cell{-player->facing.x,-player->facing.y})))) return true;
    }
    return false;
}

std::optional<Cell> target_cell(const Game& game, const Entity& effigy) {
    std::optional<Cell> chosen;
    int best = 8;
    for (std::size_t owner=0;owner<game.players.size();++owner) {
        if (!game.run.online[owner]) continue;
        const Entity* player = get_entity(game,game.players[owner]);
        if (!player || player->health <= 0) continue;
        const int gap = distance(effigy.cell,player->cell);
        if (gap < best && clear_attack_sight(game,effigy.cell,player->cell)) {
            best = gap; chosen = player->cell;
        }
    }
    const auto decoy = enemy_target(game,effigy.cell,7,false);
    if (decoy && distance(effigy.cell,decoy->cell)-2 < best) chosen = decoy->cell;
    return chosen;
}

} // namespace

bool observer_faces_cell(const Game& game, const Entity& observer, Cell cell) {
    return observer_faces_direction(game,observer,cell,observer.facing);
}

bool observer_faces_direction(const Game& game,const Entity& observer,Cell cell,Cell facing) {
    const Cell delta = cell-observer.cell;
    if (distance({},delta) > 7) return false;
    const int forward = delta.x*facing.x + delta.y*facing.y;
    const int sideways = delta.x*facing.y - delta.y*facing.x;
    // GAZE: Cardinal 90-degree cone, actual tiles and sight blockers; never camera state.
    return (delta == Cell{} || (forward > 0 && std::abs(sideways) <= forward)) &&
        clear_sight(game,observer.cell,cell);
}

void init_snow_effigy(Entity& effigy) {
    effigy.health = effigy.max_health = 60;
    effigy.move_interval = 18; effigy.impassable = true;
    effigy.sprite = Sprite::SnowEffigy;
}

void interrupt_snow_effigy(Entity& effigy) {
    if (effigy.kind != EntityKind::SnowEffigy || effigy.health <= 0) return;
    // PROVOCATION: Striking a dormant statue wakes it, without granting an immediate swing.
    effigy.label_b = 1;
    rest(effigy,45);
}

void thaw_snow_effigy(Game& game, int slot) {
    Entity& effigy = game.entities[static_cast<std::size_t>(slot)];
    if (effigy.kind != EntityKind::SnowEffigy || effigy.health <= 0 || effigy.counter_b) return;
    if (effigy.burn_ticks <= 0 && effigy.scorch_ticks <= 0 && !hot_cell(game,effigy.cell)) return;
    effigy.counter_b = 1; effigy.label_b = 1;
    effigy.max_health = 18; effigy.health = std::min(effigy.health,18);
    rest(effigy,45);
    emit_sound(game,SoundId::EffigyThaw,effigy.cell);
}

void step_snow_effigy(Game& game, int slot) {
    Entity& effigy = game.entities[static_cast<std::size_t>(slot)];
    if (watched(game,effigy)) {
        if (!effigy.label_b) emit_sound(game,SoundId::EffigyWake,effigy.cell);
        else if (!effigy.counter_a) emit_sound(game,SoundId::EffigyStill,effigy.cell);
        effigy.label_b = 1; effigy.counter_a = 1;
        if (effigy.label_a == EffigyStrike) rest(effigy,24);
        effigy.sprite = resting_sprite(effigy); effigy.self_light = {};
        return;
    }
    if (!effigy.label_b) return;
    if (effigy.counter_a) { effigy.counter_a = 0; emit_sound(game,SoundId::EffigyCreak,effigy.cell); }
    if (effigy.label_a == EffigyRecover) {
        if (effigy.timer_a == 0) effigy.label_a = EffigyIdle;
        return;
    }
    if (effigy.label_a == EffigyStrike) {
        if (effigy.cell != effigy.point_a || effigy.vitals.rooted > 0) { rest(effigy,45); return; }
        if (effigy.timer_a == 0) {
            resolve_enemy_attack(game,slot,16,SoundId::EffigyHit);
            if (effigy.health > 0) rest(effigy,54);
        }
        return;
    }
    const auto target = target_cell(game,effigy);
    if (!target) return;
    if (distance(effigy.cell,*target) == 1) {
        effigy.facing = *target-effigy.cell;
        effigy.point_a = effigy.cell; effigy.point_b = *target;
        effigy.label_a = EffigyStrike; effigy.timer_a = 30;
        effigy.sprite = effigy.counter_b ? Sprite::EffigyFrameStrike : Sprite::EffigyStrike;
        effigy.self_light = {36,29,13};
        emit_sound(game,SoundId::EffigyWarn,effigy.cell);
    } else pursue(game,slot,*target);
}
