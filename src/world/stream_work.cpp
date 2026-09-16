#include "stream_work.hpp"
#include "ground_items.hpp"
#include "water.hpp"
#include "currents.hpp"
#include "../entities/fishing_work.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
Cell scale(Cell cell,int n) {return {cell.x*n,cell.y*n};}
bool dry_free(const Game& game,Cell cell) {
    const Tile& tile=game.stage.at_or_border(cell);
    return walkable(tile) && !shallow_water(tile.kind) && tile.kind!=TileKind::Water &&
        tile.kind!=TileKind::Lava && tile.prop.kind==PropKind::None &&
        entity_at(game,cell,false)<0 && distance(cell,game.run.spawn)>=8;
}
}
// Salvage crosses a fixed fishing lane on the existing shallow current. The
// bank worker, player hook and seal compete over the same finite item instances.
bool populate_stream_work(Game& game,const ThawChannel& channel) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    const Cell along=channel.along,across=channel.across;
    const Cell stand=channel.source+scale(along,4)-scale(across,2);
    const Cell seal=stand+scale(along,6);
    if (!dry_free(game,stand) || !dry_free(game,stand-across) || !dry_free(game,seal)) return false;
    for (int i=1;i<=4;++i) {
        const Cell cell=channel.source+scale(along,i)-across;
        if (water_current(game.stage.at_or_border(cell))!=along || entity_at(game,cell,false)>=0) return false;
    }
    Entity* widow=get_entity(game,spawn_entity(game,EntityKind::FishingWidow,stand));
    if (!widow) return false;
    start_fishing_work(*widow,across);
    // Place downstream first so one current beat advances the loose train.
    constexpr ItemKind stock[]{ItemKind::WoolWrap,ItemKind::IceNeedle,ItemKind::SmokedFish};
    for (int i=3;i>=1;--i) {
        const ItemKind kind=stock[static_cast<std::size_t>(i-1)];
        place_ground_item(game,channel.source+scale(along,i)-across,kind,i==3 ? 2 : i==2 ? 3 : 1);
    }
    spawn_entity(game,EntityKind::SealThief,seal);
    for (int offset:{-2,2}) {
        const Cell cell=stand-across+scale(along,offset);
        if (dry_free(game,cell)) place_prop(game.stage,cell,PropKind::FishingCreel);
    }
    return true;
}
