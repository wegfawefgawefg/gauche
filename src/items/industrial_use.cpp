#include "mold_key.hpp"
#include "rail_switch_key.hpp"
#include "insulated_boots.hpp"
#include "machine_fittings.hpp"
#include "hand_bellows.hpp"
#include "industrial_use.hpp"
#include "bolt_pouch.hpp"
#include "emergency_foam.hpp"
#include "tension_spring.hpp"
#include "coolant.hpp"
#include "barricade.hpp"
#include "magnet.hpp"
#include "belt_tools.hpp"
#include "foreman_whistle.hpp"
#include "quarry_charge.hpp"
#include "fuse_scissors.hpp"

bool use_industrial_tool(Game& game, int user_slot, Cell direction, int range, int& cooldown) {
    const Item& item=*game.entities[static_cast<std::size_t>(user_slot)].inventory.held();
    bool used=false;
    cooldown=0;
    switch (item.kind) {
    case ItemKind::InsulatedBoots:
        used=use_insulated_boots(game.entities[static_cast<std::size_t>(user_slot)]); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::RailSwitchKey:
        used=use_rail_switch_key(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::MoldKey:
        used=unlock_casting_mold(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::NozzleElbow:
        used=fit_nozzle_elbow(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::HandBellows:
        used=use_hand_bellows(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::BoltPouch:
        used=throw_bolts(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::EmergencyFoam:
        used=throw_emergency_foam(game,user_slot,direction,range); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::TensionSpring:
        used=place_tension_spring(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::CoolantCan:
        used=pour_coolant(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::FoldingBarricade:
        used=place_barricade(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::HorseshoeMagnet:
        used=pull_magnetic_item(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::BeltCrank: case ItemKind::BrakeShoe:
        used=use_belt_tool(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    case ItemKind::QuarryCharge:
        used=place_quarry_charge(game,user_slot); cooldown=30; break;
    case ItemKind::FuseScissors:
        used=snip_fuse(game,user_slot,direction); cooldown=24; break;
    case ItemKind::ForemanWhistle:
        used=use_foreman_whistle(game,user_slot,direction); cooldown=item_pattern(item).cooldown; break;
    default: break;
    }
    return used;
}
