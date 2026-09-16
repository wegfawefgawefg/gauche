#include "bear_fishing.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../projectiles/fishing.hpp"
#include "../items/sled.hpp"
#include "../world/ground_items.hpp"
#include "../world/floating_items.hpp"
#include "../surfaces/interaction.hpp"

namespace {
bool available_fish(const Game& game,Handle handle) {
    const Entity* fish=get_entity(game,handle);
    if (!fish || fish->kind!=EntityKind::GroundItem || fish->ground_item.kind!=ItemKind::RiverFish ||
        fish->ground_item.count<=0 || fish->ground_item.flight.slot>=0 || sled_cargo(game,*fish) ||
        !surface_wet(game.stage.at_or_border(fish->cell))) return false;
    for (const Entity& hook:game.entities)
        if (hook.kind==EntityKind::Projectile && hook.label_a==static_cast<int>(ProjectileKind::FishingHook) &&
            hook.label_b==FishingCargo && hook.entity_b==handle) return false;
    return true;
}
bool clear_bank(const Game& game,const Entity& bear) {
    const auto& tile=game.stage.at_or_border(bear.cell);
    if (!walkable(tile) || surface_wet(tile)) return false;
    for (const Entity& other:game.entities)
        if (&other!=&bear && other.kind!=EntityKind::None && other.cell==bear.cell &&
            (other.impassable || other.kind==EntityKind::GroundItem || other.kind==EntityKind::Coins)) return false;
    return true;
}
}

// counter_a: 1 bank fisher; point_a: dry stand; entity_b + point_b: committed
// fish handle/cell. label_a 3/4 paw/follow-through; timer_a: work beat.
// Foraging retains b-slots; hearing retains c-slots. No hidden renewable stock.
void start_bear_fishing(Entity& bear) {
    bear.counter_a=1;bear.point_a=bear.cell;bear.timer_a=12;
    bear.sprite=Sprite::BearFishWatch;
}
void interrupt_bear_fishing(Entity& bear) {
    if (bear.kind!=EntityKind::Bear || bear.counter_a!=1) return;
    if (bear.label_a>=BearFishPaw) {bear.label_a=0;bear.timer_a=120;}
    bear.entity_b={};bear.sprite=Sprite::Bear;
}
bool step_bear_fishing(Game& game,int slot) {
    Entity& bear=game.entities[static_cast<std::size_t>(slot)];
    if (bear.counter_a!=1 || bear.timer_b>0) return false;
    const auto target=enemy_target(game,bear.cell,3);
    if (target && clear_attack_sight(game,bear.cell,target->cell)) {
        interrupt_bear_fishing(bear);return false;
    }
    if (bear.cell!=bear.point_a) {
        interrupt_bear_fishing(bear);
        if (bear.move_wait==0) pursue(game,slot,bear.point_a);
        return true;
    }
    if (bear.label_a==BearFishFollow) {
        if (bear.timer_a==0) {bear.label_a=0;bear.timer_a=180;bear.sprite=Sprite::Bear;}
        return true;
    }
    if (bear.label_a==BearFishPaw) {
        if (bear.timer_a>0) return true;
        bool caught=false;
        Entity* fish=get_entity(game,bear.entity_b);
        if (available_fish(game,bear.entity_b) && fish->cell==bear.point_b &&
            distance(bear.cell,fish->cell)==1 && clear_bank(game,bear)) {
            // Split one portion only after a real landing slot is available.
            if (fish->ground_item.count>1) {
                Entity* landed=get_entity(game,spawn_entity(game,EntityKind::GroundItem,bear.cell));
                if (landed) {
                    landed->ground_item=fish->ground_item;landed->ground_item.count=1;
                    landed->sprite=fish->sprite;--fish->ground_item.count;caught=true;
                }
            } else {stop_item_float(game,*fish);fish->cell=bear.cell;caught=true;}
            if (caught) emit_sound(game,SoundId::CurrentDrift,bear.point_b);
        }
        bear.entity_b={};bear.label_a=caught ? BearFishFollow : 0;
        bear.timer_a=caught ? 18 : 120;
        bear.sprite=caught ? Sprite::BearFishSwat : Sprite::BearFishWatch;return true;
    }
    bear.sprite=Sprite::BearFishWatch;
    if (bear.timer_a>0 || bear.counter_b>0) return true;
    if (!clear_bank(game,bear)) {bear.timer_a=120;return true;}
    for (int i=0;i<max_entities;++i) {
        const auto& fish=game.entities[static_cast<std::size_t>(i)];
        const Handle handle{i,fish.generation};
        if (distance(bear.cell,fish.cell)!=1 || !available_fish(game,handle)) continue;
        bear.entity_b=handle;bear.point_b=fish.cell;bear.facing=fish.cell-bear.cell;
        bear.label_a=BearFishPaw;bear.timer_a=36;bear.sprite=Sprite::BearFishPaw;
        return true;
    }
    // No catch remains: return to ordinary territorial wandering and foraging.
    bear.sprite=Sprite::Bear;bear.timer_a=180;return false;
}
bool valid_bear(const Entity& bear) {
    return bear.kind!=EntityKind::Bear || (bear.counter_a>=0 && bear.counter_a<=1 &&
        bear.label_a>=0 && bear.label_a<=BearFishFollow &&
        (bear.label_a<BearFishPaw || bear.counter_a==1));
}
