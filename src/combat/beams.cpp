#include "beams.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

bool optical_prop(const Prop& prop) {
    return !prop.broken && (prop.kind == PropKind::MirrorShard || prop.kind == PropKind::CrystalLens);
}

BeamTrace trace_beam(const Game& game, Cell source, Cell direction, int damage, int reach, bool piercing) {
    BeamTrace trace;
    if (std::abs(direction.x) + std::abs(direction.y) != 1 || damage <= 0 || reach <= 0) return trace;
    reach = std::clamp(reach, 1, 32);
    struct Ray { Cell cell, direction; int damage, remaining; };
    std::array<Ray, 257> rays{};
    rays[0] = {source, direction, damage, reach};
    int pending = 1;
    const int budget = std::min(128, reach * 4);
    // QUEUE: Branches advance in alternating steps, sharing one finite travel budget.
    for (int next = 0; next < pending && trace.count < budget; ++next) {
        const Ray ray = rays[static_cast<std::size_t>(next)];
        const Cell cell = ray.cell + ray.direction;
        const Tile* tile = game.stage.at(cell);
        if (!tile) continue;
        BeamCell hit{ray.cell, cell, ray.damage};
        hit.stop = projectile_blocked(game, cell);
        hit.optic = optical_prop(tile->prop);
        const int slot = entity_at(game, cell, true);
        if (slot >= 0) {
            const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
            hit.target = {slot, actor.generation};
            if (!piercing || actor.hard_blocker) hit.stop = true;
        }
        trace.cells[static_cast<std::size_t>(trace.count++)] = hit;
        if (hit.stop) continue;
        if (hit.optic && tile->prop.kind == PropKind::MirrorShard) {
            const Cell turned = tile->prop.variant % 2 == 0 ?
                Cell{-ray.direction.y, -ray.direction.x} : Cell{ray.direction.y, ray.direction.x};
            rays[static_cast<std::size_t>(pending++)] = {cell, turned, ray.damage, reach};
        } else if (hit.optic) {
            // SPLITTER: Odd damage is lost, never rounded into extra energy.
            if (ray.damage < 2) continue;
            const Cell left{-ray.direction.y, ray.direction.x}, right{ray.direction.y, -ray.direction.x};
            rays[static_cast<std::size_t>(pending++)] = {cell, left, ray.damage / 2, reach};
            rays[static_cast<std::size_t>(pending++)] = {cell, right, ray.damage / 2, reach};
        } else if (ray.remaining > 1)
            rays[static_cast<std::size_t>(pending++)] = {cell, ray.direction, ray.damage, ray.remaining - 1};
    }
    return trace;
}

void resolve_beam(Game& game, const BeamTrace& trace) {
    // SNAPSHOT: Broken cover and death drops cannot change this shot's already traced route.
    for (int i = 0; i < trace.count; ++i) {
        const BeamCell& hit = trace.cells[static_cast<std::size_t>(i)];
        if (game.shot_count < static_cast<int>(game.shots.size()))
            game.shots[static_cast<std::size_t>(game.shot_count++)] =
                {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
        // LOOPS: An actor or prop takes only the strongest contact, once per pulse.
        bool strongest = true;
        for (int j = 0; j < trace.count; ++j) {
            const BeamCell& other = trace.cells[static_cast<std::size_t>(j)];
            if (other.cell == hit.cell && (other.damage > hit.damage || (other.damage == hit.damage && j < i))) {
                strongest = false; break;
            }
        }
        if (!strongest) continue;
        if (!hit.optic) hit_prop(game, hit.cell, hit.damage, hit.from);
        if (get_entity(game, hit.target))
            damage_entity(game, hit.target.slot, hit.damage, hit.from);
    }
}
