#include "field.hpp"
#include "../items/fire.hpp"
#include "../entities/steam_leech.hpp"
#include "../props/growth.hpp"

#include <algorithm>

namespace {

void add(std::vector<LightSource>& sources, const LightingCache& cache,
         Cell cell, int radius, float power, LightColor color) {
    if (cache.contains(cell)) sources.push_back({cell, radius, power, color});
}

void add_emitter(std::vector<LightSource>& sources, const LightingCache& cache,
                 Cell cell, LightEmitter emitter, float scale = 1.0F) {
    if (emitter.radius <= 0 || emitter.strength <= 0) return;
    add(sources, cache, cell, emitter.radius,
        static_cast<float>(emitter.strength) * 0.001F * scale,
        light_color(emitter.color));
}

} // namespace

std::vector<LightSource> collect_light_sources(const Game& game,
    const LightingCache& cache, std::span<const LightFlash> flashes) {
    std::vector<LightSource> sources;
    sources.reserve(48);
    const auto drains = leech_drain_cells(game);
    const auto heat_scale = [&drains](Cell cell) {
        return std::find(drains.begin(), drains.end(), cell) == drains.end() ? 1.0F : .25F;
    };

    // FIXTURES: Sunlight projects through canopy stencils separately from colored point lights.
    for (int y = cache.origin.y; y < cache.origin.y + cache.height; ++y)
        for (int x = cache.origin.x; x < cache.origin.x + cache.width; ++x)
            if ((x + y) % 3 == 0 &&
                game.stage.at_or_border({x, y}).kind == TileKind::Lava)
                add(sources, cache, {x, y}, 4, 0.75F * heat_scale({x, y}), {1.0F, 0.36F, 0.08F});

    for (int y = cache.origin.y; y < cache.origin.y + cache.height; ++y)
        for (int x = cache.origin.x; x < cache.origin.x + cache.width; ++x) {
            const Tile& tile = game.stage.at_or_border({x, y});
            add_emitter(sources, cache, {x, y}, prop_light(tile.prop), heat_scale({x, y}));
            if (tile.surface.fire_ticks == 0) continue;
            // CLUSTERS: Share neighboring emitters, but isolated flames always cast light.
            bool covered = false;
            if ((x + y) % 2 != 0)
                for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
                    covered |= game.stage.at_or_border(Cell{x, y} + side).surface.fire_ticks > 0;
            if (!covered) add(sources, cache, {x, y}, 3, .65F * heat_scale({x, y}), {1, .42F, .14F});
        }

    // ACTORS: Each entity owns its emitter; a dead actor no longer illuminates.
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None || !cache.contains(entity.cell)) continue;
        if (entity.max_health > 0 && entity.health <= 0) continue;
        const float source_scale = entity.kind == EntityKind::Campfire ?
            1.0F - 0.8F * static_cast<float>(entity.fire_dim_ticks) / 60.0F : 1.0F;
        add_emitter(sources, cache, entity.cell, entity.light, source_scale * heat_scale(entity.cell));
        if (entity.scorch_ticks > 0 || entity.burn_ticks > 0)
            add(sources, cache, entity.cell, 3, 0.55F * heat_scale(entity.cell), {1.0F, 0.36F, 0.09F});
        if (entity.kind == EntityKind::GroundItem)
            add_emitter(sources, cache, entity.cell, item_light(entity.ground_item), heat_scale(entity.cell));
        if (entity.kind != EntityKind::GroundItem &&
            entity.inventory.selected >= 0 && entity.inventory.selected < quick_slots)
            add_emitter(sources, cache, entity.cell,
                item_light(*entity.inventory.held()),
                0.75F * heat_scale(entity.cell));
        if (entity.use_flash > 0)
            add(sources, cache, entity.cell, 4,
                0.58F * static_cast<float>(entity.use_flash) / 8.0F,
                {1.0F, 0.76F, 0.45F});
    }

    // EFFECTS: These local flashes fade without entering gameplay hashes.
    for (const LightFlash& flash : flashes)
        if (flash.life > 0 && flash.span > 0)
            add(sources, cache, flash.source.cell, flash.source.radius,
                flash.source.power * static_cast<float>(flash.life) /
                    static_cast<float>(flash.span), flash.source.color);
    return sources;
}
