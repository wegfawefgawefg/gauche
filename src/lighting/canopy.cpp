#include "canopy.hpp"
#include "canopy_mask.hpp"

#include <algorithm>
#include <cmath>

namespace {

float sample(float x, float y) {
    if (x < 0 || y < 0 || x >= 15 || y >= 15) return 0;
    const int ix = static_cast<int>(std::floor(x)), iy = static_cast<int>(std::floor(y));
    const float fx = x - static_cast<float>(ix), fy = y - static_cast<float>(iy);
    const auto pixel = [](int px, int py) {
        return static_cast<float>(canopy_mask[static_cast<std::size_t>(py * 16 + px)]) / 255.0F;
    };
    return std::lerp(std::lerp(pixel(ix, iy), pixel(ix + 1, iy), fx),
                     std::lerp(pixel(ix, iy + 1), pixel(ix + 1, iy + 1), fx), fy);
}

} // namespace

float canopy_strength(const StageLight& light, float x, float y, std::uint64_t tick) {
    const float time = static_cast<float>(tick % 1000000ULL) / 60.0F;
    const float phase = static_cast<float>((light.cell.x * 17 + light.cell.y * 31) % 61);
    const float sway_x = std::sin(time * .19F + phase) * .18F;
    const float sway_y = std::sin(time * .13F + phase) * .12F;
    const float radius = static_cast<float>(std::max(1, light.light.radius));
    float u = (x - static_cast<float>(light.cell.x) - .5F + sway_x) / radius * 7.5F + 7.5F;
    const float v = (y - static_cast<float>(light.cell.y) - .5F + sway_y) / radius * 7.5F + 7.5F;
    if ((light.cell.x + light.cell.y) % 2 == 0) u = 15 - u;
    // CLOUDS: Broad shadows drift slowly across sunlight, without dimming local lamps or fire.
    const float cloud = std::sin(x * .11F + y * .07F - time * .018F);
    const float shadow = 1.0F - .42F * std::clamp((cloud - .2F) / .8F, 0.0F, 1.0F);
    return sample(u, v) * shadow;
}

void project_canopy(LightingCache& cache, const Game& game) {
    if (game.run.phase!=RunPhase::Arena && !forest_floor(game.run.floor)) return;
    for (int i = 0; i < game.run.roof_light_count; ++i) {
        const StageLight& source = game.run.roof_lights[static_cast<std::size_t>(i)];
        if (!cache.contains(source.cell)) continue;
        const int radius = source.light.radius;
        const LightColor tint = light_color(source.light.color);
        for (int y = source.cell.y - radius; y <= source.cell.y + radius; ++y)
            for (int x = source.cell.x - radius; x <= source.cell.x + radius; ++x) {
                const Cell cell{x, y};
                if (!cache.contains(cell)) continue;
                const float power = canopy_strength(source, static_cast<float>(x) + .5F,
                    static_cast<float>(y) + .5F, game.tick) *
                    static_cast<float>(source.light.strength) * .001F;
                LightColor& cast = cache.cast[cache.index(cell)];
                cast.red = std::max(cast.red, tint.red * power);
                cast.green = std::max(cast.green, tint.green * power);
                cast.blue = std::max(cast.blue, tint.blue * power);
            }
    }
}
