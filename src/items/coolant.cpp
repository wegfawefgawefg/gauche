#include "coolant.hpp"
#include "kettle.hpp"
#include "../surfaces/interaction.hpp"
#include "../entities/boiler_tank.hpp"
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
        for (Entity& actor:game.entities) {
            if (actor.kind==EntityKind::None || actor.cell!=cell || actor.health<=0) continue;
            apply_chill(actor,180);
            if (actor.kind!=EntityKind::BoilerTank) continue;
            // Forced cooling reaches the vessel even with a sealed outlet.
            // It removes pressure, never remaining fuel or an attached valve.
            reduce_boiler_pressure(actor,80);
        }
    }
    return used;
}
