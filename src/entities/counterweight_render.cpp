#include "counterweight_render.hpp"
#include "counterweight.hpp"
#include "../lighting/render.hpp"
#include <algorithm>
#include <cmath>

void draw_counterweights(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                         ViewCamera camera,float zoom,const LightingCache& lighting,bool shadows,const Entity* only) {
    const float pixels=tile_pixels(zoom);
    for (const Entity& actor:game.entities) {
        if (only && &actor!=only) continue;
        if (actor.kind!=EntityKind::Counterweight || actor.health<=0) continue;
        const SDL_FRect base=tile_rect(actor.cell,camera,zoom),mark=tile_rect(actor.point_b,camera,zoom);
        float travel=0,height=1;
        if (actor.label_a==WeightTravel) travel=1-static_cast<float>(actor.timer_a)/30;
        if (actor.label_a==WeightWarn || actor.label_a==WeightDrop) travel=1;
        if (actor.label_a==WeightDrop) {
            const float falling=1-static_cast<float>(actor.timer_a)/8;
            height=1-falling*falling;
        }
        if (actor.label_a==WeightReel) {
            const float lift=std::clamp(static_cast<float>(120-actor.timer_a)/90,0.0F,1.0F);
            const float start=static_cast<float>(actor.counter_a)/48;
            height=start+(1-start)*lift;
            travel=static_cast<float>(actor.counter_b)/30*std::min(1.0F,static_cast<float>(actor.timer_a)/30);
        }
        travel=std::clamp(travel,0.0F,1.0F);height=std::clamp(height,0.0F,1.0F);
        const SDL_FPoint ground{base.x+(mark.x-base.x)*travel+pixels*.5F,base.y+(mark.y-base.y)*travel+pixels*.5F};
        if (ground.x< -pixels*2 || ground.x>640+pixels*2 || ground.y< -pixels || ground.y>360+pixels*3) continue;
        const Cell light_cell=travel>.5F ? actor.point_b : actor.cell;
        const auto light=lit_sprite_color(lighting,light_cell);
        if (shadows) {
            const float size=pixels*(.55F+.25F*(1-height));
            SDL_FRect shade{ground.x-size*.5F,ground.y-size*.32F,size,size*.64F};
            SDL_SetRenderDrawColor(renderer,0,0,0,175);SDL_RenderFillRect(renderer,&shade);
            // Always-visible diegetic warning, independent of debug attack overlays.
            if (actor.label_a==WeightWarn || actor.label_a==WeightDrop) {
                SDL_FRect rim{mark.x+pixels*.12F,mark.y+pixels*.12F,pixels*.76F,pixels*.76F};
                SDL_SetRenderDrawColorFloat(renderer,light.red*.85F,light.green*.57F,light.blue*.25F,.95F);
                SDL_RenderRect(renderer,&rim);
            }
            continue;
        }
        const float rise=pixels*(.25F+height*1.55F);
        const float shake=actor.label_a==WeightWarn ? static_cast<float>((actor.timer_a/3)%3-1)*pixels*.035F : 0;
        const SDL_FPoint top{ground.x+shake,ground.y-rise};
        SDL_SetRenderDrawColorFloat(renderer,light.red*.65F,light.green*.60F,light.blue*.49F,1);
        SDL_RenderLine(renderer,base.x+pixels*.5F,base.y,top.x,top.y-pixels*.8F);
        SDL_RenderLine(renderer,top.x,top.y-pixels*.8F,top.x,top.y-pixels*.3F);
        SDL_FRect block{top.x-pixels*.45F,top.y-pixels*.4F,pixels*.9F,pixels*.9F};
        draw_lit_tile(renderer,texture_for(graphics,Sprite::WeightBlock),block,light_cell,lighting);
    }
}
