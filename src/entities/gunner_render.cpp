#include "gunner_render.hpp"
#include "rivet_gunner.hpp"
#include "../projectiles/projectile.hpp"
#include <algorithm>

void draw_gunner_aim(SDL_Renderer* renderer,const Game& game,ViewCamera camera,
                     float zoom,const LightingCache& lighting) {
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (const Entity& actor:game.entities) {
        if (actor.kind!=EntityKind::RivetGunner || actor.health<=0 || actor.sleep_ticks>0 ||
            actor.stun_ticks>0 || actor.freeze_ticks>0 || actor.vitals.rooted>0 ||
            actor.cell!=actor.point_a || (actor.label_a!=GunnerBrace && actor.label_a!=GunnerBurst)) continue;
        const auto source=lit_sprite_color(lighting,actor.cell);
        if (std::max({source.red,source.green,source.blue})<.10F) continue;
        const float strength=actor.label_a==GunnerBrace ?
            .45F+.55F*(1-std::clamp(static_cast<float>(actor.timer_a)/30,0.F,1.F)) : 1.F;
        // Small amber sight marks, not filled damage tiles. Cover stops the
        // actual rivets; grates, bodies and newly arrived smoke do not.
        for (int reach=1;reach<=8;++reach) {
            const Cell cell=actor.cell+Cell{actor.point_b.x*reach,actor.point_b.y*reach};
            if (projectile_blocked(game,cell,false,true)) break;
            const auto light=lit_sprite_color(lighting,cell);
            const float level=std::clamp(std::max({light.red,light.green,light.blue}),0.F,1.F);
            const SDL_FRect rect=tile_rect(cell,camera,zoom);
            const float x=rect.x+rect.w*.5F,y=rect.y+rect.h*.5F;
            const float dx=static_cast<float>(actor.point_b.x)*rect.w*.16F;
            const float dy=static_cast<float>(actor.point_b.y)*rect.h*.16F;
            SDL_SetRenderDrawColor(renderer,216,169,90,static_cast<Uint8>(level*strength*180));
            SDL_RenderLine(renderer,x-dx,y-dy,x+dx,y+dy);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
