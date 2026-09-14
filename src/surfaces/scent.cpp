#include "scent.hpp"
#include "interaction.hpp"
#include "../entities/behavior.hpp"
#include "../entities/hearing.hpp"

#include <algorithm>
#include <vector>

namespace {

bool scented(const Tile& tile) {
    return !surface_wet(tile) && tile.surface.fire_ticks == 0 &&
        (tile.surface.scent_ticks > 0 || (tile.surface.liquid == LiquidKind::Rot && tile.surface.liquid_ticks > 0));
}

} // namespace

bool step_scent(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    // ATTENTION: Noise takes priority; species decide whether food/combat comes first.
    if (actor.timer_c > 0 && actor.label_c != FollowScent && actor.label_c != SniffScent) return false;
    if (nearest_player(game, actor.cell, 1) >= 0) {
        if (actor.label_c == FollowScent || actor.label_c == SniffScent) actor.timer_c = actor.label_c = 0;
        return false;
    }
    if (actor.label_c == SniffScent && actor.timer_c > 0) return true;
    if (actor.move_wait > 0) return actor.label_c == FollowScent && actor.timer_c > 0;
    std::vector<Cell> choices;
    for (int y = std::max(0, actor.cell.y - 8); y <= std::min(game.stage.height - 1, actor.cell.y + 8); ++y)
        for (int x = std::max(0, actor.cell.x - 8); x <= std::min(game.stage.width - 1, actor.cell.x + 8); ++x) {
            const Cell cell{x, y};
            const Tile& tile = *game.stage.at(cell);
            if (distance(actor.cell, cell) > 8 || !scented(tile) || !walkable(tile)) continue;
            const int occupant = entity_at(game, cell, true);
            if (occupant < 0 || occupant == slot) choices.push_back(cell);
        }
    std::stable_sort(choices.begin(), choices.end(), [&](Cell a, Cell b) {
        return distance(actor.cell, a) < distance(actor.cell, b);
    });
    for (Cell cell : choices) {
        if (cell == actor.cell) {
            Surface& scent = game.stage.at(cell)->surface;
            if (scent.liquid == LiquidKind::Rot && scent.liquid_ticks > 0) {
                scent.liquid_ticks = static_cast<std::uint16_t>(std::max(0, scent.liquid_ticks - 180));
                if (scent.liquid_ticks == 0) scent.liquid = LiquidKind::None;
                apply_nausea(actor, 180);
                emit_sound(game, SoundId::FruitMunch, cell);
            } else {
                scent.scent_ticks = static_cast<std::uint16_t>(std::max(0, scent.scent_ticks - 90));
                emit_sound(game, SoundId::ScentSniff, cell);
            }
            actor.point_c = cell; actor.label_c = SniffScent; actor.timer_c = 60;
            return true;
        }
        // ROUTES: A sealed room cannot monopolize the animal's attention.
        if (const auto next = next_route_cell(game, slot, cell, 256)) {
            actor.point_c = cell; actor.label_c = FollowScent; actor.timer_c = 60;
            willing_step(game, slot, *next);
            return true;
        }
    }
    actor.timer_c = actor.label_c = 0;
    return false;
}
