#include "../combat/toss.hpp"
#include "shadows.hpp"
#include "../debug/panels.hpp"
#include "../entities/bell_diver.hpp"
#include "../particles/system.hpp"
#include "../projectiles/render.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace {
// A clipped-corner oval fits the small pixel silhouettes without a soft blur.
constexpr std::array<SDL_FPoint,8> outline{{{-.5F,-.2F},{-.3F,-.5F},{.3F,-.5F},{.5F,-.2F},
    {.5F,.2F},{.3F,.5F},{-.3F,.5F},{-.5F,.2F}}};
struct ShadowBatch {
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;
    void add(float x,float y,float width,float height,float alpha) {
        if (x+width*.5F<0 || y+height*.5F<0 || x-width*.5F>640 || y-height*.5F>360) return;
        const int base=static_cast<int>(vertices.size());
        for (SDL_FPoint p:outline) vertices.push_back({{x+p.x*width,y+p.y*height},{0,0,0,alpha},{}});
        for (int triangle=1;triangle<7;++triangle)
            for (int corner:{0,triangle,triangle+1}) indices.push_back(base+corner);
    }
};

bool solid_prop(PropKind kind) {
    switch (kind) {
    case PropKind::None: case PropKind::Leaves: case PropKind::Twigs:
    case PropKind::Nest: case PropKind::RootCover: case PropKind::BirdSeed:
    case PropKind::Thorns: case PropKind::SpiderStrand: case PropKind::CopperWire:
    case PropKind::BridgePlank: case PropKind::Conveyor: return false;
    default: return true;
    }
}

bool raised_scrap(DebrisKind kind) {
    switch (kind) {
    case DebrisKind::Spore: case DebrisKind::GrassBlade: case DebrisKind::PineNeedle:
    case DebrisKind::Straw: case DebrisKind::FlareCinder: case DebrisKind::KelpScrap:
    case DebrisKind::Count: return false;
    default: return true;
    }
}

void entity_shadow(ShadowBatch& batch,const Game& game,const Entity& actor,ViewCamera camera,float zoom) {
    if (game.stage.at_or_border(actor.cell).kind==TileKind::Chasm) return;
    switch (actor.kind) {
    case EntityKind::None: case EntityKind::RailLayer: case EntityKind::Trap:
    case EntityKind::Exit: case EntityKind::Encounter: case EntityKind::WaveVent:
    case EntityKind::PocketDoor: case EntityKind::EncounterGate: return;
    default: break;
    }
    if ((actor.kind==EntityKind::Door && actor.fixture_open) || diver_submerged(actor)) return;
    const float pixels=tile_pixels(zoom);
    if (actor.kind==EntityKind::Projectile && actor.label_a==static_cast<int>(ProjectileKind::Net)) {
        const int width=item_pattern(actor.ground_item).half_width;
        const Cell side{-actor.facing.y,actor.facing.x};
        const float net_travel=projectile_blocked(game,actor.cell+actor.facing) ? 0 :
            std::clamp(1-static_cast<float>(actor.timer_b)/4,0.0F,1.0F);
        for (int lane=-width;lane<=width;++lane) {
            if ((actor.label_b & (1 << (lane+width)))==0) continue;
            const Cell cell=actor.cell+Cell{side.x*lane,side.y*lane};
            if (projectile_blocked(game,cell)) continue;
            const float travel=projectile_blocked(game,cell+actor.facing) ? 0 : net_travel;
            const SDL_FRect rect=tile_rect(cell,camera,zoom);
            batch.add(rect.x+pixels*(.5F+static_cast<float>(actor.facing.x)*travel),
                rect.y+pixels*(.60F+static_cast<float>(actor.facing.y)*travel),pixels*.40F,pixels*.10F,.25F);
        }
        return;
    }
    if (actor.kind==EntityKind::Projectile) {
        const ProjectilePose pose=projectile_pose(actor,game,camera,zoom);
        const float width=actor.label_a==static_cast<int>(ProjectileKind::Arrow) ? .18F : .30F;
        batch.add(pose.ground.x+pose.ground.w*.5F,pose.ground.y+pose.ground.h*.58F,
            pixels*width*(1+pose.height*.18F),pixels*.10F,.30F/(1+pose.height*.6F));
        return;
    }
    const SDL_FRect rect=tile_rect(actor.cell,camera,zoom);
    const bool small=actor.kind==EntityKind::GroundItem || actor.kind==EntityKind::Coins || actor.kind==EntityKind::Key || actor.sprite==Sprite::Chick;
    const bool insect=actor.kind==EntityKind::Mosquito || actor.kind==EntityKind::Wasp || actor.kind==EntityKind::LanternMoth || actor.kind==EntityKind::FurnaceMoth;
    const float width=actor.kind==EntityKind::Train ? 1.3F : small ? .32F : insect ? .30F : .58F;
    const float height=actor.kind==EntityKind::Train ? .3F : small || insect ? .10F : .17F;
    batch.add(rect.x+pixels*.5F,rect.y+pixels*(small ? .67F : .83F),pixels*width,pixels*height,(insect ? .22F : .38F)/(1+actor_toss_height(actor)));
}
}

void draw_contact_shadows(SDL_Renderer* renderer,const Game& game,
                          const Cosmetics* cosmetics,ViewCamera camera,float zoom) {
    const DebugPanels& options=debug_panels();
    if (!options.contact_shadows) return;
    // Local scratch is reused across frames/views; no shadow state enters the simulation.
    static ShadowBatch batch;
    batch.vertices.clear(); batch.indices.clear();
    const float pixels=tile_pixels(zoom);
    if (options.shadow_props) {
        const int rx=static_cast<int>(320/pixels)+2,ry=static_cast<int>(180/pixels)+2;
        const int cx=static_cast<int>(std::floor(camera.x)),cy=static_cast<int>(std::floor(camera.y));
        for (int y=std::max(0,cy-ry);y<std::min(game.stage.height,cy+ry);++y)
            for (int x=std::max(0,cx-rx);x<std::min(game.stage.width,cx+rx);++x) {
                const Prop& prop=game.stage.at({x,y})->prop;
                if (prop.broken || !solid_prop(prop.kind)) continue;
                const SDL_FRect rect=tile_rect({x,y},camera,zoom);
                batch.add(rect.x+pixels*.5F,rect.y+pixels*.82F,pixels*.64F,pixels*.16F,.32F);
            }
    }
    if (options.shadow_debris && cosmetics)
        for (const LoosePiece& p:cosmetics->debris.pieces) {
            if (!raised_scrap(p.kind)) continue;
            const float size=pixels*(.35F+static_cast<float>(std::min<int>(p.count,6))*.025F);
            batch.add(view_center_x+(p.x-camera.x)*pixels,view_center_y+(p.y-camera.y)*pixels+size*.13F,
                size*.64F,std::max(.45F,size*.18F),.22F);
        }
    if (options.shadow_entities)
        for (const Entity& actor:game.entities) entity_shadow(batch,game,actor,camera,zoom);
    if (batch.vertices.empty()) return;
    SDL_BlendMode previous;
    SDL_GetRenderDrawBlendMode(renderer,&previous);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(renderer,nullptr,batch.vertices.data(),static_cast<int>(batch.vertices.size()),
        batch.indices.data(),static_cast<int>(batch.indices.size()));
    SDL_SetRenderDrawBlendMode(renderer,previous);
}
