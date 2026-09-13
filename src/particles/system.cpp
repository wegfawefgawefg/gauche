#include "system.hpp"

#include <algorithm>

void step_particles(Cosmetics& cosmetics) {
    for (SpriteParticle& particle : cosmetics.sprites) {
        if (particle.motion == ParticleMotion::Accelerate) {
            particle.vx += particle.ax;
            particle.vy += particle.ay;
        }
        if (particle.motion != ParticleMotion::Still) {
            particle.x += particle.vx;
            particle.y += particle.vy;
        }
        particle.angle += particle.spin;
        --particle.life;
    }
    for (RibbonParticle& ribbon : cosmetics.ribbons) --ribbon.life;
    for (RingParticle& ring : cosmetics.rings) {
        ring.radius += ring.speed;
        --ring.life;
    }
    for (TileShake& shake : cosmetics.tile_shakes) {
        shake.strength = std::max(0.0F, shake.strength - 0.01F);
        --shake.life;
    }
    std::erase_if(cosmetics.sprites, [](const SpriteParticle& p) { return p.life <= 0; });
    std::erase_if(cosmetics.ribbons, [](const RibbonParticle& p) { return p.life <= 0; });
    std::erase_if(cosmetics.rings, [](const RingParticle& p) { return p.life <= 0; });
    std::erase_if(cosmetics.tile_shakes, [](const TileShake& p) { return p.life <= 0; });
}
