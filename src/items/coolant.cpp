#include "../surfaces/liquid_transfer.hpp"
#include "coolant.hpp"
#include "kettle.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem can{"Coolant Can",
    "Four three-cell pours. Douse fire, chill actors, drain boiler pressure. Cools lava to stone. Residue slips and conducts shocks for 10s.",
    Sprite::CoolantCan,{1,1,0,0,45,PatternEffect::Utility,false,1},
    ItemAction::Material,12,1,false,4,0,0,0,0,SoundId::CoolantPour};
}
const RegionalItem* coolant_item(ItemKind kind) { return kind==ItemKind::CoolantCan ? &can : nullptr; }
bool pour_coolant(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const auto cells=kettle_cells(game,*user.inventory.held(),user.cell,direction);
    bool used=false;
    for (Cell cell:cells) {
        if (!pour_surface(game,cell,LiquidKind::Coolant,600)) continue;
        used=true;
        splash_coolant(game,cell,600);
    }
    return used;
}
