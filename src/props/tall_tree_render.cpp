#include "tall_tree_render.hpp"
#include "tall_tree.hpp"
#include "hit_render.hpp"
#include <algorithm>
#include <cmath>

void draw_tall_tree(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting) {
    const auto& tile=*game.stage.at(cell);const Prop& tree=tile.prop;
    const SDL_FRect floor=tile_rect(cell,camera,zoom);
    const Cell dir=tree_direction(tree);
    const float reach=static_cast<float>(tree_height(tree));
    const float progress=tree.growth_ticks ? std::clamp(1-static_cast<float>(tree.growth_ticks)/tree_fall_ticks,0.0F,1.0F) : 0;
    const float theta=progress*1.57079633F;
    float dx=reach*floor.w*static_cast<float>(dir.x)*std::sin(theta);
    const float dy=reach*floor.w*(static_cast<float>(dir.y)*std::sin(theta)-std::cos(theta));
    if (!tree.growth_ticks) dx+=floor.w*.12F*static_cast<float>(dir.x);
    if (tree.growth_ticks>tree_fall_ticks) dx+=std::sin(static_cast<float>(game.tick%100)*1.5F)*floor.w*.06F;
    const float width=floor.w*(1.25F+reach*.25F),height=std::max(floor.w*.2F,std::hypot(dx,dy))*64/58;
    const SDL_FPoint pivot{width*.5F,height*60/64};
    SDL_FRect body{floor.x+floor.w*.5F-pivot.x,floor.y+floor.h*.6F-pivot.y,width,height};
    const double angle=std::atan2(dx,-dy)*180/3.141592653589793;
    const auto light=light_at_cell(lighting,cell);
    bool obscures=false;
    for (Handle h : controlled_entities(game)) if (const Entity* player=get_entity(game,h))
        if (player->health>0 && std::abs(player->cell.x-cell.x)<=1 && player->cell.y<cell.y && player->cell.y>=cell.y-tree_height(tree)) obscures=true;
    const auto draw=[&](Sprite sprite,const SDL_FRect* source,SDL_FRect rect,SDL_FPoint anchor) {
        SDL_Texture* texture=texture_for(graphics,sprite);
        SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);SDL_SetTextureAlphaMod(texture,obscures ? 100 : 255);
        SDL_RenderTextureRotated(renderer,texture,source,&rect,angle,&anchor,SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(texture,255);SDL_SetTextureColorModFloat(texture,1,1,1);
    };
    draw(tree.hp<36 || tree.growth_ticks ? Sprite::TallTreeCut : Sprite::TallTree,nullptr,body,pivot);
    if(tile.kind==TileKind::Snow && !tile.surface.fire_ticks && !tree_burn_height(tree))
        draw(Sprite::TallTreeSnowCap,nullptr,body,pivot);
    const float burnt=static_cast<float>(tree_burn_height(tree))/12;
    if (burnt>0) {
        const SDL_FRect source{0,64*(1-burnt),32,64*burnt};
        SDL_FRect lower=body;lower.y+=height*(1-burnt);lower.h=height*burnt;
        const SDL_FPoint anchor{pivot.x,pivot.y-height*(1-burnt)};draw(Sprite::TallTreeChar,&source,lower,anchor);
    }
    if (tile.surface.fire_ticks>0) {
        SDL_Texture* flame=texture_for(graphics,(game.tick/8)%2 ? Sprite::FlameA : Sprite::FlameB);
        for (int n=0;n<=tree_burn_height(tree)/4;++n) {
            const float f=static_cast<float>(n)/4;
            SDL_FRect rect{floor.x+dx*f,floor.y+floor.h*.2F+dy*f-floor.h*.6F,floor.w,floor.h};
            SDL_RenderTexture(renderer,flame,nullptr,&rect);
        }
    }
}
void draw_tree_ground(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting,const Cosmetics* cosmetics) {
    const Prop& prop=game.stage.at(cell)->prop;SDL_FRect floor=tile_rect(cell,camera,zoom);
    if (prop.kind==PropKind::TallTree && !prop.broken) {
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_FRect shadow{floor.x+floor.w*.1F,floor.y+floor.h*.5F,floor.w*.8F,floor.h*.24F};
        SDL_SetRenderDrawColor(renderer,0,0,0,160);SDL_RenderFillRect(renderer,&shadow);
        if (prop.growth_ticks) {
            const Cell dir=tree_direction(prop);
            for (int n=1;n<=tree_height(prop);++n) {
                const Cell target=cell+Cell{dir.x*n,dir.y*n};const auto kind=game.stage.at_or_border(target).kind;
                if (kind==TileKind::Wall) break;
                SDL_FRect mark=tile_rect(target,camera,zoom);mark.x+=mark.w*.08F;mark.y+=mark.h*.2F;mark.w*=.84F;mark.h*=.6F;
                SDL_SetRenderDrawColor(renderer,0,0,0,175);SDL_RenderFillRect(renderer,&mark);
                SDL_SetRenderDrawColor(renderer,179,134,67,215);SDL_RenderRect(renderer,&mark);
            }
        }
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);return;
    }
    const auto light=light_at_cell(lighting,cell);
    const Sprite sprite=prop.kind==PropKind::TallTree ? Sprite::TallTreeStump : prop.broken ? Sprite::FallenLogBroken :
        prop.hp<=12 ? Sprite::FallenLogSplit : prop.hp<24 ? Sprite::FallenLogBruised : Sprite::FallenLog;
    if (prop.kind!=PropKind::TallTree && !prop.broken) floor=jolted_prop_rect(floor,cell,cosmetics);
    SDL_Texture* texture=texture_for(graphics,sprite);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTextureRotated(renderer,texture,nullptr,&floor,prop.kind!=PropKind::TallTree && (prop.variant&1U) ? 90 : 0,nullptr,SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
