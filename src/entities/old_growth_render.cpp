#include "old_growth_render.hpp"
#include "old_growth_bear.hpp"
#include "attacks.hpp"
#include "../world/terrain_material.hpp"
#include <algorithm>

void draw_old_growth_warning(SDL_Renderer* renderer,const Game& game,ViewCamera camera,float zoom,const LightingCache& lighting) {
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for(const auto& bear:game.entities) {
        if(bear.kind!=EntityKind::OldGrowthBear || bear.health<=0 || bear.sleep_ticks || bear.stun_ticks || bear.freeze_ticks ||
            (bear.label_a!=GrowthMaul && bear.label_a!=GrowthPaw && bear.label_a!=GrowthRush))continue;
        const auto seen=lit_sprite_color(lighting,bear.cell);
        if(std::max({seen.red,seen.green,seen.blue})<.1F)continue;
        const auto attack=enemy_attack(bear);const bool rushing=bear.label_a!=GrowthMaul;
        const float progress=rushing ? 1-std::clamp(static_cast<float>(bear.timer_a)/72,0.F,1.F) : 1-std::clamp(static_cast<float>(bear.timer_a)/54,0.F,1.F);
        for(int i=0;i<attack.count;++i) {
            const Cell cell=attack.cells[static_cast<std::size_t>(i)];
            const auto* tile=game.stage.at(cell);if(!tile)break;
            if(!rushing && !clear_attack_sight(game,bear.cell,cell,false))continue;
            const auto light=lit_sprite_color(lighting,cell);
            const float level=std::clamp(std::max({light.red,light.green,light.blue}),0.F,1.F);
            const auto alpha=static_cast<Uint8>(level*(110+100*progress));
            auto rect=tile_rect(cell,camera,zoom);const float p=rect.w/16;
            SDL_SetRenderDrawColor(renderer,203,141,76,static_cast<Uint8>(alpha/4));
            SDL_FRect inset{rect.x+p*2,rect.y+p*4,rect.w-p*4,rect.h-p*6};SDL_RenderFillRect(renderer,&inset);
            SDL_SetRenderDrawColor(renderer,221,159,89,alpha);
            if(rushing) {
                // Directional chevrons stay readable for both vertical and horizontal rushes.
                const float cx=rect.x+rect.w*.5F,cy=rect.y+rect.h*.5F;
                const float dx=static_cast<float>(bear.facing.x)*p*4,dy=static_cast<float>(bear.facing.y)*p*4;
                SDL_RenderLine(renderer,cx-dx-dy,cy-dy+dx,cx+dx,cy+dy);
                SDL_RenderLine(renderer,cx-dx+dy,cy-dy-dx,cx+dx,cy+dy);
            } else for(int claw=0;claw<3;++claw)
                SDL_RenderLine(renderer,rect.x+p*static_cast<float>(4+claw*3),rect.y+p*11,rect.x+p*static_cast<float>(6+claw*3),rect.y+p*5);
            // Timber and soft cover can be broken by this rush; stone cannot.
            if(rushing && ((tile->kind==TileKind::Wall && (!wooden_terrain(*tile) || tile->hp>120 || tile->break_rule==BreakRule::Unbreakable)) ||
                (prop_blocks(tile->prop) && tile->prop.hp>90) || tile->kind==TileKind::Chasm || tile->kind==TileKind::Water || tile->kind==TileKind::IceHole))break;
        }
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
