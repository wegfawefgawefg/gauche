#include "bird_feeding.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"

#include <optional>

namespace {

bool edible_seed(const Tile& tile) {
    return tile.prop.kind == PropKind::BirdSeed && !tile.prop.broken &&
        tile.prop.hp > 0 && tile.surface.fire_ticks == 0;
}

} // namespace

bool feed_on_bird_seed(Game& game, int slot) {
    Entity& bird = game.entities[static_cast<std::size_t>(slot)];
    std::optional<Cell> chosen, approach;
    int best = 9;
    constexpr Cell sides[]{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    // GATHER: Feed from neighboring cells, so one bird does not occupy the whole pile.
    // No extra actors are spawned and no AI-specific state slots are borrowed.
    for (int y = -8; y <= 8; ++y)
        for (int x = -8; x <= 8; ++x) {
            const Cell cell = bird.cell + Cell{x, y};
            const Tile* tile = game.stage.at(cell);
            const int gap = distance(bird.cell, cell);
            if (tile == nullptr || gap >= best || !edible_seed(*tile) ||
                !clear_sight(game, bird.cell, cell, false)) continue;
            if (gap <= 1) { chosen = cell; approach.reset(); best = gap; continue; }
            for (Cell side : sides) {
                const Cell edge = cell + side;
                const Tile* ground = game.stage.at(edge);
                if (ground == nullptr || !walkable(*ground) || entity_at(game, edge, true) >= 0) continue;
                const auto step = next_route_cell(game, slot, edge, 256);
                if (!step) continue;
                chosen = cell; approach = *step; best = gap;
                break;
            }
        }
    if (!chosen) return false;
    if (bird.move_wait > 0) return true;
    if (approach) willing_step(game, slot, *approach);
    else {
        bird.facing = cardinal_toward(bird.cell, *chosen, bird.facing);
        if (bird.attack_wait == 0) {
            hit_prop(game, *chosen, 1, bird.cell);
            bird.attack_wait = 45;
            bird.use_flash = 5;
            emit_sound(game, SoundId::SeedPeck, bird.cell);
        }
        bird.move_wait = 6;
    }
    return true;
}
