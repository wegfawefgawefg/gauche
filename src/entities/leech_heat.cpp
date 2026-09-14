#include "steam_leech.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include "attacks.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

bool leech_source_cell(const Game& game, const Entity& leech, Cell& cell) {
    if (leech.label_b == 1) {
        cell = leech.point_b;
        const Tile* tile = game.stage.at(cell);
        return tile && (prop_has_flame(tile->prop) || tile->surface.warmth_ticks > 0 || tile->surface.fire_ticks > 0 ||
            tile->kind == TileKind::Lava);
    }
    const Entity* source = get_entity(game, leech.entity_a);
    if (source == nullptr || !entity_has_flame(*source)) return false;
    cell = source->cell;
    return true;
}

bool leech_attached(const Game& game, const Entity& leech) {
    if (leech.kind != EntityKind::SteamLeech || leech.health <= 0 ||
        leech.cell != leech.point_a || leech.sleep_ticks > 0 || leech.stun_ticks > 0 ||
        leech.freeze_ticks > 0 || (leech.label_a != LeechFeed && leech.label_a != LeechSwell)) return false;
    Cell source;
    return leech_source_cell(game, leech, source) && distance(leech.cell, source) <= 1 &&
        clear_attack_sight(game, leech.cell, source, false);
}

std::vector<Cell> leech_drain_cells(const Game& game) {
    std::vector<Cell> cells;
    for (const Entity& leech : game.entities) {
        if (!leech_attached(game, leech)) continue;
        Cell source;
        if (leech_source_cell(game, leech, source) &&
            std::find(cells.begin(), cells.end(), source) == cells.end()) cells.push_back(source);
    }
    return cells;
}

bool leech_drains_cell(const Game& game, Cell cell) {
    for (const Entity& leech : game.entities) {
        if (!leech_attached(game, leech)) continue;
        Cell source;
        if (leech_source_cell(game, leech, source) && source == cell) return true;
    }
    return false;
}
