#include "snow_burrower.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../world/snow.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a burrow/warn/exposed/dive; timer_a phase; timer_b idle rumble.
// point_a committed origin, point_b committed bite cell. Terrain owns cleared ground.
constexpr Cell sides[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

void crawl(Game& game, int slot, Cell target, bool under_snow) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.move_wait > 0 || actor.vitals.rooted > 0) return;
    struct Node { Cell cell{}, first{}; int depth = 0; };
    std::array<Node, 145> nodes{}; // Eight-step diamond; no whole-map allocation.
    nodes[0] = {actor.cell, actor.cell, 0};
    int count = 1, best = -1, gap = distance(actor.cell, target);
    const int rotation = static_cast<int>(random_u32(game) % 4);
    for (int next = 0; next < count; ++next) {
        const Node node = nodes[static_cast<std::size_t>(next)];
        if (node.depth == 8) continue;
        for (int i = 0; i < 4; ++i) {
            const Cell cell = node.cell + sides[(rotation + i) % 4];
            const Tile* tile = game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind == TileKind::Lava ||
                (under_snow && !burrowable_snow(game, cell)) || entity_at(game, cell, true) >= 0) continue;
            bool seen = false;
            for (int j = 0; j < count; ++j)
                if (nodes[static_cast<std::size_t>(j)].cell == cell) { seen = true; break; }
            if (seen || count == static_cast<int>(nodes.size())) continue;
            const int at = count++;
            nodes[static_cast<std::size_t>(at)] = {cell, next == 0 ? cell : node.first, node.depth + 1};
            const int candidate = under_snow ? distance(cell, target) : burrowable_snow(game, cell) ? node.depth : 100;
            if (candidate < gap || (!under_snow && best < 0 && candidate < 100)) { best = at; gap = candidate; }
        }
    }
    if (best >= 0 && willing_step(game, slot, nodes[static_cast<std::size_t>(best)].first)) return;
    // IDLE: Choose among available neighbors, not repeated failed rolls against one wall.
    if (under_snow)
        for (int i = 0; i < 4; ++i) {
            const Cell cell = actor.cell + sides[(rotation + i) % 4];
            if (burrowable_snow(game, cell) && willing_step(game, slot, cell)) return;
        }
    actor.move_wait = 24;
}

void expose(Entity& actor, int ticks) {
    actor.label_a = SnowExposed;
    actor.timer_a = ticks;
    actor.sprite = Sprite::SnowBurrower;
    actor.move_interval = 28;
}

} // namespace

void init_snow_burrower(Entity& actor) {
    actor.health = actor.max_health = 36;
    actor.move_interval = 12;
    actor.impassable = true;
    actor.sprite = Sprite::SnowMound;
    actor.point_a = actor.cell;
}

void expose_snow_burrower(Entity& actor) {
    if (actor.kind != EntityKind::SnowBurrower || actor.health <= 0) return;
    expose(actor, 90);
}

void step_snow_burrower(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const bool snow = burrowable_snow(game, actor.cell);
    // EXPOSE: A shove onto cleared ground or a melted mound ends the underground action.
    if (!snow && actor.label_a != SnowExposed) expose(actor, 90);
    if (actor.label_a == SnowWarn) {
        if (actor.cell != actor.point_a) { expose(actor, 90); return; }
        if (actor.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 14, SoundId::SnowBurst);
        expose(actor, 75);
        return;
    }
    if (actor.label_a == SnowDive) {
        if (actor.cell != actor.point_a) { expose(actor, 90); return; }
        if (actor.timer_a > 0) return;
        actor.label_a = SnowBurrow;
        actor.sprite = Sprite::SnowMound;
        actor.move_interval = 12;
        return;
    }
    if (actor.label_a == SnowExposed) {
        if (actor.timer_a > 0) return;
        if (snow) {
            actor.label_a = SnowDive;
            actor.timer_a = 30;
            actor.point_a = actor.cell;
            actor.sprite = Sprite::SnowDive;
            emit_sound(game, SoundId::SnowDig, actor.cell);
        } else crawl(game, slot, actor.cell, false);
        return;
    }
    const auto target = enemy_target(game, actor.cell, 7);
    if (target && distance(actor.cell, target->cell) == 1 && clear_attack_sight(game, actor.cell, target->cell)) {
        actor.label_a = SnowWarn;
        actor.timer_a = 27;
        actor.point_a = actor.cell;
        actor.point_b = target->cell;
        actor.facing = actor.point_b - actor.cell;
        actor.sprite = Sprite::SnowWarn;
        emit_sound(game, SoundId::SnowWarning, actor.cell);
        return;
    }
    if (actor.timer_b == 0) { emit_sound(game, SoundId::SnowRumble, actor.cell); actor.timer_b = 150; }
    crawl(game, slot, target ? target->cell : actor.cell, true);
}
