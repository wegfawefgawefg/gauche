#include "pressure.hpp"
#include "../entities/boiler_feed.hpp"
#include "../entities/boiler_tank.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem valve{"Pressure Valve", "Fit a tank: vents point away from you. Recover below 25 pressure. A started warning keeps its aim.",
    Sprite::PressureValve,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,18,1,true,0,0,0,0,0,SoundId::ValveFit};
constexpr RegionalItem sealant{"Sealant", "Repair 20 HP of pipe, belt, drive or tank. Tank repair plugs its vent for 10s; pressure builds. Three portions.",
    Sprite::Sealant,{1,1,0,0,45,PatternEffect::Utility},ItemAction::Material,9,1,false,3,0,0,0,0,SoundId::SealantPatch};
}

const RegionalItem* pressure_item(ItemKind kind) {
    if (kind == ItemKind::PressureValve) return &valve;
    return kind == ItemKind::Sealant ? &sealant : nullptr;
}

bool use_pressure_item(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.inventory.held()->kind==ItemKind::Sealant && repair_feed_fixture(game,actor.cell+direction)) return true;
    const int target = boiler_at(game,actor.cell+direction);
    if (target < 0) return false;
    Entity& tank = game.entities[static_cast<std::size_t>(target)];
    const Item& item = *actor.inventory.held();
    if (item.kind == ItemKind::PressureValve) {
        if (tank.ground_item.kind != ItemKind::None) return false;
        tank.ground_item = item; tank.ground_item.count = 1;
        tank.facing = direction;
        return true;
    }
    if (tank.health == tank.max_health && tank.timer_b >= 600) return false;
    tank.health = std::min(tank.max_health,tank.health+20);
    tank.timer_b = 600;
    tank.label_a = BoilerIdle; tank.timer_a = 0;
    tank.sprite = Sprite::BoilerPlugged;
    return true;
}
