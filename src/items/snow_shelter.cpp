#include "snow_shelter.hpp"
#include "../props/ice_cover.hpp"

namespace {
constexpr RegionalItem shelter{"Snow Shelter",
    "Two low walls ahead/right. Blocks snow and flat shots; bombs lob over. Heat melts it. Needs two clear cells.",
    Sprite::SnowShelter,{1,1,0,0,45,PatternEffect::Utility},
    ItemAction::Material,18,1,false,2,0,0,0,0,SoundId::ShelterPack};
}
const RegionalItem* snow_shelter_item(ItemKind kind) { return kind==ItemKind::SnowShelter ? &shelter : nullptr; }

std::array<Cell,2> snow_shelter_cells(Cell origin,Cell direction) {
    const Cell front=origin+direction,side{-direction.y,direction.x};
    return {front,front+side};
}
bool place_snow_shelter(Game& game,int slot,Cell direction) {
    if (distance({},direction)!=1) return false;
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const auto cells=snow_shelter_cells(user.cell,direction);
    // COMMIT: Validate both before touching either; no partial wall or spent charge.
    for (Cell cell : cells) if (!ice_cover_space(game,cell)) return false;
    const auto orientation=static_cast<std::uint8_t>(direction.x!=0 ? 1 : 0);
    for (Cell cell : cells)
        game.stage.at(cell)->prop={PropKind::SnowWindbreak,24,orientation,false};
    return true;
}
