#include "nail_board.hpp"
#include "../world/water.hpp"
#include "../items/sled.hpp"
#include "../item_pattern.hpp"
#include "../surfaces/temperature.hpp"
#include <algorithm>

namespace {
void break_board(Game& game,int slot) {
    const auto& trap=game.entities[static_cast<std::size_t>(slot)];const Cell cell=trap.cell;
    remove_entity(game,{slot,trap.generation});emit_sound(game,SoundId::NailBreak,cell);
}
void recover(Game& game,int slot) {
    Entity& trap=game.entities[static_cast<std::size_t>(slot)];
    Item item=trap.ground_item;item.durability=trap.health;item.cooldown=18;item.opened=false;
    const Cell cell=trap.cell;const auto generation=trap.generation;
    trap={};trap.kind=EntityKind::GroundItem;trap.generation=generation;trap.birth_tick=game.tick;
    trap.cell=cell;trap.ground_item=item;trap.sprite=Sprite::NailBoard;
}
void trigger(Game& game,int trap_slot,int actor_slot) {
    Entity& trap=game.entities[static_cast<std::size_t>(trap_slot)];
    const Entity& actor=game.entities[static_cast<std::size_t>(actor_slot)];
    if (!nail_board_trap(trap) || trap.timer_a>0 || trap.health<=0 || !wading_actor(actor) ||
        actor.hard_blocker || ridden_sled(game,actor)) return;
    const Cell cell=trap.cell;const Handle source=trap.entity_a;
    const int damage=16*item_pattern(trap.ground_item).damage/10;
    // Disarm before the hit: nested movement/damage cannot retrigger the same board.
    trap.health=std::max(0,trap.health-4);
    if (trap.health==0) break_board(game,trap_slot);else recover(game,trap_slot);
    damage_entity(game,actor_slot,damage,cell,false,source);
    emit_sound(game,SoundId::NailStep,cell);
}
}
bool nail_board_trap(const Entity& trap) {return trap.kind==EntityKind::Trap && trap.ground_item.kind==ItemKind::NailBoard;}
// Uses existing trap/item state: timer_a arms, entity_a exact placer; health is
// deployed condition. Recovery transfers that condition back to the same item.
void step_nail_board_trap(Game& game,int slot) {
    Entity& trap=game.entities[static_cast<std::size_t>(slot)];
    if (game.tick%30==0 && hot_cell(game,trap.cell)) trap.health=std::max(0,trap.health-5);
    if (trap.health==0) {break_board(game,slot);return;}
    const int actor=entity_at(game,trap.cell,true);
    if (actor>=0) trigger(game,slot,actor);
}
void enter_nail_boards(Game& game,int actor_slot) {
    for (int slot=0;slot<max_entities;++slot) {
        const auto& actor=game.entities[static_cast<std::size_t>(actor_slot)];
        const auto& trap=game.entities[static_cast<std::size_t>(slot)];
        if (actor.health<=0) return;
        if (nail_board_trap(trap) && trap.cell==actor.cell && trap.birth_tick!=game.tick) trigger(game,slot,actor_slot);
    }
}
bool hit_nail_boards(Game& game,Cell cell,int damage,Cell source) {
    bool hit=false;
    for (int slot=0;slot<max_entities;++slot) {
        Entity& trap=game.entities[static_cast<std::size_t>(slot)];
        if (!nail_board_trap(trap) || trap.cell!=cell || damage<=0) continue;
        damage_entity(game,slot,damage,source,false);hit=true;
        if (trap.health==0) break_board(game,slot);
    }
    return hit;
}
Item recoverable_nail_board(const Game& game,Cell cell) {
    for (const Entity& trap:game.entities) if (nail_board_trap(trap) && trap.cell==cell && trap.health>0) {
        Item item=trap.ground_item;item.durability=trap.health;item.cooldown=18;item.opened=false;return item;
    }
    return {};
}
int release_nail_board(Game& game,Cell cell) {
    for (int slot=0;slot<max_entities;++slot) {
        const auto& trap=game.entities[static_cast<std::size_t>(slot)];
        if (nail_board_trap(trap) && trap.cell==cell && trap.health>0) {recover(game,slot);return slot;}
    }
    return -1;
}
bool valid_nail_board(const Entity& trap) {
    return !nail_board_trap(trap) || (trap.health<=trap.max_health && trap.max_health==trap.ground_item.max_durability &&
        trap.max_health>0 && trap.timer_a<=18 && trap.ground_item.count==1 && trap.ground_item.flight.slot<0 &&
        !trap.impassable && !trap.hard_blocker && trap.move_interval==0);
}
