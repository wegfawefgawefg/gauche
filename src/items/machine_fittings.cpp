#include "../artifacts/powers.hpp"
#include "machine_fittings.hpp"
#include "../entities/boiler_tank.hpp"
#include "../entities/emergency_pump.hpp"
#include "../world/ground_items.hpp"

namespace {
constexpr RegionalItem elbow{"Nozzle Elbow",
    "Bend boiler/pump jets 90 degrees. Secondary: left/right. Recover when idle. Hits wear it. Warned jets keep their aim.",
    Sprite::NozzleElbow,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,14,1,true,0,0,0,0,0,SoundId::NozzleFit,18};
bool compatible(EntityKind kind) {return kind==EntityKind::BoilerTank || kind==EntityKind::EmergencyPump;}
int machine_at(const Game& game,Cell cell) {
    for (int i=0;i<max_entities;++i) {
        const Entity& machine=game.entities[static_cast<std::size_t>(i)];
        if (compatible(machine.kind) && machine.cell==cell && machine.health>0) return i;
    }
    return -1;
}
bool removable(const Entity& machine) {
    if (machine.kind==EntityKind::BoilerTank)
        return machine.label_a==BoilerIdle && machine.counter_a<25;
    return machine.label_a==PumpRoam || (machine.label_a==PumpRecover && (machine.label_b==0 || machine.timer_a<=60));
}
int loosen(Game& game,const Item& fitting,Cell destination) {
    Entity* loose=get_entity(game,spawn_entity(game,EntityKind::GroundItem,destination));
    if (!loose) return -1;
    loose->ground_item=fitting;loose->sprite=item_sprite(fitting);
    return static_cast<int>(loose-game.entities.data());
}
}
const RegionalItem* nozzle_elbow_item(ItemKind kind) {return kind==ItemKind::NozzleElbow ? &elbow : nullptr;}

// STORAGE: ground_item is the actual single fitting, not a generated replacement.
// Elbow.loaded chooses clockwise (0) or counterclockwise (1); durability is its HP.
// No machine direction is rewritten, so removal restores the ordinary outlet.
Cell outlet_direction(const Entity& machine,Cell straight) {
    const Item& fitting=machine.ground_item;
    if (fitting.kind!=ItemKind::NozzleElbow || fitting.count<=0 || fitting.durability<=0) return straight;
    return fitting.loaded ? Cell{straight.y,-straight.x} : Cell{-straight.y,straight.x};
}
bool fit_nozzle_elbow(Game& game,int slot,Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const Item& item=*user.inventory.held();
    if (distance({},direction)!=1 || item.kind!=ItemKind::NozzleElbow || !valid_nozzle_elbow(item)) return false;
    const int target=machine_at(game,user.cell+direction);
    if (target<0) return false;
    Entity& machine=game.entities[static_cast<std::size_t>(target)];
    if (machine.ground_item.kind!=ItemKind::None) return false;
    machine.ground_item=item;machine.ground_item.cooldown=0;
    return true;
}
bool reverse_nozzle_elbow(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::NozzleElbow || !valid_nozzle_elbow(item) || item.cooldown>0) return false;
    item.loaded=1-item.loaded;item.cooldown=18;
    emit_sound(game,SoundId::NozzleTurn,user.cell);return true;
}
Item removable_machine_fitting(const Game& game,Cell cell) {
    const int slot=machine_at(game,cell);
    if (slot<0) return {};
    const Entity& machine=game.entities[static_cast<std::size_t>(slot)];
    return removable(machine) ? machine.ground_item : Item{};
}
int release_machine_fitting(Game& game,Cell source,Cell destination) {
    const Item fitting=removable_machine_fitting(game,source);
    if (fitting.kind==ItemKind::None) return -1;
    const int slot=machine_at(game,source);
    const int released=loosen(game,fitting,destination);
    // Exhausted entity pools leave the fitting attached, ready for a later try.
    if (released<0) return -1;
    game.entities[static_cast<std::size_t>(slot)].ground_item={};
    emit_sound(game,fitting.kind==ItemKind::PressureValve ? SoundId::ValveRemove : SoundId::NozzleRemove,source);
    return released;
}
void damage_machine_fitting(Game& game,Entity& machine,int damage) {
    Item& fitting=machine.ground_item;
    if (!compatible(machine.kind) || fitting.kind!=ItemKind::NozzleElbow || damage<=0) return;
    fitting.durability-=damage;
    if (fitting.durability>0) return;
    fitting={};emit_sound(game,SoundId::NozzleBreak,machine.cell);
}
void drop_machine_fitting(Game& game,const Entity& machine) {
    if (!compatible(machine.kind) || machine.ground_item.kind==ItemKind::None) return;
    loosen(game,machine.ground_item,nearby_ground_item_cell(game,machine.cell));
}
bool valid_nozzle_elbow(const Item& item) {
    if (item.kind!=ItemKind::NozzleElbow) return true;
    Entity technician;technician.powers[static_cast<std::size_t>(ArtifactKind::Technical)]=item.technical_level;
    Item expected=make_item(item.kind,1,item.attribute);improve_pickup(technician,expected);
    return item.count==1 && item.max_count==1 && item.loaded>=0 && item.loaded<=1 && item.spare==0 &&
        item.durability>0 && item.durability<=item.max_durability &&
        item.max_durability==expected.max_durability &&
        (item.attribute==ItemAttribute::None || item.attribute==ItemAttribute::Durable);
}
bool valid_machine_fitting(const Entity& machine) {
    if (!compatible(machine.kind)) return true;
    const Item& fitting=machine.ground_item;
    return fitting.kind==ItemKind::None || (fitting.kind==ItemKind::NozzleElbow && valid_nozzle_elbow(fitting)) ||
        (machine.kind==EntityKind::BoilerTank && fitting.kind==ItemKind::PressureValve);
}
