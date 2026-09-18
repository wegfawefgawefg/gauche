#include "warden_render.hpp"
#include "lens_warden.hpp"
#include "../lighting/render.hpp"

#include <algorithm>

void draw_warden_charge(tr::Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& warden, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!warden_charging(game, warden)) return;
    SDL_FRect lamp = tile_rect(warden.point_a, camera, zoom);
    const LightColor light = lit_sprite_color(lighting, warden.point_a);
    tr::Texture* glow = texture_for(graphics, Sprite::BeamLampCharged);
    tr::texture_color(glow, light.red, light.green, light.blue);
    tr::draw_texture(renderer, glow, nullptr, &lamp);
    tr::texture_color(glow, 1, 1, 1);
    // CHARGE: Three gathering motes signal the source without an always-on attack grid.
    tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 3; ++i) {
        const float phase = static_cast<float>((game.tick + static_cast<std::uint64_t>(i * 7)) % 23) / 23;
        const float spread = .65F * (1-phase);
        tr::set_color_bytes(renderer, 183, 216, 247, static_cast<Uint8>(80+150*phase));
        const float x = lamp.x + lamp.w * (.5F + (i == 1 ? -spread : spread));
        const float y = lamp.y + lamp.h * (.5F + (i == 2 ? spread : -spread));
        tr::point(renderer, x, y);
    }
    tr::set_blend(renderer, SDL_BLENDMODE_NONE);
}
