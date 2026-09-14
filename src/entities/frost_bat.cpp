#include "frost_bat.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "../projectiles/frost.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a hang/inhale/relocate/rest; timer_a phase; counter_a puff range.
// point_a chosen perch; point_b inhale cell; facing holds the committed breath lane.
enum { Hang, Inhale, Relocate, Rest };
constexpr Cell sides[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

void rest(Entity& bat) {
    bat.label_a = Rest;
    bat.timer_a = 50;
    bat.sprite = Sprite::FrostBat;
}

bool wall_perch(const Game& game, Cell cell) {
    for (Cell side : sides)
        if (game.stage.at_or_border(cell + side).kind == TileKind::Wall) return true;
    return false;
}

void relocate(Game& game, Entity& bat, Cell threat, bool retreat) {
    Cell best = bat.cell;
    int score = -1000;
    // PERCHES: Prefer nearby wall edges, with a clear approach and breathing lane.
    // A bounded search also gives a stranded bat an ordinary open-cell fallback.
    for (int y = -3; y <= 3; ++y)
        for (int x = -3; x <= 3; ++x) {
            const Cell cell = bat.cell + Cell{x, y};
            const int travel = distance(cell, bat.cell), gap = distance(cell, threat);
            const Tile* tile = game.stage.at(cell);
            if (travel == 0 || travel > 4 || gap < 2 || gap > 7 || tile == nullptr ||
                !walkable(*tile) || tile->kind == TileKind::Lava || entity_at(game, cell, true) >= 0 ||
                !clear_sight(game, bat.cell, cell)) continue;
            const bool aligned = cell.x == threat.x || cell.y == threat.y;
            const int candidate = (wall_perch(game, cell) ? 8 : 0) +
                (aligned && clear_attack_sight(game, cell, threat) ? 10 : 0) +
                (retreat ? std::min(gap, 5) * 3 : 0) - travel;
            if (candidate > score) { best = cell; score = candidate; }
        }
    if (best == bat.cell) { rest(bat); return; }
    bat.point_a = best;
    bat.label_a = Relocate;
    bat.timer_a = 70;
    bat.move_wait = 0;
    bat.sprite = Sprite::FrostBatFlying;
    emit_sound(game, SoundId::FrostFlutter, bat.cell);
}

void fly_to_perch(Game& game, int slot) {
    Entity& bat = game.entities[static_cast<std::size_t>(slot)];
    if (bat.cell == bat.point_a || bat.timer_a == 0) { rest(bat); return; }
    if (bat.move_wait > 0) return;
    const auto step = next_route_cell(game, slot, bat.point_a, 256);
    if (!step || !willing_step(game, slot, *step)) rest(bat);
}

} // namespace

void init_frost_bat(Entity& bat) {
    bat.sprite = Sprite::FrostBat;
    bat.health = bat.max_health = 24;
    bat.impassable = true;
    bat.move_interval = 10;
    bat.counter_a = 6;
    bat.point_a = bat.cell;
}

void step_frost_bat(Game& game, int slot) {
    Entity& bat = game.entities[static_cast<std::size_t>(slot)];
    if (bat.label_a == Rest) {
        if (bat.timer_a == 0) bat.label_a = Hang;
        return;
    }
    if (bat.label_a == Relocate) { fly_to_perch(game, slot); return; }
    if (bat.label_a == Inhale) {
        // DISPLACEMENT: A shove invalidates the old tell instead of moving its attack.
        if (bat.cell != bat.point_b) { rest(bat); return; }
        if (bat.timer_a > 0) return;
        if (launch_frost_puff(game, slot, bat.facing, bat.counter_a))
            emit_sound(game, SoundId::FrostExhale, bat.cell);
        const auto target = enemy_target(game, bat.cell, 8);
        relocate(game, bat, target ? target->cell : bat.cell + bat.facing, true);
        return;
    }
    if (step_hearing(game, slot)) { bat.sprite = Sprite::FrostBatFlying; return; }
    bat.sprite = Sprite::FrostBat;
    if (bat.timer_a > 0) return;
    bat.timer_a = 15;
    const auto target = enemy_target(game, bat.cell, 8);
    if (!target || !clear_attack_sight(game, bat.cell, target->cell)) return;
    const Cell cell = target->cell;
    if (distance(cell, bat.cell) <= bat.counter_a &&
        (cell.x == bat.cell.x || cell.y == bat.cell.y)) {
        bat.facing = cardinal_toward(bat.cell, cell, bat.facing);
        bat.point_b = bat.cell;
        bat.label_a = Inhale;
        bat.timer_a = 30;
        bat.sprite = Sprite::FrostBatInhale;
        emit_sound(game, SoundId::FrostInhale, bat.cell);
    } else relocate(game, bat, cell, false);
}
