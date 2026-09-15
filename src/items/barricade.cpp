#include "barricade.hpp"
#include "../entities/attacks.hpp"

namespace {
constexpr RegionalItem kit{"Folding Barricade",
    "Three low steel sections. Walkers stop; bullets/arrows pass, lobbed bombs go over. Break panels to reopen gaps.",
    Sprite::FoldingBarricade,{1,1,0,0,45,PatternEffect::Utility,false,1},
    ItemAction::Material,23,1,true,0,0,0,0,0,SoundId::BarricadeUnfold,20};
}
const RegionalItem* barricade_item(ItemKind kind) { return kind==ItemKind::FoldingBarricade ? &kit : nullptr; }
std::array<Cell,3> barricade_cells(Cell origin,Cell direction) {
    const Cell front=origin+direction,side{-direction.y,direction.x};
    return {front-side,front,front+side};
}
bool place_barricade(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::FoldingBarricade || item.durability<=0 || distance({},direction)!=1) return false;
    const auto cells=barricade_cells(user.cell,direction);
    for (Cell cell:cells) {
        const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->kind==TileKind::Rail ||
            (tile->prop.kind!=PropKind::None && !tile->prop.broken) || entity_at(game,cell,false)>=0 ||
            !clear_attack_sight(game,user.cell,cell,false)) return false;
    }
    // One kit unfolds completely or remains in the inventory. Sections become
    // ordinary compact props: no actor slots, hidden links or duplicated kit.
    const auto variant=static_cast<std::uint8_t>((direction.x!=0 ? 1 : 0) | (item.attribute==ItemAttribute::Durable ? 2 : 0));
    for (Cell cell:cells) game.stage.at(cell)->prop={PropKind::Barricade,static_cast<std::uint8_t>(item.durability),variant,false};
    return true;
}
