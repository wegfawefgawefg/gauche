#include "field.hpp"

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

    // FIXTURES: These positions live in the deterministic run snapshot.
    for (int index = 0; index < std::min(game.run.roof_light_count,
                                        static_cast<int>(game.run.roof_lights.size())); ++index) {
        const StageLight& light = game.run.roof_lights[static_cast<std::size_t>(index)];
        add_emitter(sources, cache, light.cell, light.light);
    }
    for (int y = cache.origin.y; y < cache.origin.y + cache.height; ++y)
        for (int x = cache.origin.x; x < cache.origin.x + cache.width; ++x)
            if ((x + y) % 3 == 0 &&
                game.stage.at_or_border({x, y}).kind == TileKind::Lava)
                add(sources, cache, {x, y}, 4, 0.75F, {1.0F, 0.36F, 0.08F});

    // ACTORS: Each entity owns its emitter; a dead actor no longer illuminates.
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None || !cache.contains(entity.cell)) continue;
        if (entity.max_health > 0 && entity.health <= 0) continue;
        add_emitter(sources, cache, entity.cell, entity.light);
        if (entity.kind == EntityKind::GroundItem)
            add_emitter(sources, cache, entity.cell, entity.ground_item.light);
        if (entity.kind != EntityKind::GroundItem &&
            entity.inventory.selected >= 0 && entity.inventory.selected < quick_slots)
            add_emitter(sources, cache, entity.cell,
                entity.inventory.slots[static_cast<std::size_t>(entity.inventory.selected)].light,
                0.75F);
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
