#include "conveyor_render.hpp"
#include "conveyor.hpp"
void draw_conveyor(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,
                   SDL_FRect rect,LightColor light,std::uint64_t tick) {
    const Stage& stage=game.stage;
    const Prop& prop=stage.at_or_border(cell).prop;
    const bool stopped=prop.growth_ticks>0 || !belt_powered(game,cell);
    const Cell outgoing=belt_direction(prop);
    Cell incoming{};
    for (Cell side:{Cell{1,0},{-1,0},{0,1},{0,-1}}) {
        if (side.x*outgoing.x+side.y*outgoing.y!=0) continue;
        const Cell feed=belt_direction(stage.at_or_border(cell+side).prop);
        if (feed==Cell{-side.x,-side.y}) {incoming=side;break;}
    }
    if (incoming!=Cell{}) {
        const float pixel=rect.w/16;
        const auto color=[&](float r,float g,float b) {SDL_SetRenderDrawColorFloat(renderer,r*light.red,g*light.green,b*light.blue,1);};
        color(.16F,.19F,.2F);SDL_RenderFillRect(renderer,&rect);
        const SDL_FPoint center{rect.x+8*pixel,rect.y+8*pixel};
        // Draw the real elbow rather than a straight sprite with a sudden turn.
        for (Cell direction:{incoming,outgoing}) {
            const float dx=static_cast<float>(direction.x),dy=static_cast<float>(direction.y);
            SDL_FRect lane=direction.x ? SDL_FRect{center.x+(direction.x<0 ? -8*pixel : 0),center.y-5*pixel,8*pixel,10*pixel} :
                SDL_FRect{center.x-5*pixel,center.y+(direction.y<0 ? -8*pixel : 0),10*pixel,8*pixel};
            color(.29F,.33F,.34F);SDL_RenderFillRect(renderer,&lane);
            const int beat=static_cast<int>(tick/10%3);
            const int phase=stopped ? 0 : direction==incoming ? (3-beat)%3 : beat;
            for (int n=phase;n<8;n+=3) {
                const float along=static_cast<float>(n)*pixel;
                color(.43F,.47F,.46F);
                SDL_RenderLine(renderer,center.x+dx*along-dy*4*pixel,center.y+dy*along-dx*4*pixel,
                    center.x+dx*along+dy*4*pixel,center.y+dy*along+dx*4*pixel);
            }
        }
        const float dx=static_cast<float>(outgoing.x),dy=static_cast<float>(outgoing.y);
        color(prop.growth_ticks ? .8F : .65F,prop.growth_ticks ? .23F : .57F,.29F);
        for (int side:{-1,1}) {
            const float offset=static_cast<float>(side)*2*pixel;
            SDL_RenderLine(renderer,center.x+dx*4*pixel,center.y+dy*4*pixel,
                center.x+dx*pixel-dy*offset,center.y+dy*pixel+dx*offset);
        }
        return;
    }
    const Sprite sprite=prop.growth_ticks>0 ? Sprite::ConveyorBrake : stopped ? Sprite::ConveyorOff :
        (tick/10)%2 ? Sprite::ConveyorMoving : Sprite::Conveyor;
    auto* texture=texture_for(graphics,sprite);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,static_cast<double>(prop.variant&3U)*90,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
