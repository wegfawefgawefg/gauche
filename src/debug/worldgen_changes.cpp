#include "worldgen.hpp"

void draw_worldgen_changes(tr::Renderer* renderer,const WorldGenViewer& v,const Game& before,const Game& after) {
    if (v.changes) for (int y=0;y<after.stage.height;++y) for (int x=0;x<after.stage.width;++x) {
        const Cell cell{x,y};const auto& now=*after.stage.at(cell);const auto* old=before.stage.at(cell);
        if (old && old->kind==now.kind && old->material==now.material && old->prop.kind==now.prop.kind &&
            old->prop.hp==now.prop.hp && old->current==now.current && old->contents==now.contents) continue;
        tr::set_color_bytes(renderer,255,130,40,95);
        const auto box=tile_rect(cell,v.render.camera,v.zoom);tr::fill_rect(renderer,&box);
    }
    if (!v.actor_changes) return;
    const auto mark=[&](Cell cell,bool cross) {
        const auto box=tile_rect(cell,v.render.camera,v.zoom);tr::rect(renderer,&box);
        if (cross) {
            tr::line(renderer,box.x,box.y,box.x+box.w,box.y+box.h);
            tr::line(renderer,box.x+box.w,box.y,box.x,box.y+box.h);
        }
    };
    for (std::size_t i=0;i<after.entities.size();++i) {
        const auto& old=before.entities[i];const auto& now=after.entities[i];
        const bool existed=old.kind!=EntityKind::None,exists=now.kind!=EntityKind::None;
        const bool same=existed && exists && old.kind==now.kind && old.generation==now.generation;
        if (existed && (!same || old.cell!=now.cell)) {tr::set_color_bytes(renderer,255,90,80,230);mark(old.cell,true);}
        if (exists && !same) {tr::set_color_bytes(renderer,100,255,130,230);mark(now.cell,false);}
        if (same && (old.cell!=now.cell || old.health!=now.health || old.sprite!=now.sprite ||
            old.ground_item.kind!=now.ground_item.kind || old.ground_item.count!=now.ground_item.count)) {
            tr::set_color_bytes(renderer,255,225,80,230);mark(now.cell,false);
            if (old.cell!=now.cell) {
                const auto a=tile_rect(old.cell,v.render.camera,v.zoom),b=tile_rect(now.cell,v.render.camera,v.zoom);
                tr::line(renderer,a.x+a.w*.5F,a.y+a.h*.5F,b.x+b.w*.5F,b.y+b.h*.5F);
            }
        }
    }
}
