#include "air.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>
#include <cmath>

void blow_debris(Cosmetics& cosmetics, const Game& game, const SweepEvent& sweep) {
    const float dx = static_cast<float>(sweep.direction.x), dy = static_cast<float>(sweep.direction.y);
    const float ox = static_cast<float>(sweep.source.x) + .5F, oy = static_cast<float>(sweep.source.y) + .5F;
    // GUST: Local leaves and cases scatter outward; they never push gameplay objects.
    for (LoosePiece& piece : cosmetics.debris.pieces) {
        const float along = (piece.x - ox) * dx + (piece.y - oy) * dy;
        const float side = (piece.y - oy) * dx - (piece.x - ox) * dy;
        const int reach = static_cast<int>(std::round(along));
        if (reach < 1 || reach > sweep.reach ||
            std::abs(side) > static_cast<float>(std::min(reach - 1, sweep.half_width)) + .5F ||
            !clear_sight(game, sweep.source,
                {static_cast<int>(std::floor(piece.x)), static_cast<int>(std::floor(piece.y))}, false)) continue;
        piece.vx += dx * .22F - dy * side * .035F;
        piece.vy += dy * .22F + dx * side * .035F;
        piece.spin += side * 4;
        piece.settled = 0;
    }
    // WAVES: Brief curved strokes show the gust without leaving an attack-grid overlay.
    for (int reach = 1; reach <= sweep.reach && cosmetics.ribbons.size() < 256; ++reach) {
        const Cell center = sweep.source + Cell{sweep.direction.x * reach, sweep.direction.y * reach};
        if (!clear_sight(game, sweep.source, center, false)) break;
        RibbonParticle wave;
        wave.count = 3;
        const float radius = static_cast<float>(std::min(reach - 1, sweep.half_width)) + .45F;
        const float depth = static_cast<float>(reach);
        wave.points[0] = {ox + dx * (depth - .25F) + dy * radius, oy + dy * (depth - .25F) - dx * radius};
        wave.points[1] = {ox + dx * (depth + .15F), oy + dy * (depth + .15F)};
        wave.points[2] = {ox + dx * (depth - .25F) - dy * radius, oy + dy * (depth - .25F) + dx * radius};
        wave.red = 179; wave.green = 185; wave.blue = 156;
        wave.life = wave.span = 6 + reach * 2;
        cosmetics.ribbons.push_back(wave);
    }
}
