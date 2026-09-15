#include "render.hpp"
#include "../items/bolt_pouch.hpp"
#include "../items/emergency_foam.hpp"
#include "fishing.hpp"
#include "../lighting/render.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cmath>

namespace {

void draw_net(SDL_Renderer* renderer, const GameGraphics& graphics, const Entity& shot,
              const Game& game, ViewCamera camera, float zoom, const LightingCache& lighting) {
    const int width = item_pattern(shot.ground_item).half_width;
    const Cell side{-shot.facing.y, shot.facing.x};
    const float travel = projectile_blocked(game, shot.cell + shot.facing) ? 0 :
        std::clamp(1 - static_cast<float>(shot.timer_b) / 4, 0.0F, 1.0F);
    const float pixels = tile_pixels(zoom);
    SDL_Texture* texture = texture_for(graphics, Sprite::NetFlight);
    const double angle = std::atan2(static_cast<double>(shot.facing.y), static_cast<double>(shot.facing.x)) * 180 / 3.141592653589793;
    for (int lane = -width; lane <= width; ++lane) {
        if ((shot.label_b & (1 << (lane + width))) == 0) continue;
        const Cell cell = shot.cell + Cell{side.x * lane, side.y * lane};
        if (projectile_blocked(game, cell)) continue;
        SDL_FRect rect = tile_rect(cell, camera, zoom);
        const float offset = projectile_blocked(game, cell + shot.facing) ? 0 : travel;
        rect.x += static_cast<float>(shot.facing.x) * offset * pixels;
        rect.y += static_cast<float>(shot.facing.y) * offset * pixels;
        const LightColor light = lit_sprite_color(lighting, cell);
        SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
        SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
}

} // namespace

ProjectilePose projectile_pose(const Entity& shot,const Game& game,ViewCamera camera,float zoom) {
    const bool drill=shot.label_a==static_cast<int>(ProjectileKind::Drill);
    const bool harpoon=shot.label_a==static_cast<int>(ProjectileKind::Harpoon);
    const bool hook=harpoon || shot.label_a==static_cast<int>(ProjectileKind::Hook) ||
        shot.label_a==static_cast<int>(ProjectileKind::WidowHook) || shot.label_a==static_cast<int>(ProjectileKind::FishingHook);
    const bool thrown=shot.label_a==static_cast<int>(ProjectileKind::FoamCan) || shot.label_a==static_cast<int>(ProjectileKind::Blink) ||
        shot.label_a==static_cast<int>(ProjectileKind::EchoPebble) || shot.label_a==static_cast<int>(ProjectileKind::PrismBomb) ||
        shot.label_a==static_cast<int>(ProjectileKind::Snowball) || shot.label_a==static_cast<int>(ProjectileKind::IceBrick) ||
        shot.label_a==static_cast<int>(ProjectileKind::Mixture) || shot.label_a==static_cast<int>(ProjectileKind::Bomb) ||
        shot.label_a==static_cast<int>(ProjectileKind::Firecracker) || shot.label_a==static_cast<int>(ProjectileKind::Flask);
    const float travel = shot.counter_a > 0 && (!(hook || drill) || shot.label_b == 0) ?
        std::clamp(1 - static_cast<float>(shot.timer_b) / static_cast<float>(projectile_step_ticks(shot)), 0.0F, 1.0F) : 0;
    const float pixels = tile_pixels(zoom);
    Cell render_cell = shot.cell;
    // TETHER: The victim may have stepped later in entity order. Attach to its real cell.
    if (harpoon && shot.label_b == 1)
        if (const Entity* victim = get_entity(game,shot.entity_b)) render_cell = victim->cell;
    SDL_FRect rect = tile_rect(render_cell, camera, zoom);
    const float offset = drill && shot.label_b == 1 ? .25F : travel;
    Cell motion=shot.facing;
    if (shot.label_a==static_cast<int>(ProjectileKind::ThrownBolt)) motion=thrown_bolt_next(shot)-shot.cell;
    rect.x += static_cast<float>(motion.x) * offset * pixels;
    rect.y += static_cast<float>(motion.y) * offset * pixels;
    ProjectilePose pose{rect,rect,0};
    if (thrown && shot.counter_a > 0) {
        const float progress = (static_cast<float>(shot.attack_interval - shot.counter_a) + travel) /
            static_cast<float>(std::max(1, shot.attack_interval));
        pose.height = .85F * std::sin(progress * 3.14159265F);
        pose.body.y -= pixels * pose.height;
    }
    return pose;
}

void draw_projectile(SDL_Renderer* renderer, const GameGraphics& graphics,
                     const Entity& shot, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting) {
    if (shot.label_a == static_cast<int>(ProjectileKind::Net)) { draw_net(renderer, graphics, shot, game, camera, zoom, lighting); return; }
    const bool foam=shot.label_a==static_cast<int>(ProjectileKind::FoamCan);
    const bool echo = shot.label_a == static_cast<int>(ProjectileKind::EchoPebble);
    const bool spinning = shot.label_a == static_cast<int>(ProjectileKind::Boomerang);
    const bool drill = shot.label_a == static_cast<int>(ProjectileKind::Drill);
    const bool blink = shot.label_a == static_cast<int>(ProjectileKind::Blink);
    const bool fishing = shot.label_a == static_cast<int>(ProjectileKind::FishingHook);
    const bool widow = shot.label_a == static_cast<int>(ProjectileKind::WidowHook);
    const bool harpoon = shot.label_a == static_cast<int>(ProjectileKind::Harpoon);
    const bool hook = harpoon || widow || fishing || shot.label_a == static_cast<int>(ProjectileKind::Hook);
    const bool flare = shot.label_a == static_cast<int>(ProjectileKind::Flare);
    const bool prism = shot.label_a == static_cast<int>(ProjectileKind::PrismBomb);
    const bool thaw = shot.label_a == static_cast<int>(ProjectileKind::ThawCharge) || shot.label_a == static_cast<int>(ProjectileKind::QuarryCharge);
    const bool bomb = shot.label_a == static_cast<int>(ProjectileKind::Bomb);
    const bool cracker = shot.label_a == static_cast<int>(ProjectileKind::Firecracker);
    const bool rocket = shot.label_a == static_cast<int>(ProjectileKind::Rocket);
    const bool mixture = shot.label_a == static_cast<int>(ProjectileKind::Mixture);
    const bool pitch = mixture && shot.ground_item.kind == ItemKind::PitchBomb;
    const bool thrown = foam || blink || echo || prism || shot.label_a == static_cast<int>(ProjectileKind::Snowball) || shot.label_a == static_cast<int>(ProjectileKind::IceBrick) || mixture || bomb || cracker || shot.label_a == static_cast<int>(ProjectileKind::Flask);
    const float pixels=tile_pixels(zoom);
    const ProjectilePose pose=projectile_pose(shot,game,camera,zoom);
    SDL_FRect rect=pose.body;
    Cell render_cell=shot.cell;
    if (harpoon && shot.label_b==1)
        if (const Entity* victim=get_entity(game,shot.entity_b)) render_cell=victim->cell;
    rect.x += pixels * .17F; rect.y += pixels * .17F;
    rect.w = rect.h = pixels * .66F;
    SDL_Texture* texture = texture_for(graphics, shot.sprite);
    const LightColor light = lit_sprite_color(lighting, render_cell);
    SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
    const double angle = spinning ? static_cast<double>(game.tick % 12) * 30 : blink ? static_cast<double>(game.tick % 18) * 20 : thrown ? (shot.counter_a > 0 ? static_cast<double>(game.tick % 60) * 9 : 0) :
        shot.facing.x > 0 ? 0 : shot.facing.x < 0 ? 180 : shot.facing.y > 0 ? 90 : -90;
    const double wobble = drill ? ((game.tick / 3) % 2 == 0 ? -7.0 : 7.0) : 0;
    if (hook) {
        if (const Entity* owner = get_entity(game, shot.entity_a)) {
            const SDL_FRect hand = tile_rect(owner->cell, camera, zoom);
            SDL_SetRenderDrawColorFloat(renderer, light.red * (fishing ? .70F : .57F),
                light.green * (fishing ? .72F : .47F), light.blue * (fishing ? .67F : .31F), 1);
            SDL_RenderLine(renderer, hand.x + hand.w * .5F, hand.y + hand.h * .5F,
                rect.x + rect.w * .5F, rect.y + rect.h * .5F);
        }
    }
    // CARGO: The actual item is already drawn at this cell; keep the hook off its icon.
    if ((!fishing || shot.label_b != FishingCargo) && (!(widow || harpoon) || shot.label_b == 0))
        SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, angle + wobble, nullptr, SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
    if (foam && shot.label_b==1) {
        SDL_Texture* mound=texture_for(graphics,Sprite::FoamCover);
        const float size=.18F+.65F*(1-static_cast<float>(shot.timer_a)/foam_expand_ticks);
        SDL_FRect cloud=tile_rect(shot.cell,camera,zoom);
        cloud.x+=cloud.w*(1-size)*.5F;cloud.y+=cloud.h*(1-size);cloud.w*=size;cloud.h*=size;
        SDL_SetTextureColorModFloat(mound,light.red,light.green,light.blue);
        SDL_RenderTexture(renderer,mound,nullptr,&cloud);
        SDL_SetTextureColorModFloat(mound,1,1,1);
    }
    if (echo && shot.use_flash>0) {
        const float spread = pixels*(1-static_cast<float>(shot.use_flash)/12);
        const float x = rect.x+rect.w*.5F, y = rect.y+rect.h*.5F;
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,139,198,211,static_cast<Uint8>(shot.use_flash*12));
        const SDL_FPoint wave[]{{x-spread,y},{x,y-spread*.5F},{x+spread,y},{x,y+spread*.5F},{x-spread,y}};
        SDL_RenderLines(renderer,wave,5);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
    }
    if (rocket) {
        SDL_SetRenderDrawColor(renderer, 255, 181, 76, 255);
        const float x = rect.x + rect.w * .5F, y = rect.y + rect.h * .5F;
        SDL_RenderLine(renderer, x - static_cast<float>(shot.facing.x) * pixels * .35F,
            y - static_cast<float>(shot.facing.y) * pixels * .35F,
            x - static_cast<float>(shot.facing.x) * pixels * .65F,
            y - static_cast<float>(shot.facing.y) * pixels * .65F);
    }
    if (thaw || flare || prism || bomb || cracker || pitch) {
        // FUSE: A few local sparks communicate danger without a debug attack grid.
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 3; ++i) {
            const float beat = static_cast<float>((game.tick + static_cast<std::uint64_t>(i * 5)) % 17) / 17;
            SDL_SetRenderDrawColor(renderer, shot.freeze_ticks>0 ? 133 : prism ? 186 : 255, static_cast<Uint8>(shot.freeze_ticks>0 ? 211 : flare ? 65 : prism ? 218 : 180 - i * 25), shot.freeze_ticks>0 ? 235 : flare ? 42 : prism ? 255 : 62,
                static_cast<Uint8>(230 * (1-beat)));
            SDL_RenderPoint(renderer, rect.x + rect.w * .68F + pixels * beat * (i == 1 ? -.18F : .12F),
                rect.y + rect.h * .1F - pixels * beat * .35F);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}
