#include "pressure.hpp"
#include "../entities/boiler_tank.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem valve{"Pressure Valve", "Fit a tank: vents point away from you. Recover below 25 pressure. A started warning keeps its aim.",
    Sprite::PressureValve,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,18,1,true,0,0,0,0,0,SoundId::ValveFit};
constexpr RegionalItem sealant{"Sealant", "Repair 20 HP; plug a tank for 10s. Pressure keeps building. Three portions. Reopening may vent.",
    Sprite::Sealant,{1,1,0,0,45,PatternEffect::Utility},ItemAction::Material,9,1,false,3,0,0,0,0,SoundId::SealantPatch};
}

const RegionalItem* pressure_item(ItemKind kind) {
    if (kind == ItemKind::PressureValve) return &valve;
    return kind == ItemKind::Sealant ? &sealant : nullptr;
}

bool use_pressure_item(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
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

Item removable_valve(const Game& game, Cell cell) {
    const int slot = boiler_at(game,cell);
    if (slot < 0) return {};
    const Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    return tank.counter_a < 25 && tank.label_a == BoilerIdle &&
        tank.ground_item.kind == ItemKind::PressureValve ? tank.ground_item : Item{};
}

int release_valve(Game& game, Cell source, Cell destination) {
    const Item item = removable_valve(game,source);
    if (item.kind == ItemKind::None) return -1;
    const int slot = boiler_at(game,source);
    const Handle handle = spawn_entity(game,EntityKind::GroundItem,destination);
    Entity* loose = get_entity(game,handle);
    // POOL: Keep hardware attached until the replacement loose item exists.
    if (!loose) return -1;
    loose->ground_item = item; loose->sprite = item_sprite(item);
    game.entities[static_cast<std::size_t>(slot)].ground_item = {};
    emit_sound(game,SoundId::ValveRemove,source);
    return handle.slot;
}
