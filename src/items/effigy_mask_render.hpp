#pragma once
#include "effigy_mask.hpp"
#include "../lighting/field.hpp"

inline void draw_effigy_mask(SDL_Renderer* renderer,const GameGraphics& graphics,
    const Entity& user,SDL_FRect rect,LightColor light) {
    if (!effigy_mask_active(user)) return;
    SDL_FRect face{rect.x+rect.w*(.25F-.35F*static_cast<float>(user.facing.x)),
        rect.y+rect.h*(.20F-.35F*static_cast<float>(user.facing.y)),rect.w*.5F,rect.h*.5F};
    SDL_Texture* texture=texture_for(graphics,Sprite::MaskWatching);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTexture(renderer,texture,nullptr,&face);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}
