#include "circuits.hpp"
#include "../props/interaction.hpp"

namespace {
constexpr RegionalItem wire{"Copper Wire","Lay linked wire ahead. Carries shocks within their range; never creates power. Six segments, 4 HP each.",
    Sprite::CopperWire,{1,1,0,0,15,PatternEffect::Utility},
    ItemAction::Material,11,1,false,6,0,0,0,0,SoundId::WireLay};
constexpr RegionalItem spike{"Grounding Spike","Absorbs one circuit pulse. Cool 3s or wet it; pick up and replant to rearm.",
    Sprite::GroundingSpike,{1,1,0,0,30,PatternEffect::Utility},
    ItemAction::Material,15,3,true,0,0,0,0,0,SoundId::SpikePlant,8};
}

const RegionalItem* circuit_item(ItemKind kind) {
    if (kind==ItemKind::CopperWire) return &wire;
    return kind==ItemKind::GroundingSpike ? &spike : nullptr;
}

// PLACEMENT: Water is useful here. Bodies, fixtures and existing props keep their cell.
bool circuit_space(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && walkable(tile->kind) && tile->kind!=TileKind::Lava &&
        (tile->prop.kind==PropKind::None || tile->prop.broken) && entity_at(game,cell,false)<0;
}

bool place_circuit_item(Game& game,Cell cell,const Item& item) {
    if (!circuit_space(game,cell)) return false;
    if (item.kind!=ItemKind::CopperWire && item.kind!=ItemKind::GroundingSpike) return false;
    if (item.kind==ItemKind::GroundingSpike && item.durability<=0) return false;
    Tile& tile=*game.stage.at(cell);
    tile.prop={};
    place_prop(game.stage,cell,item.kind==ItemKind::CopperWire ? PropKind::CopperWire : PropKind::GroundingSpike);
    if (item.kind==ItemKind::GroundingSpike) tile.prop.hp=static_cast<std::uint8_t>(item.durability);
    return true;
}
