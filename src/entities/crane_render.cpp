#include "crane_render.hpp"
#include "magnet_crane.hpp"
#include "crane_operator.hpp"
#include "../lighting/render.hpp"
#include <algorithm>

void draw_crane_parts(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                      ViewCamera camera,float zoom,const LightingCache& lighting,bool shadows,const Entity* only) {
    const float pixels=tile_pixels(zoom);
    for (const Entity& crane:game.entities) {
        if (only && &crane!=only) continue;
        if (crane.kind!=EntityKind::MagnetCrane || crane.health<=0) continue;
        const SDL_FRect base=tile_rect(crane.cell,camera,zoom),target=tile_rect(crane.point_b,camera,zoom);
        float progress=0;
        if (crane.label_a==CraneTravel) progress=1-static_cast<float>(crane.timer_a)/30;
        if (crane.label_a==CraneLock) progress=1;
        if (crane.label_a==CraneReturn) progress=static_cast<float>(crane.counter_a)/30*static_cast<float>(crane.timer_a)/60;
        progress=std::clamp(progress,0.0F,1.0F);
        const SDL_FPoint ground{base.x+(target.x-base.x)*progress+pixels*.5F,
                               base.y+(target.y-base.y)*progress+pixels*.5F};
        if (ground.x< -pixels || ground.x>640+pixels || ground.y< -pixels || ground.y>360+pixels) continue;
        const bool locked=crane.label_a==CraneLock;
        const float height=pixels*(locked ? .62F+.20F*static_cast<float>(crane.timer_a)/45 : 1.0F);
        if (shadows) {
            const Entity* worker=get_entity(game,crane.entity_b);
            if (crane.label_b==1 && worker && worker->kind==EntityKind::CraneOperator) {
                const SDL_FRect stand=tile_rect(worker->point_b,camera,zoom);
                const auto tint=lit_sprite_color(lighting,worker->point_b);
                SDL_SetRenderDrawColorFloat(renderer,tint.red*.25F,tint.green*.29F,tint.blue*.25F,1);
                SDL_RenderLine(renderer,base.x+pixels*.5F,base.y+pixels*.65F,
                    stand.x+pixels*.8F,stand.y+pixels*.85F);
                SDL_Texture* console=texture_for(graphics,Sprite::CraneConsole);
                SDL_SetTextureColorModFloat(console,tint.red,tint.green,tint.blue);
                SDL_RenderTexture(renderer,console,nullptr,&stand);SDL_SetTextureColorModFloat(console,1,1,1);
                SDL_SetRenderDrawColor(renderer,crane_operator_ready(game,crane) ? 101 : 189,114,56,220);
                SDL_FRect lamp{stand.x+pixels*.875F,stand.y+pixels*.6875F,pixels/16,pixels/16};
                SDL_RenderFillRect(renderer,&lamp);
            }
            SDL_FRect shade{ground.x-pixels*.30F,ground.y-pixels*.12F,pixels*.60F,pixels*.24F};
            SDL_SetRenderDrawColor(renderer,0,0,0,85);SDL_RenderFillRect(renderer,&shade);
            if (locked) {
                SDL_SetRenderDrawColor(renderer,181,125,60,155);
                SDL_FRect mark{target.x+pixels*.1F,target.y+pixels*.1F,pixels*.8F,pixels*.8F};
                SDL_RenderRect(renderer,&mark);
            }
            continue;
        }
        const auto light=lit_sprite_color(lighting,progress>.5F ? crane.point_b : crane.cell);
        const SDL_FPoint pivot{base.x+pixels*.5F,base.y+pixels*.12F};
        // A simple rigid boom and hanging line explain where the overhead head
        // belongs. Only the fixed marked cell can grab after the full tell.
        SDL_SetRenderDrawColorFloat(renderer,light.red*.52F,light.green*.42F,light.blue*.24F,1);
        SDL_RenderLine(renderer,pivot.x,pivot.y,ground.x,ground.y-height-pixels*.35F);
        SDL_RenderLine(renderer,pivot.x,pivot.y+2,ground.x,ground.y-height-pixels*.35F+2);
        SDL_SetRenderDrawColorFloat(renderer,light.red*.7F,light.green*.72F,light.blue*.70F,1);
        SDL_RenderLine(renderer,ground.x,ground.y-height-pixels*.35F,ground.x,ground.y-height);
        SDL_FRect head{ground.x-pixels*.45F,ground.y-height-pixels*.35F,pixels*.9F,pixels*.7F};
        SDL_Texture* texture=texture_for(graphics,Sprite::CraneHead);
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
        SDL_RenderTexture(renderer,texture,nullptr,&head);SDL_SetTextureColorModFloat(texture,1,1,1);
        if (locked) {
            SDL_SetRenderDrawColor(renderer,218,165,72,220);
            SDL_FRect lamp{head.x+head.w*.45F,head.y+head.h*.5F,2,2};SDL_RenderFillRect(renderer,&lamp);
        }
    }
}
