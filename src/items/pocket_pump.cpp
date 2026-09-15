#include "pocket_pump.hpp"
#include "action.hpp"
#include "../surfaces/liquid_transfer.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem pump{"Pocket Pump",
    "Use: collect spills. Secondary: pour. Holds one liquid, three pours. Starts full of water.",
    Sprite::PocketPump,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,23,1,false,0,0,0,0,0,SoundId::PumpDraw};
void worked(Game& game,Entity& user,Item& item,SoundId sound,Cell cell) {
    item.cooldown=item_pattern(item).cooldown;user.use_flash=8;
    emit_sound(game,sound,cell);
}
}
const RegionalItem* pocket_pump_item(ItemKind kind) {return kind==ItemKind::PocketPump ? &pump : nullptr;}
// STORAGE: loaded is a LiquidKind, spare is retained spill lifetime/volume.
// 600 units make one full ten-second puddle, 1800 units fill the sealed tank.
// Partial spills conserve their exact remaining quantity; storing stops evaporation.
bool valid_pocket_pump(const Item& item) {
    if (item.kind!=ItemKind::PocketPump) return true;
    return item.loaded>=0 && item.loaded<static_cast<int>(LiquidKind::Count) && item.spare>=0 && item.spare<=pump_capacity &&
        (item.spare==0 ? item.loaded==0 : pumpable_liquid(static_cast<LiquidKind>(item.loaded)));
}
const char* pump_contents(const Item& item) {
    switch (static_cast<LiquidKind>(item.loaded)) {
    case LiquidKind::Tar: return "TAR";
    case LiquidKind::Oil: return "OIL";
    case LiquidKind::Sap: return "SAP";
    case LiquidKind::Water: return "WATER";
    case LiquidKind::Honey: return "HONEY";
    case LiquidKind::Rot: return "ROT";
    case LiquidKind::Brine: return "BRINE";
    case LiquidKind::Coolant: return "COOLANT";
    default: return "EMPTY";
    }
}
bool collect_pocket_pump(Game& game,int slot,Cell direction) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::PocketPump || item.cooldown>0 || !valid_pocket_pump(item) || distance({},direction)!=1) return false;
    const Cell cell=user.cell+direction;
    const auto load=collect_spill(game,cell,static_cast<LiquidKind>(item.loaded),std::min(pump_portion,pump_capacity-item.spare));
    if (load.amount==0) return false;
    item.loaded=static_cast<int>(load.kind);item.spare+=load.amount;
    worked(game,user,item,SoundId::PumpDraw,cell);return true;
}
bool discharge_pocket_pump(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::PocketPump || item.cooldown>0 || item.spare<=0 || !valid_pocket_pump(item)) return false;
    const Cell cell=user.cell+user.facing;
    const int spent=discharge_spill(game,cell,{static_cast<LiquidKind>(item.loaded),std::min(pump_portion,item.spare)});
    if (spent==0) return false;
    item.spare-=spent;if (item.spare==0) item.loaded=0;
    worked(game,user,item,SoundId::PumpPour,cell);return true;
}
bool step_pump_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (user.inventory.held()->kind!=ItemKind::PocketPump) return false;
    if (user.label_b!=0) cancel_item_action(user);
    if (input.cancel_use || input.drop || input.interact) return true;
    if (input.reload) discharge_pocket_pump(game,slot);
    else if (input.use) collect_pocket_pump(game,slot,user.facing);
    return true;
}
