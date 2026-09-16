#include "ice_pillar_render.hpp"
#include "streetlamp_render.hpp"
#include "render.hpp"
#include "rail_render.hpp"
#include "conveyor_render.hpp"
#include "circuit_render.hpp"
#include "candle.hpp"
#include "stove.hpp"
#include "alarm_clock.hpp"

#include <algorithm>
#include <cmath>

void draw_props(SDL_Renderer* renderer, const GameGraphics& graphics, const Stage& stage,
                 ViewCamera camera, float zoom, const LightingCache& lighting,std::uint64_t tick) {
    const int radius_x = static_cast<int>(320.0F / tile_pixels(zoom)) + 2;
    const int radius_y = static_cast<int>(180.0F / tile_pixels(zoom)) + 2;
    const int center_x = static_cast<int>(std::floor(camera.x));
    const int center_y = static_cast<int>(std::floor(camera.y));
    for (int y = std::max(0, center_y - radius_y); y < std::min(stage.height, center_y + radius_y); ++y)
        for (int x = std::max(0, center_x - radius_x); x < std::min(stage.width, center_x + radius_x); ++x) {
            const Cell cell{x, y};
            const Prop& prop = stage.at(cell)->prop;
            if (prop.kind==PropKind::WaterPipe) {
                const auto light=light_at_cell(lighting,cell);const auto rect=tile_rect(cell,camera,zoom);
                SDL_Texture* texture=texture_for(graphics,prop.broken ? Sprite::WaterPipeBroken : Sprite::WaterPipe);
                SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
                SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,prop.variant ? 90 : 0,nullptr,SDL_FLIP_NONE);
                SDL_SetTextureColorModFloat(texture,1,1,1);continue;
            }
            if (prop.kind == PropKind::None || prop.broken) continue;
            if (prop.kind==PropKind::IcePillar) {draw_pillar_shadow(renderer,stage,cell,camera,zoom);continue;}
            if (prop.kind==PropKind::StreetLamp) {draw_streetlamp_shadow(renderer,stage,cell,camera,zoom);continue;}
            if (prop.kind==PropKind::PoleWreck) {draw_pole_wreck(renderer,prop,tile_rect(cell,camera,zoom),light_at_cell(lighting,cell));continue;}
            const PropSpec spec = prop_spec(prop.kind);
            const LightColor light = light_at_cell(lighting, cell);
            const Sprite sprite = prop.kind==PropKind::Grate ? (prop.variant==1 ? Sprite::GrateV : Sprite::GrateH) : prop.kind==PropKind::SnowWindbreak ? (prop.variant==1 ? Sprite::SnowWallV : Sprite::SnowWallH) : prop.kind == PropKind::BridgePlank ? ((prop.variant&1U) ? Sprite::BridgePlankV : Sprite::BridgePlankH) : prop.kind == PropKind::GroundingSpike && prop.variant>0 ?
                (prop.variant==1 ? Sprite::SpikeHot : Sprite::SpikeSpent) : prop.kind == PropKind::SpiderStrand ?
                (prop.variant == 0 ? Sprite::SpiderStrand : Sprite::SpiderStrandV) : stove_lit(prop) ? Sprite::StoveLit : candle_lit(prop) ? Sprite::CandleLit : prop.kind == PropKind::AlarmClock ? alarm_clock_sprite(prop) : prop.kind == PropKind::Shoot && prop.growth_ticks <= 90 ?
                Sprite::ShootTall : prop.kind == PropKind::IceBlock && prop.growth_ticks <= 120 ?
                Sprite::IceBlockThaw : spec.sprite;
            SDL_Texture* texture = texture_for(graphics, sprite);
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            if (prop.kind==PropKind::FoamCover && prop.growth_ticks<120) {
                const float size=.35F+.65F*static_cast<float>(prop.growth_ticks)/120;
                rect.x+=rect.w*(1-size)*.5F;rect.y+=rect.h*(1-size);rect.w*=size;rect.h*=size;
            }
            if (prop.kind == PropKind::Doorstop) {
                rect.x += rect.w*.35F; rect.y += rect.h*.4F; rect.w *= .55F; rect.h *= .55F;
            }
            if (prop.kind==PropKind::RailPoints) {draw_rail_points(renderer,graphics,prop,rect,light);continue;}
            if (prop.kind==PropKind::Conveyor) { draw_conveyor(renderer,graphics,stage,cell,rect,light,tick); continue; }
            if (prop.kind == PropKind::CopperWire) draw_wire_connections(renderer,stage,cell,rect,light);
            SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
            SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, prop.kind==PropKind::TensionSpring ? static_cast<double>(prop.variant&3U)*90 : (prop.kind==PropKind::Barricade || prop.kind==PropKind::IceRubble) && (prop.variant&1U) ? 90 : 0, nullptr,
                (prop.kind == PropKind::PayCage || prop.kind == PropKind::TensionSpring || prop.kind == PropKind::Grate || prop.kind==PropKind::Barricade || prop.kind == PropKind::SnowWindbreak || prop.kind == PropKind::BridgePlank || prop.kind == PropKind::Doorstop || prop.kind == PropKind::GroundingSpike || prop.kind == PropKind::SpiderStrand || prop.kind == PropKind::Candle || prop.kind == PropKind::Stove || prop.variant % 2 == 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
            SDL_SetTextureColorModFloat(texture, 1, 1, 1);
            if (prop.covered) {
                SDL_Texture* cloth = texture_for(graphics, Sprite::FeltCover);
                SDL_SetTextureColorModFloat(cloth, light.red, light.green, light.blue);
                SDL_RenderTexture(renderer, cloth, nullptr, &rect);
                SDL_SetTextureColorModFloat(cloth, 1, 1, 1);
            }
            if (prop.hp < prop_max_health(prop)) {
                SDL_FRect bar{rect.x + rect.w * .2F, rect.y + rect.h * .87F,
                    rect.w * .6F * static_cast<float>(prop.hp) / static_cast<float>(prop_max_health(prop)), 1};
                SDL_SetRenderDrawColorFloat(renderer, light.red * .8F,
                                           light.green * .6F, light.blue * .3F, 1);
                SDL_RenderFillRect(renderer, &bar);
            }
        }
}
