#include "stillwater_bell.hpp"
#include "../entities/hearing.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem bell{"Stillwater Bell",
    "Calms currents, floats and ice/oil slips for 3s within four steps. Walk and attack normally; shoves still work.",
    Sprite::StillwaterBell,{0,0,4,0,60,PatternEffect::Utility},
    ItemAction::Material,30,1,false,4,0,0,0,0,SoundId::StillwaterRing};
}
const RegionalItem* stillwater_bell_item(ItemKind kind) {
    return kind==ItemKind::StillwaterBell ? &bell : nullptr;
}

bool ring_stillwater_bell(Game& game, int slot) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const auto cells=audible_cells(game,user.cell,item_pattern(*user.inventory.held()).blast_radius);
    bool changed=false;
    for (Cell cell : cells) {
        Tile* tile=game.stage.at(cell);
        if (!tile || tile->surface.still_ticks>=180) continue;
        tile->surface.still_ticks=180;
        changed=true;
    }
    if (!changed) return false;
    for (Entity& actor : game.entities)
        if (actor.kind!=EntityKind::None && std::find(cells.begin(),cells.end(),actor.cell)!=cells.end())
            actor.vitals.slide_momentum=0;
    make_noise(game,user.cell,8);
    return true;
}
