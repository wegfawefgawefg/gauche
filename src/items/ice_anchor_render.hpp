#pragma once
#include "ice_anchor.hpp"
#include "../lighting/render.hpp"

inline void draw_anchor_tether(SDL_Renderer* renderer,const Game& game,const Entity& anchor,
    ViewCamera camera,float zoom,const LightingCache& lighting) {
    for (Handle handle : controlled_entities(game)) {
        const Entity* user=get_entity(game,handle);
        if (!user || user->health<=0 || user->inventory.held()->kind!=ItemKind::IceAnchor ||
            get_entity(game,user->inventory.held()->anchor)!=&anchor ||
            !anchor_path_clear(game,user->cell,anchor.cell)) continue;
        const LightColor light=lit_sprite_color(lighting,user->cell);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColorFloat(renderer,light.red*.62F,light.green*.62F,light.blue*.49F,.8F);
        Cell cell=user->cell;
        while (cell!=anchor.cell) {
            const Cell next=anchor_next_cell(cell,anchor.cell);
            const SDL_FRect a=tile_rect(cell,camera,zoom),b=tile_rect(next,camera,zoom);
            SDL_RenderLine(renderer,a.x+a.w*.5F,a.y+a.h*.7F,b.x+b.w*.5F,b.y+b.h*.7F);
            cell=next;
        }
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
    }
}
