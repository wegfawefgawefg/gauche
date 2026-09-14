#include "field.hpp"

#include <algorithm>
#include <cmath>

namespace {

void add(std::vector<LightSource>& sources, const LightingCache& cache,
         Cell cell, int radius, float power, LightColor color) {
    if (cache.contains(cell)) sources.push_back({cell, radius, power, color});
}

void item_source(std::vector<LightSource>& sources, const LightingCache& cache,
                 ItemKind kind, Cell cell, bool carried) {
    const float scale = carried ? 0.75F : 1.0F;
    switch (kind) {
    case ItemKind::Medkit:
        add(sources, cache, cell, 3, 0.42F * scale, {0.28F, 1.0F, 0.36F});
        break;
    case ItemKind::ConductorHat:
        add(sources, cache, cell, 3, 0.46F * scale, {1.0F, 0.72F, 0.27F});
        break;
    case ItemKind::RocketLauncher: case ItemKind::Mine:
        add(sources, cache, cell, 3, 0.35F * scale, {1.0F, 0.37F, 0.20F});
        break;
    default: break;
    }
}

} // namespace

std::vector<LightSource> collect_light_sources(const Game& game,
    const LightingCache& cache, std::span<const LightFlash> flashes) {
    std::vector<LightSource> sources;
    sources.reserve(48);

    // FIXTURES: These positions live in the deterministic run snapshot.
    for (int index = 0; index < std::min(game.run.roof_light_count,
                                        static_cast<int>(game.run.roof_lights.size())); ++index)
        add(sources, cache, game.run.roof_lights[static_cast<std::size_t>(index)],
            7, 1.20F, {0.94F, 0.88F, 0.69F});
    add(sources, cache, game.run.exit, 6, 0.95F,
        game.run.has_key ? LightColor{0.33F, 1.0F, 0.53F} :
                           LightColor{0.24F, 0.73F, 0.91F});
    for (int y = cache.origin.y; y < cache.origin.y + cache.height; ++y)
        for (int x = cache.origin.x; x < cache.origin.x + cache.width; ++x)
            if ((x + y) % 3 == 0 &&
                game.stage.at_or_border({x, y}).kind == TileKind::Lava)
                add(sources, cache, {x, y}, 4, 0.75F, {1.0F, 0.36F, 0.08F});

    // ACTORS: Self-glow is handled separately when the sprite is drawn.
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None || !cache.contains(entity.cell)) continue;
        switch (entity.kind) {
        case EntityKind::Player:
            add(sources, cache, entity.cell, 8, 1.15F, {1.0F, 0.97F, 0.88F});
            break;
        case EntityKind::Campfire:
            add(sources, cache, entity.cell, 8, 1.25F, {1.0F, 0.55F, 0.23F});
            break;
        case EntityKind::Ember:
            add(sources, cache, entity.cell, 5, 0.96F, {1.0F, 0.29F, 0.14F});
            break;
        case EntityKind::FrostBat:
            add(sources, cache, entity.cell, 4, 0.58F, {0.36F, 0.70F, 1.0F});
            break;
        case EntityKind::Key:
            add(sources, cache, entity.cell, 3, 0.56F, {1.0F, 0.83F, 0.28F});
            break;
        case EntityKind::GroundItem:
            item_source(sources, cache, entity.ground_item.kind, entity.cell, false);
            break;
        default: break;
        }
        if (entity.kind != EntityKind::GroundItem &&
            entity.inventory.selected >= 0 && entity.inventory.selected < quick_slots)
            item_source(sources, cache,
                entity.inventory.slots[static_cast<std::size_t>(entity.inventory.selected)].kind,
                entity.cell, true);
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
