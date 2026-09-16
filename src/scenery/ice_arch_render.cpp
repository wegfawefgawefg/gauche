#include "ice_arch_render.hpp"
#include <algorithm>

namespace {
void piece(SDL_Renderer* renderer,const GameGraphics& graphics,Sprite sprite,SDL_FRect rect,
    LightColor light,float alpha,SDL_FlipMode flip=SDL_FLIP_NONE,float lean=0) {
    if (rect.x+rect.w+lean<0 || rect.y+rect.h<0 || rect.x>640 || rect.y>360) return;
    auto* texture=texture_for(graphics,sprite);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_SetTextureAlphaModFloat(texture,alpha);
    if (lean==0) SDL_RenderTextureRotated(renderer,texture,nullptr,&rect,0,nullptr,flip);
    else {
        // A slight eastward cant exposes the north/south arch's open side.
        // The feet stay rooted; rotating a tall sprite would rotate its height.
        const float left=flip==SDL_FLIP_HORIZONTAL ? 1.0F : 0.0F,right=1-left;
        const SDL_FColor tint{light.red,light.green,light.blue,alpha};
        const SDL_Vertex vertices[]{
            {{rect.x+lean,rect.y},tint,{left,0}},{{rect.x+rect.w+lean,rect.y},tint,{right,0}},
            {{rect.x+rect.w,rect.y+rect.h},tint,{right,1}},{{rect.x,rect.y+rect.h},tint,{left,1}}};
        constexpr int indices[]{0,1,2,0,2,3};
        SDL_SetTextureColorModFloat(texture,1,1,1);SDL_SetTextureAlphaMod(texture,255);
        SDL_RenderGeometry(renderer,texture,vertices,4,indices,6);
    }
    SDL_SetTextureColorModFloat(texture,1,1,1);SDL_SetTextureAlphaMod(texture,255);
}
}

void draw_ice_arch_row(SDL_Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
    const RoofSpan& roof,int row,const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting) {
    if (!roof.vertical && row!=1) return;
    const bool reveal=viewer && reveal_roof(roof,viewer->cell);
    const float condition=static_cast<float>(roof.hp)/static_cast<float>(roof_health(roof.kind));
    for (int a=0;a<roof.length;++a) {
        if (roof.vertical && a!=row) continue;
        const Cell cell=roof_cell(roof,a,1);
        const auto floor=tile_rect(cell,camera,zoom);
        const auto light=light_at_cell(lighting,cell);
        const float top=floor.y+floor.h*.8F-floor.h*roof.height;
        const bool end=a==0 || a==roof.length-1;
        const auto& foot=stage.at_or_border(cell).prop;
        if (end && foot.kind==PropKind::IceArchFoot && !foot.broken) {
            SDL_FRect support{floor.x,top,floor.w,floor.h*roof.height};
            piece(renderer,graphics,Sprite::IceArchFoot,support,light,1,
                a==0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL,roof.vertical ? floor.w*.9F : 0);
        }
        // Open air occupies the full space below this thin, faceted crown.
        // Each part keeps its ground-row anchor, including north/south arches.
        SDL_FRect crown{floor.x,top-floor.h*9/16,floor.w,floor.h*1.5F};
        Sprite sprite=end ? Sprite::IceArchShoulder : Sprite::IceArchCrown;
        if (roof.vertical) {sprite=Sprite::IceArchCrownV;crown.y=top-floor.h*.25F;crown.x+=floor.w*.9F;}
        const float tint=.8F+.2F*condition;
        piece(renderer,graphics,sprite,crown,{light.red*tint,light.green*tint,light.blue*tint},reveal ? .18F : 1,
            !roof.vertical && a==roof.length-1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
}
