#include "leech_render.hpp"
#include "steam_leech.hpp"

#include <cmath>

void apply_leech_pose(const Entity& leech, std::uint64_t tick, SDL_FRect& rect, double& angle) {
    if (leech.kind != EntityKind::SteamLeech) return;
    angle = std::atan2(static_cast<double>(leech.facing.y), static_cast<double>(leech.facing.x)) * 180.0 / 3.14159265;
    if (leech.label_a != LeechSwell && leech.label_a != LeechFeed) return;
    // BREATH: Expansion stays centered on the occupied cell; there is no delayed body position.
    const float pulse = 1 + .035F * std::sin(static_cast<float>(tick % 120) *
        (leech.label_a == LeechSwell ? .9F : .25F));
    rect.x -= rect.w * (pulse - 1) * .5F;
    rect.y -= rect.h * (pulse - 1) * .5F;
    rect.w *= pulse; rect.h *= pulse;
}

void draw_leech_tether(tr::Renderer* renderer, const Game& game, const Entity& leech,
    ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!leech_attached(game, leech)) return;
    Cell source;
    if (!leech_source_cell(game, leech, source)) return;
    const SDL_FRect body = tile_rect(leech.cell, camera, zoom);
    const SDL_FRect heat = tile_rect(source, camera, zoom);
    const LightColor light = lit_sprite_color(lighting, leech.cell, light_color(leech.self_light));
    tr::set_color(renderer, light.red * .8F, light.green * .5F, light.blue * .25F, 1);
    const SDL_FPoint start{body.x + body.w * (.5F + static_cast<float>(leech.facing.x) * .3F),
        body.y + body.h * (.5F + static_cast<float>(leech.facing.y) * .3F)};
    tr::line(renderer, start.x, start.y, heat.x + heat.w * .5F, heat.y + heat.h * .5F);
}
