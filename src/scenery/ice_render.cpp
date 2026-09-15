#include "ice_render.hpp"
#include "../lighting/render.hpp"
#include "../particles/water.hpp"
#include "../props/stove.hpp"
#include <cmath>

void draw_ice_scenery(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Cosmetics& cosmetics, ViewCamera camera, float zoom,
    const LightingCache& lighting) {
    prepare_ice_scenery(cosmetics.ice_scenery,game);
    constexpr Sprite sprites[]{Sprite::IceWindow,Sprite::IceChain,Sprite::IceHatch,
        Sprite::IcePipe,Sprite::IceWheel,Sprite::IceMotor,Sprite::IceFlag,Sprite::IceDrip,Sprite::IceChimney};
    for (const auto& site : cosmetics.ice_scenery.sites) {
        if (!ice_scene_alive(site,game)) continue;
        auto rect=tile_rect(site.cell,camera,zoom);
        if (rect.x+rect.w<0 || rect.y+rect.h<0 || rect.x>640 || rect.y>360) continue;
        double angle=0;
        if (site.kind==IceSceneKind::Motor || site.kind==IceSceneKind::Chimney) {
            // APPARATUS: Small rear fittings must leave the usable prop readable.
            rect.x+=rect.w*.28F; rect.y-=rect.h*.35F; rect.w*=.7F; rect.h*=.7F;
        }
        if (site.kind==IceSceneKind::Wheel || site.kind==IceSceneKind::Motor)
            angle=static_cast<double>(game.tick%1440)/4;
        if (site.kind==IceSceneKind::Flag)
            angle=std::sin(static_cast<double>(game.tick)/55)*3;
        auto* texture=texture_for(graphics,sprites[static_cast<int>(site.kind)]);
        const auto light=lit_sprite_color(lighting,site.cell);
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
        SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,angle,nullptr,SDL_FLIP_NONE);
        SDL_SetTextureColorModFloat(texture,1,1,1);
    }
}

void observe_ice_scenery(Cosmetics& cosmetics, const Game& game, Cell focus) {
    prepare_ice_scenery(cosmetics.ice_scenery,game);
    if (game.tick%36!=0) return;
    for (const auto& site : cosmetics.ice_scenery.sites) {
        if (distance(site.cell,focus)>22 || !ice_scene_alive(site,game)) continue;
        if (site.kind==IceSceneKind::Drip || site.kind==IceSceneKind::Wheel)
            spawn_water_rings(cosmetics,site.cell,false);
        bool smoke=site.kind==IceSceneKind::Chimney && stove_lit(game.stage.at_or_border(site.cell).prop);
        if ((!smoke && site.kind!=IceSceneKind::Pipe) || cosmetics.sprites.size()>=1024) continue;
        SpriteParticle puff;
        puff.sprite=Sprite::SteamPuff; puff.layer=ParticleLayer::Foreground;
        puff.motion=ParticleMotion::Drift; puff.x=static_cast<float>(site.cell.x)+.6F;
        puff.y=static_cast<float>(site.cell.y)+.1F; puff.vx=.002F; puff.vy=-.006F;
        puff.width=puff.height=.3F; puff.alpha=.22F; puff.life=puff.span=72;
        cosmetics.sprites.push_back(puff);
    }
}
