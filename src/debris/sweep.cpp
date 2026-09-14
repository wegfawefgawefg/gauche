#include "sweep.hpp"
#include "air.hpp"
#include "system.hpp"
#include "../particles/system.hpp"

#include <algorithm>
#include <cmath>

// GATHER: A rake pulls scraps toward a narrow pile in front of its user. Scraps
// retain their kinds/counts and ordinary wall collision; this never changes tiles.
void rake_debris(LooseDebris& debris, const Stage& stage, const SweepEvent& sweep) {
    const float dx = static_cast<float>(sweep.direction.x);
    const float dy = static_cast<float>(sweep.direction.y);
    const float ox = static_cast<float>(sweep.source.x) + .5F;
    const float oy = static_cast<float>(sweep.source.y) + .5F;
    for (std::size_t index = 0; index < debris.pieces.size(); ++index) {
        LoosePiece& piece = debris.pieces[index];
        const float along = (piece.x - ox)*dx + (piece.y - oy)*dy;
        const float side = -(piece.x - ox)*dy + (piece.y - oy)*dx;
        if (along < .45F || along > static_cast<float>(sweep.reach) + .5F ||
            std::abs(side) > static_cast<float>(sweep.half_width) + .5F) continue;
        const int lane = static_cast<int>(std::round(side));
        bool blocked = false;
        for (int reach = 1; reach <= static_cast<int>(std::floor(along+.5F)); ++reach) {
            const Cell cell = sweep.source + Cell{sweep.direction.x*reach - sweep.direction.y*lane,
                                                  sweep.direction.y*reach + sweep.direction.x*lane};
            const Tile* tile = stage.at(cell);
            if (tile == nullptr || !walkable(*tile)) { blocked = true; break; }
        }
        if (blocked) continue;
        // DRAG: Geometric decay brings each piece near its intended pile instead of overshooting it.
        const float jitter = static_cast<float>((index*37) % 23) / 23 - .5F;
        const float gather = std::clamp(side * .16F + jitter*.20F, -.45F, .45F);
        const float depth = .78F + static_cast<float>((index*19) % 13) * .018F;
        const float drag = 1-debris_friction(piece.kind);
        piece.vx = (ox + dx*depth - dy*gather - piece.x) * drag;
        piece.vy = (oy + dy*depth + dx*gather - piece.y) * drag;
        piece.spin = jitter*8;
        piece.settled = 0;
    }
}

void observe_raking(Cosmetics& cosmetics, const Game& game) {
    for (int index = 0; index < game.sweep_count; ++index) {
        const auto key = (1ULL << 61) | (game.tick << 8) | (static_cast<std::uint64_t>(index)+1);
        if (std::find(cosmetics.seen_events.begin(), cosmetics.seen_events.end(), key) != cosmetics.seen_events.end()) continue;
        cosmetics.seen_events[cosmetics.next_event++ % cosmetics.seen_events.size()] = key;
        const SweepEvent& sweep = game.sweeps[static_cast<std::size_t>(index)];
        if (sweep.outward) blow_debris(cosmetics, game, sweep);
        else rake_debris(cosmetics.debris, game.stage, sweep);
    }
}
