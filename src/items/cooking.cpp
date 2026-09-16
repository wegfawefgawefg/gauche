#include "cooking.hpp"
#include "action.hpp"
#include "fire.hpp"
#include "../entities/attacks.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

ItemKind cooked_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::RawMeat: return ItemKind::CookedMeat;
    case ItemKind::Egg: return ItemKind::FriedEgg;
    default: return ItemKind::None;
    }
}

bool cooking_heat(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && !surface_wet(*tile) && flame_cell(game,cell);
}

// PLAYER SLOTS: label_b = quick_slots + ingredient slot + 1; ground_item recipe
// identity; point_b heat source; counter_a progress; counter_b one-shot/held/latch
// (0/1/2). Other item actions own these slots only outside a cooking action.
bool cooking_action(const Entity& actor) {
    return actor.kind==EntityKind::Player && actor.label_b>quick_slots && actor.label_b<=quick_slots*2;
}

namespace {
bool usable_fire(const Game& game,const Entity& player,Cell fire) {
    return distance(player.cell,fire)<=1 && cooking_heat(game,fire) &&
        clear_heat_sight(game,player.cell,fire);
}

bool begin(Game& game,Entity& player,Cell fire,int slot,bool held_use) {
    const Item& ingredient=player.inventory.slots[static_cast<std::size_t>(slot)];
    if (cooked_item(ingredient.kind)==ItemKind::None || ingredient.count<=0) return false;
    cancel_item_action(player);
    player.label_b=quick_slots+slot+1;player.counter_a=0;player.counter_b=held_use ? 1 : 0;
    player.point_b=fire;player.ground_item=ingredient;
    emit_sound(game,SoundId::CookingSizzle,fire);
    return true;
}

// Prepare a copy first: a full pack never consumes the raw portion.
bool cook_portion(Entity& player,int slot) {
    Inventory cooked=player.inventory;
    Item& ingredient=cooked.slots[static_cast<std::size_t>(slot)];
    const Item meal=make_item(cooked_item(ingredient.kind),1,ingredient.attribute);
    if (--ingredient.count==0) ingredient={};
    if (!insert_item(cooked,meal)) return false;
    player.inventory=cooked;
    player.use_flash=6;
    return true;
}
}

bool use_cooking_fire(Game& game,Entity& player,Cell fire,bool held_use) {
    if (!usable_fire(game,player,fire)) return false;
    if (cooking_action(player)) return true;
    if (player.timer_b>0) return true;
    if (light_stick(game,*player.inventory.held(),fire)) {player.timer_b=45;return true;}
    if (begin(game,player,fire,player.inventory.selected,held_use)) return true;
    for (int slot=0;slot<quick_slots;++slot)
        if (begin(game,player,fire,slot,held_use)) return true;
    return false;
}

bool step_cooking_action(Game& game,int slot,const Input& input) {
    Entity& player=game.entities[static_cast<std::size_t>(slot)];
    if (!cooking_action(player)) {
        if (!input.use || input.cancel_use || input.drop || input.interact ||
            cooked_item(player.inventory.held()->kind)==ItemKind::None) return false;
        // Food use has range zero for eating; roasting deliberately reaches the
        // adjacent flame the player faces, without heating the whole backpack.
        for (Cell fire:{player.cell+player.facing,player.cell})
            if (usable_fire(game,player,fire)) return begin(game,player,fire,player.inventory.selected,true);
        return false;
    }
    if (input.cancel_use || input.drop || input.pickup || input.interact || input.select>=0) {
        cancel_item_action(player);return true;
    }
    if (player.counter_b==2) {
        if (!input.use) cancel_item_action(player);
        return true; // Finishing/extinguishing never changes held cooking into eating.
    }
    if (player.counter_b==1 && !input.use) {cancel_item_action(player);return true;}
    const int ingredient_slot=player.label_b-quick_slots-1;
    const Item& ingredient=player.inventory.slots[static_cast<std::size_t>(ingredient_slot)];
    if (!usable_fire(game,player,player.point_b) || ingredient.kind!=player.ground_item.kind ||
        ingredient.attribute!=player.ground_item.attribute || ingredient.count<=0) {
        player.counter_b=2;return true;
    }
    player.counter_a=std::min(cooking_ticks,player.counter_a+1);
    if (player.counter_a<cooking_ticks || !cook_portion(player,ingredient_slot)) return true;
    const bool more=player.counter_b==1 && ingredient.kind==player.ground_item.kind && ingredient.count>0;
    player.counter_a=0;
    if (more) emit_sound(game,SoundId::CookingSizzle,player.point_b);
    else player.counter_b=2;
    return true;
}

// LOOSE FOOD: counter_c is heat progress, independent of float/sled fields.
// Pickups discard this partial cooking, so stacks cannot duplicate a partly cooked portion.
void step_ground_cooking(Game& game,int slot) {
    Entity& loose=game.entities[static_cast<std::size_t>(slot)];
    if (loose.kind!=EntityKind::GroundItem) return;
    Item& raw=loose.ground_item;
    if (raw.count<=0) return;
    const ItemKind result=cooked_item(raw.kind);
    if (result==ItemKind::None && raw.kind!=ItemKind::CookedMeat && raw.kind!=ItemKind::FriedEgg) return;
    if (game.stage.at_or_border(loose.cell).kind==TileKind::Lava) {
        emit_sound(game,SoundId::CookingSizzle,loose.cell);
        remove_entity(game,{slot,loose.generation});return;
    }
    if (result==ItemKind::None) {loose.counter_c=0;return;}
    if (!cooking_heat(game,loose.cell)) {loose.counter_c=0;return;}
    if (loose.counter_c==0) emit_sound(game,SoundId::CookingSizzle,loose.cell);
    loose.counter_c=std::min(cooking_ticks,loose.counter_c+1);
    if (loose.counter_c<cooking_ticks) return;
    const Item meal=make_item(result,1,raw.attribute);
    if (raw.count==1) {raw=meal;loose.sprite=item_sprite(meal);loose.counter_c=0;return;}
    // The output stays at the real fire. Never use shore/loot relocation here.
    for (Entity& other:game.entities) {
        if (other.kind!=EntityKind::GroundItem || other.cell!=loose.cell ||
            other.ground_item.kind!=result || other.ground_item.attribute!=raw.attribute ||
            other.ground_item.count>=other.ground_item.max_count) continue;
        ++other.ground_item.count;--raw.count;loose.counter_c=0;return;
    }
    if (Entity* output=get_entity(game,spawn_entity(game,EntityKind::GroundItem,loose.cell))) {
        output->ground_item=meal;output->sprite=item_sprite(meal);
        --raw.count;loose.counter_c=0;
    }
}

bool valid_cooking_state(const Entity& actor) {
    if (actor.kind==EntityKind::GroundItem && cooked_item(actor.ground_item.kind)!=ItemKind::None)
        return actor.counter_c>=0 && actor.counter_c<=cooking_ticks;
    if (actor.kind!=EntityKind::Player || actor.label_b<=quick_slots) return true;
    return cooking_action(actor) && cooked_item(actor.ground_item.kind)!=ItemKind::None &&
        actor.counter_a>=0 && actor.counter_a<=cooking_ticks && actor.counter_b>=0 && actor.counter_b<=2;
}
