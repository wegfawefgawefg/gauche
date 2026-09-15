#include "../items/machine_fittings.hpp"
#include "emergency_pump.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "hearing.hpp"
#include "../surfaces/liquid_transfer.hpp"
#include "../items/pocket_pump.hpp"
#include "../items/sled.hpp"
#include "../combat/shove.hpp"
#include "../world/ground_items.hpp"
#include <algorithm>
#include <vector>

namespace {
void recover(Entity& actor) {actor.label_a=PumpRecover;actor.timer_a=72;actor.label_b=0;}
bool open_nozzle(const Game& game,Cell source,Cell cell) {
    if (!clear_sight(game,source,cell,false)) return false;
    const int blocker=entity_at(game,cell,true);
    if (blocker<0) return true;
    const Entity& target=game.entities[static_cast<std::size_t>(blocker)];
    // Wet the near face of machinery, but never pass through it or a shut door.
    return !target.hard_blocker || (target.health>0 && target.kind!=EntityKind::Door && target.kind!=EntityKind::EncounterGate);
}
std::optional<Cell> find_spill(const Game& game,const Entity& actor) {
    std::optional<Cell> best;int nearest=6;
    for (int y=-5;y<=5;++y) for (int x=-5;x<=5;++x) {
        const Cell cell=actor.cell+Cell{x,y};const int range=distance(actor.cell,cell);
        const Tile* tile=game.stage.at(cell);
        if (range>=nearest || !tile || !pumpable_spill(*tile) || !walkable(*tile) ||
            !clear_sight(game,actor.cell,cell,false)) continue;
        const int blocker=entity_at(game,cell,true);
        if (blocker>=0 && &game.entities[static_cast<std::size_t>(blocker)]!=&actor) continue;
        best=cell;nearest=range;
    }
    return best;
}
void spray(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    const Cell source=actor.point_a,direction=actor.point_b;
    const Handle owner{slot,actor.generation};
    struct Contact {Handle target;Cell cell;int lane;};
    std::vector<Contact> contacts;
    // Capture before dousing or deaths change the cells/occupants. Each victim
    // receives one pressure hit even if a shove puts it in the next wet cell.
    for (int reach=3;reach>=1;--reach) {
        const Cell cell=source+Cell{direction.x*reach,direction.y*reach};
        for (int i=0;i<max_entities;++i) {
            const Entity& other=game.entities[static_cast<std::size_t>(i)];
            if (i!=slot && other.kind!=EntityKind::None && other.health>0 && other.cell==cell)
                contacts.push_back({{i,other.generation},cell,reach-1});
        }
    }
    recover(actor);
    std::array<int,3> amounts{};
    for (int reach=1;reach<=3 && actor.counter_b>0;++reach) {
        const Cell cell=source+Cell{direction.x*reach,direction.y*reach};
        if (!open_nozzle(game,source,cell)) break;
        const int spent=discharge_spill(game,cell,{static_cast<LiquidKind>(actor.counter_a),std::min(600,actor.counter_b)});
        if (!spent) break;
        actor.counter_b-=spent;actor.label_b=reach;
        amounts[static_cast<std::size_t>(reach-1)]=spent;
    }
    for (const Contact& contact:contacts) {
        Entity* target=get_entity(game,contact.target);
        const int amount=amounts[static_cast<std::size_t>(contact.lane)];
        if (!amount || !target || target->cell!=contact.cell || target->health<=0) continue;
        const int health=target->health;
        damage_entity(game,contact.target.slot,std::max(1,amount/100),source,true,owner);
        target=get_entity(game,contact.target);
        if (!target || target->health<=0 || target->health==health || amount<300 ||
            target->cell!=contact.cell || target->hard_blocker || target->toss.ticks ||
            target->vitals.rooted || target->vitals.grip || ridden_sled(game,*target)) continue;
        const Cell landing=target->cell+direction;const Tile* tile=game.stage.at(landing);
        // Pressure pushes, never invokes shove_actor's wall-crush shortcut.
        if (tile && walkable(*tile) && entity_at(game,landing,true)<0)
            shove_actor(game,contact.target.slot,direction,source);
    }
    if (actor.label_b) emit_sound(game,SoundId::PumpJet,source);
}
}
// SLOTS: label_a phase; timer_a fill/warning/recovery; timer_b idle scan delay.
// counter_a liquid kind, counter_b exact tank quantity (600 = one full pour).
// point_a committed origin; point_b source while filling, direction while firing.
// label_b last jet length for its brief cosmetic stream. c slots retain hearing.
void init_emergency_pump(Entity& actor) {
    actor.health=actor.max_health=100;actor.move_interval=24;actor.impassable=true;
    actor.sprite=Sprite::EmergencyPump;actor.counter_a=static_cast<int>(LiquidKind::Water);
    actor.counter_b=pump_capacity;
}
void interrupt_emergency_pump(Entity& actor) {
    if (actor.kind==EntityKind::EmergencyPump && (actor.label_a==PumpFill || actor.label_a==PumpWarn)) recover(actor);
}
void step_emergency_pump(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health<=0) return;
    if (actor.freeze_ticks || actor.stun_ticks || actor.sleep_ticks || actor.toss.ticks || actor.vitals.rooted) {
        interrupt_emergency_pump(actor);return;
    }
    if ((actor.label_a==PumpFill || actor.label_a==PumpWarn) && actor.cell!=actor.point_a) {
        interrupt_emergency_pump(actor);return;
    }
    if (actor.label_a==PumpRecover) {
        if (!actor.timer_a) {actor.label_a=PumpRoam;actor.label_b=0;}
        return;
    }
    if (actor.label_a==PumpFill) {
        if (actor.timer_a) return;
        if (!open_nozzle(game,actor.cell,actor.point_b)) {recover(actor);return;}
        const LiquidLoad load=collect_spill(game,actor.point_b,LiquidKind::None,pump_capacity);
        actor.counter_a=static_cast<int>(load.kind);actor.counter_b=load.amount;
        recover(actor);return;
    }
    if (actor.label_a==PumpWarn) {if (!actor.timer_a) spray(game,slot);return;}
    if (!actor.counter_b) {
        if (actor.timer_b) return;
        actor.timer_b=24;
        const auto spill=find_spill(game,actor);
        if (!spill) {if (!step_hearing(game,slot)) wander(game,slot);return;}
        if (distance(actor.cell,*spill)>1) {approach(game,slot,*spill);return;}
        actor.point_a=actor.cell;actor.point_b=*spill;actor.label_a=PumpFill;actor.timer_a=60;
        actor.facing=cardinal_toward(actor.cell,*spill,actor.facing);
        emit_sound(game,SoundId::PumpSlurp,actor.cell);return;
    }
    const auto target=enemy_target(game,actor.cell,8);
    if (!target || !clear_sight(game,actor.cell,target->cell)) {
        if (!step_hearing(game,slot)) wander(game,slot);
        return;
    }
    const Cell delta=target->cell-actor.cell;const int range=distance(actor.cell,target->cell);
    if (range>0 && range<=3 && (delta.x==0 || delta.y==0) && open_nozzle(game,actor.cell,target->cell)) {
        actor.point_a=actor.cell;actor.facing=cardinal_toward(actor.cell,target->cell,actor.facing);
        actor.point_b=outlet_direction(actor,actor.facing);actor.label_a=PumpWarn;actor.timer_a=36;
        emit_sound(game,SoundId::PumpPressure,actor.cell);return;
    }
    approach(game,slot,target->cell);
}
bool valid_emergency_pump(const Entity& actor) {
    if (actor.kind!=EntityKind::EmergencyPump) return true;
    if (actor.label_a<PumpRoam || actor.label_a>PumpRecover || actor.label_b<0 || actor.label_b>3 ||
        actor.counter_a<0 || actor.counter_a>=static_cast<int>(LiquidKind::Count) ||
        actor.counter_b<0 || actor.counter_b>pump_capacity || actor.timer_a<0 || actor.timer_b<0 || actor.timer_b>24) return false;
    constexpr int limits[]{0,60,36,72};
    if (actor.timer_a>limits[actor.label_a] || (actor.counter_a!=0 && !pumpable_liquid(static_cast<LiquidKind>(actor.counter_a))) ||
        (actor.counter_b>0 && actor.counter_a==0)) return false;
    if (actor.label_a==PumpFill && (actor.counter_b!=0 || distance(actor.point_a,actor.point_b)>1)) return false;
    if (actor.label_a==PumpWarn && (actor.counter_b==0 || distance({},actor.point_b)!=1)) return false;
    return actor.label_b==0 || (actor.label_a==PumpRecover && distance({},actor.point_b)==1);
}
void drop_emergency_pump(Game& game,const Entity& actor) {
    const auto roll=random_u32(game)%100;
    if (roll<25) {
        const Cell cell=nearby_ground_item_cell(game,actor.cell);
        Entity* loot=get_entity(game,spawn_entity(game,EntityKind::GroundItem,cell));
        if (!loot) return;
        loot->ground_item=make_item(ItemKind::PocketPump);
        loot->ground_item.loaded=actor.counter_b ? actor.counter_a : 0;
        loot->ground_item.spare=actor.counter_b;loot->sprite=Sprite::PocketPump;
    } else if (roll<40) place_ground_item(game,actor.cell,ItemKind::NozzleElbow);
    else if (roll<55) place_ground_item(game,actor.cell,ItemKind::CoolantCan);
}
