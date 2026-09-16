#include "fishing_work.hpp"
#include "fishing_widow.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../projectiles/fishing.hpp"
#include "../world/floating_items.hpp"
#include "../world/water.hpp"
#include "../items/sled.hpp"

namespace {
void wait_for_catch(Entity& widow,int ticks=120) {
    widow.label_a=WidowWorkWait;widow.timer_a=ticks;widow.entity_a={};
    widow.sprite=Sprite::FishingWidow;
}
bool clear_feet(Game& game,Entity& widow) {
    for (Entity& cargo:game.entities) {
        if (cargo.kind!=EntityKind::GroundItem || cargo.cell!=widow.cell) continue;
        if (cargo.ground_item.flight.slot>=0 || sled_cargo(game,cargo)) return false;
        const Cell side{-widow.facing.y,widow.facing.x};
        for (Cell offset:{Cell{-widow.facing.x,-widow.facing.y},side,Cell{-side.x,-side.y}}) {
            const Cell cell=widow.cell+offset;
            const Tile& tile=game.stage.at_or_border(cell);
            if (shallow_water(tile.kind) || tile.kind==TileKind::Water ||
                !float_cell_free(game,cell,static_cast<int>(&cargo-game.entities.data()))) continue;
            stop_item_float(game,cargo);cargo.cell=cell;
            emit_sound(game,SoundId::FishingFinish,cell);return true;
        }
        return false; // A packed bank pauses work; no teleport or item copy.
    }
    return true;
}
}
// WORK SLOTS: label_b 0 ordinary hunter / 1 bank worker / 2 disturbed worker;
// point_b dry stand, point_a committed casting origin; entity_a actual hook.
// counter_b saved casting direction (east/south/west/north), independent of steps.
// The carried Fishing Line has real charges and drops with its remaining uses.
// c-slots remain hearing-owned. Work never creates fish or replacement supplies.
void start_fishing_work(Entity& widow,Cell facing) {
    widow.label_b=1;widow.point_b=widow.cell;widow.facing=facing;
    widow.counter_b=facing.x>0 ? 0 : facing.y>0 ? 1 : facing.x<0 ? 2 : 3;
    widow.inventory.slots[0]=make_item(ItemKind::FishingLine);widow.inventory.selected=0;
    wait_for_catch(widow,24);
}
void rouse_fishing_worker(Game& game,Entity& widow) {
    if (widow.kind!=EntityKind::FishingWidow || widow.label_b!=1) return;
    const Entity* hook=get_entity(game,widow.entity_a);
    if (hook && hook->kind==EntityKind::Projectile && hook->label_a==static_cast<int>(ProjectileKind::FishingHook) &&
        get_entity(game,hook->entity_a)==&widow) remove_entity(game,widow.entity_a);
    widow.entity_a={};widow.label_b=2;widow.label_a=WidowUntangle;widow.timer_a=90;
    widow.sprite=Sprite::WidowUntangle;
    emit_sound(game,SoundId::WidowTangle,widow.cell);
}
bool step_fishing_work(Game& game,int slot) {
    Entity& widow=game.entities[static_cast<std::size_t>(slot)];
    if (widow.label_b!=1) return false;
    const int nearby=nearest_player(game,widow.cell,2);
    if (nearby>=0 && clear_attack_sight(game,widow.cell,game.entities[static_cast<std::size_t>(nearby)].cell)) {
        rouse_fishing_worker(game,widow);return true;
    }
    if (widow.cell!=widow.point_b) {
        wait_for_catch(widow);
        if (widow.move_wait==0) {
            const auto next=next_route_cell(game,slot,widow.point_b,1024);
            if (next) willing_step(game,slot,*next);
            else widow.move_wait=widow.move_interval;
        }
        return true;
    }
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    widow.facing=directions[widow.counter_b];
    if (widow.label_a==WidowWorkLine) {
        if (!get_entity(game,widow.entity_a)) {
            bool landed=false;
            for (const Entity& cargo:game.entities)
                if (cargo.kind==EntityKind::GroundItem && cargo.cell==widow.cell) landed=true;
            // An empty cast gets a patient wait, not a rapid loop burning up
            // the rod before the player ever reaches this part of the stream.
            wait_for_catch(widow,landed ? 36 : 600);
        }
        return true;
    }
    if (widow.timer_a>0) return true;
    Item& line=*widow.inventory.held();
    if (!clear_feet(game,widow) || line.kind!=ItemKind::FishingLine || line.uses<=0) {
        wait_for_catch(widow);return true;
    }
    if (widow.label_a!=WidowWorkWindup) {
        widow.label_a=WidowWorkWindup;widow.timer_a=36;widow.point_a=widow.cell;
        widow.sprite=Sprite::WidowWorkCast;return true;
    }
    if (launch_fishing_hook(game,slot,line,widow.facing)) {
        const Handle owner{slot,widow.generation};
        for (int i=0;i<max_entities;++i) {
            const Entity& hook=game.entities[static_cast<std::size_t>(i)];
            if (hook.kind==EntityKind::Projectile && hook.label_a==static_cast<int>(ProjectileKind::FishingHook) && hook.entity_a==owner) {
                widow.entity_a={i,hook.generation};break;
            }
        }
        --line.uses;widow.label_a=WidowWorkLine;widow.sprite=Sprite::WidowWorkReel;
        emit_sound(game,SoundId::FishingCast,widow.cell);
    } else wait_for_catch(widow);
    return true;
}
bool valid_fishing_widow(const Entity& widow) {
    if (widow.kind!=EntityKind::FishingWidow) return true;
    return widow.label_b>=0 && widow.label_b<=2 && widow.label_a>=WidowHunt &&
        widow.label_a<=WidowWorkWait && widow.timer_a>=0 && widow.timer_a<=600 &&
        (widow.label_b==0 || (widow.counter_b>=0 && widow.counter_b<4)) &&
        (widow.label_a<WidowWorkWindup || widow.label_b==1);
}
