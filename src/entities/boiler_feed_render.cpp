#include "boiler_feed_render.hpp"
#include "boiler_feed.hpp"
#include <cstdio>

void draw_boiler_feeds(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    ViewCamera camera,float zoom,const LightingCache& lighting) {
    for (const auto& feed:game.boiler_feeds) {
        const Entity* tank=get_entity(game,feed.tank);if (!tank || tank->health<=0) continue;
        SDL_FRect intake=tile_rect(feed.source,camera,zoom);
        if (intake.x<-96 || intake.x>736 || intake.y<-96 || intake.y>456) continue;
        const LightColor light=light_at_cell(lighting,feed.source);
        SDL_Texture* texture=texture_for(graphics,Sprite::SpringIntake);
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
        SDL_RenderTexture(renderer,texture,nullptr,&intake);SDL_SetTextureColorModFloat(texture,1,1,1);
        const Cell direction{(feed.mount.x-feed.source.x)/3,(feed.mount.y-feed.source.y)/3};
        bool flowing=game.stage.at_or_border(feed.source).kind==TileKind::Spring;
        for (int i=1;i<=2;++i) {
            const Cell cell=feed.source+Cell{direction.x*i,direction.y*i};
            const Tile& tile=game.stage.at_or_border(cell);const Prop& pipe=tile.prop;
            SDL_FRect rect=tile_rect(cell,camera,zoom);const float pixel=rect.w/16;
            const auto color=light_at_cell(lighting,cell);
            const bool broken=!walkable(tile.kind) || pipe.kind!=PropKind::WaterPipe || pipe.broken || pipe.hp==0;
            const float phase=static_cast<float>(game.tick%30)/30;
            if (flowing && (broken || pipe.hp<24)) {
                SDL_SetRenderDrawColorFloat(renderer,color.red*.45F,color.green*.74F,color.blue*.84F,1);
                for (int drop=0;drop<3;++drop) {
                    const float spread=static_cast<float>(drop-1)*pixel*(1+phase*3);
                    SDL_FRect dot{rect.x+rect.w*.5F+spread,rect.y+rect.h*(.35F+phase*.4F),pixel,pixel};
                    SDL_RenderFillRect(renderer,&dot);
                }
            }
            flowing=flowing&&!broken;
            if (flowing) {
                SDL_SetRenderDrawColorFloat(renderer,color.red*.4F,color.green*.66F,color.blue*.7F,1);
                SDL_FRect dot{rect.x+rect.w*(.5F+static_cast<float>(direction.x)*(phase-.5F)),
                    rect.y+rect.h*(.5F+static_cast<float>(direction.y)*(phase-.5F)),pixel*2,pixel};
                SDL_RenderFillRect(renderer,&dot);
            }
        }
    }
}
void draw_boiler_water(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,const Entity& tank,SDL_FRect rect,LightColor light) {
    const auto* feed=boiler_feed(game,tank);if (!feed || tank.health<=0) return;
    const float pixel=rect.w/16;
    // Ground flames sit behind tall fixtures; expose this firebox failure over
    // its housing so the exhausted boiler cannot conceal its own fire.
    if (game.stage.at_or_border(tank.cell).surface.fire_ticks>0) {
        SDL_Texture* flame=texture_for(graphics,game.tick/6%2 ? Sprite::FlameA : Sprite::FlameB);
        SDL_FRect fire{rect.x+pixel*3,rect.y-pixel*2,pixel*10,pixel*15};
        SDL_RenderTexture(renderer,flame,nullptr,&fire);
    }
    SDL_FRect gauge{rect.x+rect.w+pixel,rect.y+pixel*3,pixel*2,pixel*9};
    SDL_SetRenderDrawColorFloat(renderer,light.red*.12F,light.green*.16F,light.blue*.15F,1);SDL_RenderFillRect(renderer,&gauge);
    gauge.h*=static_cast<float>(feed->water)/boiler_water_limit;gauge.y=rect.y+pixel*12-gauge.h;
    SDL_SetRenderDrawColorFloat(renderer,light.red*.35F,light.green*.72F,light.blue*.85F,1);SDL_RenderFillRect(renderer,&gauge);
    if (feed->dry_ticks==0) return;
    SDL_SetRenderDrawColorFloat(renderer,light.red,light.green*.4F,light.blue*.12F,1);
    SDL_FRect warn{rect.x+pixel*6,rect.y-pixel*4,pixel*3,pixel*2};
    if (feed->dry_ticks/10%2==0) SDL_RenderFillRect(renderer,&warn);
    char label[32];std::snprintf(label,sizeof(label),"DRY FIRE %.1fs",static_cast<double>(180-feed->dry_ticks)/60);
    float sx=1,sy=1;SDL_GetRenderScale(renderer,&sx,&sy);SDL_SetRenderScale(renderer,sx*.5F,sy*.5F);
    SDL_RenderDebugText(renderer,rect.x*2,(rect.y-pixel*9)*2,label);SDL_SetRenderScale(renderer,sx,sy);
}
