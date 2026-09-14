#include "field.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace {

constexpr int halo = 12;
constexpr std::array<Cell, 4> neighbors{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

bool solid(const Stage& stage, Cell cell) {
    return stage.at_or_border(cell).kind == TileKind::Wall;
}

float ambient_seed(const Stage& stage, Cell cell) {
    const bool wall = solid(stage, cell);
    float open = 0.0F;
    for (Cell direction : neighbors)
        if (!solid(stage, cell + direction)) open += 1.0F;
    constexpr std::array<Cell, 4> diagonals{{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
    for (Cell direction : diagonals)
        if (!solid(stage, cell + direction)) open += 0.5F;
    return (wall ? 0.06F : 0.10F) + 0.08F * open / 6.0F;
}

void cast_source(LightingCache& cache, const Stage& stage, LightSource source) {
    if (!cache.contains(source.cell) || source.radius <= 0 || source.power <= 0.0F) return;
    struct Wave { Cell cell; float power; int distance; };
    std::vector<float> best(cache.cast.size(), -1.0F);
    std::vector<Wave> wave;
    wave.push_back({source.cell, source.power, 0});
    best[cache.index(source.cell)] = source.power;
    for (std::size_t cursor = 0; cursor < wave.size(); ++cursor) {
        const Wave current = wave[cursor];
        LightColor& target = cache.cast[cache.index(current.cell)];
        target.red = std::max(target.red, source.color.red * current.power);
        target.green = std::max(target.green, source.color.green * current.power);
        target.blue = std::max(target.blue, source.color.blue * current.power);
        if (current.distance >= source.radius) continue;
        for (Cell direction : neighbors) {
            const Cell next = current.cell + direction;
            if (!cache.contains(next)) continue;
            const float decay = solid(stage, next) ? 0.24F :
                stage.at_or_border(next).kind == TileKind::Water ? 0.17F : 0.13F;
            const float power = current.power - decay;
            if (power <= 0.02F) continue;
            float& prior = best[cache.index(next)];
            if (power <= prior + 0.001F) continue;
            prior = power;
            wave.push_back({next, power, current.distance + 1});
        }
    }
}

LightColor display_color(float ambient, LightColor cast) {
    return {
        0.10F + 0.90F * std::clamp(ambient + cast.red, 0.0F, 1.0F),
        0.10F + 0.90F * std::clamp(ambient + cast.green, 0.0F, 1.0F),
        0.10F + 0.90F * std::clamp(ambient + cast.blue, 0.0F, 1.0F),
    };
}

} // namespace

bool LightingCache::contains(Cell cell) const {
    return cell.x >= origin.x && cell.y >= origin.y &&
        cell.x < origin.x + width && cell.y < origin.y + height;
}

std::size_t LightingCache::index(Cell cell) const {
    return static_cast<std::size_t>((cell.y - origin.y) * width + cell.x - origin.x);
}

void build_lighting(LightingCache& cache, const Game& game,
                    ViewCamera camera, float zoom, std::span<const LightFlash> flashes) {
    cache.active = game.run.phase != RunPhase::Arena;
    if (!cache.active) return;
    const float pixels = tile_pixels(std::max(zoom, 0.5F));
    const int left = static_cast<int>(std::floor(camera.x - view_center_x / pixels));
    const int right = static_cast<int>(std::ceil(camera.x + view_center_x / pixels));
    const int top = static_cast<int>(std::floor(camera.y - view_center_y / pixels));
    const int bottom = static_cast<int>(std::ceil(camera.y + view_center_y / pixels));
    cache.origin = {left - halo, top - halo};
    cache.width = right - left + halo * 2 + 1;
    cache.height = bottom - top + halo * 2 + 1;
    const std::size_t count = static_cast<std::size_t>(cache.width * cache.height);
    cache.ambient.resize(count);
    cache.cast.assign(count, {});

    // AMBIENT: Nearby open tiles lift the minimum light, even across corners.
    for (int y = cache.origin.y; y < cache.origin.y + cache.height; ++y)
        for (int x = cache.origin.x; x < cache.origin.x + cache.width; ++x) {
            const Cell cell{x, y};
            const std::size_t index = cache.index(cell);
            cache.ambient[index] = ambient_seed(game.stage, cell);
            if (game.stage.at_or_border(cell).kind == TileKind::Lava)
                cache.cast[index] = {0.82F, 0.35F, 0.07F};
        }

    // SOURCES: Cosmetic flashes share this local cache; rules never read it.
    for (LightSource source : collect_light_sources(game, cache, flashes))
        cast_source(cache, game.stage, source);
}

LightColor light_at_cell(const LightingCache& cache, Cell cell) {
    if (!cache.active || !cache.contains(cell)) return {1.0F, 1.0F, 1.0F};
    const std::size_t index = cache.index(cell);
    return display_color(cache.ambient[index], cache.cast[index]);
}

LightColor light_at_corner(const LightingCache& cache, Cell corner) {
    const std::array<Cell, 4> around{{corner, corner + Cell{-1, 0},
                                     corner + Cell{0, -1}, corner + Cell{-1, -1}}};
    LightColor result{};
    for (Cell cell : around) {
        const LightColor color = light_at_cell(cache, cell);
        result.red += color.red * 0.25F;
        result.green += color.green * 0.25F;
        result.blue += color.blue * 0.25F;
    }
    return result;
}
