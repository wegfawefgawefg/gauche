#include "gunfire.hpp"
#include "electricity.hpp"
#include "../projectiles/projectile.hpp"
#include "templates.hpp"

#include <algorithm>

void observe_gunfire(Cosmetics& cosmetics, const Game& game, Cell focus) {
    // SHOTS: Cosmetic events replay with simulation, but fragments emit once locally.
    for (int index = 0; index < game.shot_count; ++index) {
        const auto key = (1ULL << 62) | (game.tick << 8) | (static_cast<std::uint64_t>(index) + 1);
        if (std::find(cosmetics.seen_events.begin(), cosmetics.seen_events.end(), key) != cosmetics.seen_events.end()) continue;
        cosmetics.seen_events[cosmetics.next_event++ % cosmetics.seen_events.size()] = key;
        const ShotEvent& shot = game.shots[static_cast<std::size_t>(index)];
        if (distance(shot.source, focus) > 24 && distance(shot.end, focus) > 24) continue;
        if (shot.electric) {
            if (shot.impact) spawn_electric_arc(cosmetics, shot.source, shot.end, key);
            else spawn_water_arc(cosmetics, shot.source, shot.end, key);
            if (shot.casing) scatter_material(cosmetics.debris, shot.end, DebrisKind::Acorn, 2, key);
            continue;
        }
        RibbonParticle tracer;
        tracer.life = tracer.span = shot.beam ? 9 : 4;
        tracer.count = 2;
        tracer.red = 246; tracer.green = 213; tracer.blue = 145;
        if (shot.beam) { tracer.red = 191; tracer.green = 224; tracer.blue = 242; }
        tracer.points[0] = {static_cast<float>(shot.source.x) + .5F, static_cast<float>(shot.source.y) + .5F};
        tracer.points[1] = {static_cast<float>(shot.end.x) + .5F, static_cast<float>(shot.end.y) + .5F};
        if (cosmetics.ribbons.size() < 256) cosmetics.ribbons.push_back(tracer);
        if (shot.muzzle) {
            cosmetics.flashes.push_back({{shot.source, 3, .85F, shot.beam ? LightColor{.65F, .85F, 1.0F} : LightColor{1, .76F, .38F}}, 4, 4});
            push_debris(cosmetics.debris, shot.source, 1.1F, .035F);
            if (shot.casing) scatter_material(cosmetics.debris, shot.source, DebrisKind::BrassCase, 1, key);
        }
        if (shot.impact) push_debris(cosmetics.debris, shot.end, .8F, .07F);
    }
    // EXHAUST: No damage or collision comes from the rocket's brief smoke trail.
    if (game.tick % 3 == 0)
        for (const Entity& shot : game.entities)
            if (shot.kind == EntityKind::Projectile && shot.label_a == static_cast<int>(ProjectileKind::Rocket) &&
                distance(shot.cell, focus) < 20)
                spawn_campfire_smoke(cosmetics, shot.cell, game.tick ^ shot.generation);
}
