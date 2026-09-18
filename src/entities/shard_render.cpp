#include "shard_render.hpp"
#include "shard_colony.hpp"
#include "../lighting/render.hpp"

#include <algorithm>

void draw_shard_links(tr::Renderer* renderer, const Game& game, ViewCamera camera,
                      float zoom, const LightingCache& lighting) {
    tr::set_blend(renderer,SDL_BLENDMODE_BLEND);
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& node = game.entities[static_cast<std::size_t>(slot)];
        if (node.kind != EntityKind::ShardColony || node.health <= 0 ||
            (node.label_a != ShardCharge && node.label_a != ShardPulse)) continue;
        const ShardGroup group = shard_group(game,node.entity_a);
        if (group.count < 2 || group.nodes[0].slot != slot) continue;
        for (const ShardLink& link : shard_links(game,group)) {
            const bool pulse = node.label_a == ShardPulse;
            // GRID: The dotted warning visits the exact cells the pulse will hit.
            // Short connected segments make the diagonal's tile coverage unambiguous.
            SDL_FPoint previous{};
            bool first = true;
            for (Cell cell : link.cells) {
                const SDL_FRect tile = tile_rect(cell,camera,zoom);
                const SDL_FPoint center{tile.x+tile.w*.5F,tile.y+tile.h*.5F};
                const LightColor seen = lit_sprite_color(lighting,cell);
                const float level = std::clamp(std::max({seen.red,seen.green,seen.blue}),.18F,1.0F);
                const float alpha = level*(pulse ? .95F : .24F);
                tr::set_color(renderer,.52F,.83F,.95F,alpha);
                if (!first) tr::line(renderer,previous.x,previous.y,center.x,center.y);
                const float size = tile.w*(pulse ? .12F : .065F);
                const SDL_FRect mote{center.x-size*.5F,center.y-size*.5F,size,size};
                tr::set_color(renderer,.72F,.93F,1.0F,level*(pulse ? 1.0F : .65F));
                tr::fill_rect(renderer,&mote);
                previous = center; first = false;
            }
        }
    }
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}
