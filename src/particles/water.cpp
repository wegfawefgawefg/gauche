#include "water.hpp"
#include "../world/water.hpp"

void spawn_water_rings(Cosmetics& cosmetics, Cell cell, bool footstep) {
    if (cosmetics.rings.size() >= 160) return;
    RingParticle ring;
    ring.layer = ParticleLayer::Ground;
    ring.x = static_cast<float>(cell.x) + .5F;
    ring.y = static_cast<float>(cell.y) + .65F;
    ring.radius = .08F;
    ring.speed = footstep ? .016F : .011F;
    ring.life = ring.span = footstep ? 32 : 42;
    ring.red = 109; ring.green = 157; ring.blue = 151;
    ring.water = true;
    cosmetics.rings.push_back(ring);
}

void observe_water(Cosmetics& cosmetics, const Game& game, Cell focus) {
    if (game.tick % 24 != 0) return;
    // STREAM: Source ripples are local and bounded to the visible neighborhood.
    for (int y = focus.y - 14; y <= focus.y + 14; ++y)
        for (int x = focus.x - 22; x <= focus.x + 22; ++x) {
            const Tile* tile = game.stage.at({x, y});
            if (tile != nullptr && tile->kind == TileKind::Spring)
                spawn_water_rings(cosmetics, {x, y}, false);
        }
}
