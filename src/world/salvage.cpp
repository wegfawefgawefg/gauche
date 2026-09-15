#include "salvage.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../items/supply.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {
bool suitable(const Game& game,const FloorPlan& plan,const RoomPlan& room,
              Cell center,Cell bank,Cell forward,Cell sideways) {
    const Tile* approach=game.stage.at(bank);
    if (!approach || !walkable(*approach) || approach->kind==TileKind::Lava ||
        approach->kind==TileKind::Water || approach->prop.kind!=PropKind::None ||
        entity_at(game,bank,false)>=0) return false;
    for (int y=-3;y<=2;++y) for (int x=-2;x<=2;++x) {
        const Cell cell=center+Cell{forward.x*y+sideways.x*x,forward.y*y+sideways.y*x};
        const Tile* tile=game.stage.at(cell);
        if (!tile || plan.protected_cell(cell) ||
            (!walkable(*tile) && tile->kind!=TileKind::Water) ||
            tile->prop.kind!=PropKind::None || tile->kind==TileKind::Spring ||
            entity_at(game,cell,false)>=0 || distance(cell,game.run.spawn)<6 ||
            std::abs(cell.x-room.center.x)>=room.half_width ||
            std::abs(cell.y-room.center.y)>=room.half_height) return false;
    }
    return true;
}
}

bool place_salvage_pocket(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    const bool cold=ice_floor(game.run.floor);
    const bool hot=industrial_floor(game.run.floor);
    if (cold && room.role!=RoomRole::FishingHut && room.role!=RoomRole::Reservoir) return false;
    if (hot && room.role!=RoomRole::Workfront && room.role!=RoomRole::BlastingAlcove &&
        room.role!=RoomRole::Workshop && room.role!=RoomRole::Cache) return false;
    if (!cold && !hot) return false;
    // A single optional cache has its own small scene budget. Reserve enough
    // entity slots for the weapon, coins and (in Ice) the shore's fishing line.
    const auto free=std::count_if(game.entities.begin(),game.entities.end(),
        [](const Entity& actor){return actor.kind==EntityKind::None;});
    if (free<(cold ? 3 : 2)) return false;
    for (bool vertical:{true,false}) for (int sy:{-1,1}) for (int sx:{1,-1}) {
        const Cell forward=vertical ? Cell{0,sy} : Cell{sx,0};
        const Cell sideways=vertical ? Cell{sx,0} : Cell{0,sy};
        const Cell center=room.center+Cell{sx*(vertical ? 4 : 5),sy*(vertical ? 5 : 4)};
        const Cell bank=center-Cell{forward.x*4,forward.y*4};
        if (!suitable(game,plan,room,center,bank,forward,sideways)) continue;
        const ItemKind reward=roll_item_supply(game,LootSource::Salvage,false);
        if (reward==ItemKind::None) return false;
        // An isolated dry plinth, with an exact three-cell portable-bridge span.
        // Hooks can recover its real loose item; bridges span the Ice pocket and
        // teleports reach either. Coins give actually crossing an extra payoff.
        for (int y=-3;y<=2;++y) for (int x=-2;x<=2;++x)
            *game.stage.at(center+Cell{forward.x*y+sideways.x*x,forward.y*y+sideways.y*x})=
                {cold ? TileKind::Water : TileKind::Lava,0,0};
        *game.stage.at(center)={TileKind::Ruin,0,0};
        place_coins(game,center,5+static_cast<int>(random_u32(game)%5));
        place_ground_item(game,center,reward,supply_count(reward));
        if (cold) place_ground_item(game,bank,ItemKind::FishingLine);
        // A broken-working-place cue beside the casting bank, never on its ray.
        const Cell crate=center+Cell{sideways.x*3-forward.x*2,sideways.y*3-forward.y*2};
        Tile* tile=game.stage.at(crate);
        if (tile && walkable(*tile) && tile->kind!=TileKind::Lava &&
            tile->prop.kind==PropKind::None && !plan.protected_cell(crate) &&
            entity_at(game,crate,false)<0)
            place_prop(game.stage,crate,cold ? PropKind::FishingCreel : PropKind::ScrapBin);
        return true;
    }
    return false;
}
