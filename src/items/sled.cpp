#include "../entities/river_raft.hpp"
#include "sled.hpp"
#include "../world/water.hpp"
#include "../world/floating_items.hpp"
#include "../surfaces/interaction.hpp"
#include "../entities/echo_hound.hpp"

namespace {
constexpr RegionalItem kit{"Sled",
    "Deploy; step aboard. Carries one item. Steer when stopped. Secondary dismounts. Pick up empty stopped sleds.",
    Sprite::Sled,{1,1,0,0,30,PatternEffect::Utility},ItemAction::Material,
    25,1,true,0,0,0,0,0,SoundId::SledDeploy,45};

Handle handle_of(const Game& game,const Entity& entity) {
    return {static_cast<int>(&entity-game.entities.data()),entity.generation};
}
void stop_sled(Game& game,Entity& sled) {
    if (sled.label_a!=0) emit_sound(game,SoundId::SledBrake,sled.cell);
    sled.label_a=0; sled.timer_b=0;
}
bool sled_ground(const Tile* tile) {
    return tile && walkable(*tile) && tile->kind!=TileKind::Water && tile->kind!=TileKind::Lava;
}
int travel_beat(const Tile& tile) {
    return (tile.kind==TileKind::Snow || tile.kind==TileKind::Ice ? 4 : 8)+surface_step_delay(tile);
}
// A second stack cannot share the loaded landing. Empty runners can scoop one up.
bool free_landing(const Game& game,const Entity& sled,Cell cell) {
    if (!sled_ground(game.stage.at(cell))) return false;
    for (const Entity& other : game.entities) {
        if (&other==&sled || other.kind==EntityKind::None || other.cell!=cell) continue;
        if (other.impassable || other.kind==EntityKind::Sled) return false;
        if (other.kind==EntityKind::GroundItem && (sled.entity_b.slot>=0 ||
            other.ground_item.flight.slot>=0 || sled_cargo(game,other))) return false;
    }
    return true;
}
void load_cargo(Game& game,Entity& sled) {
    if (sled.entity_b.slot>=0) return;
    for (Entity& cargo : game.entities) {
        if (cargo.kind!=EntityKind::GroundItem || cargo.cell!=sled.cell || cargo.ground_item.count<=0 ||
            cargo.ground_item.flight.slot>=0 || sled_cargo(game,cargo) || ridden_river_raft(game,cargo)) continue;
        stop_item_float(game,cargo);
        cargo.label_a=2; cargo.entity_a=handle_of(game,sled);
        sled.entity_b=handle_of(game,cargo);
        return;
    }
}
void sync_passengers(Game& game,Entity& sled) {
    Entity* rider=get_entity(game,sled.entity_a);
    if (!rider || rider->kind!=EntityKind::Player || rider->health<=0 || rider->cell!=sled.cell ||
        (rider->owner>=0 && !game.run.online[static_cast<std::size_t>(rider->owner)])) sled.entity_a={};
    Entity* cargo=get_entity(game,sled.entity_b);
    if (!cargo || !sled_cargo(game,*cargo)) {
        if (cargo && cargo->kind==EntityKind::GroundItem && cargo->label_a==2 &&
            cargo->entity_a==handle_of(game,sled)) { cargo->label_a=0; cargo->entity_a={}; }
        sled.entity_b={};
    }
}
}

const RegionalItem* sled_item(ItemKind kind) { return kind==ItemKind::Sled ? &kit : nullptr; }

// SLED SLOTS: entity_a rider, entity_b cargo; point_a expected cell detects pushes.
// label_a 0 stopped / 1 coasting, timer_b movement beat, facing fixed heading.
// ground_item preserves the packed item/attribute, with health as deployed wear.
// CARGO SLOTS: GroundItem label_a=2, entity_a reciprocal sled generation handle.
void init_sled(Entity& sled) {
    sled.sprite=Sprite::Sled;
    sled.health=sled.max_health=45;
    sled.point_a=sled.cell;
    sled.ground_item=make_item(ItemKind::Sled);
}
bool place_sled(Game& game,int user_slot,Cell direction,const Item& item) {
    if (distance({},direction)!=1 || item.durability<=0) return false;
    const Cell cell=game.entities[static_cast<std::size_t>(user_slot)].cell+direction;
    const Tile* tile=game.stage.at(cell);
    Entity probe;
    if (!free_landing(game,probe,cell) || !tile || surface_wet(*tile) || tile->surface.fire_ticks>0 ||
        (tile->prop.kind!=PropKind::None && !tile->prop.broken)) return false;
    for (const Entity& other : game.entities)
        if (other.kind!=EntityKind::None && other.kind!=EntityKind::GroundItem && other.cell==cell) return false;
    Entity* sled=get_entity(game,spawn_entity(game,EntityKind::Sled,cell));
    if (!sled) return false;
    sled->facing=direction; sled->ground_item=item;
    sled->ground_item.cooldown=0;
    sled->health=item.durability; sled->max_health=item.max_durability;
    load_cargo(game,*sled);
    return true;
}
const Entity* ridden_sled(const Game& game,const Entity& player) {
    if (player.kind!=EntityKind::Player || player.health<=0) return nullptr;
    const Handle rider=handle_of(game,player);
    for (const Entity& sled : game.entities)
        if (sled.kind==EntityKind::Sled && sled.health>0 && sled.entity_a==rider && sled.cell==player.cell)
            return &sled;
    return nullptr;
}
bool sled_cargo(const Game& game,const Entity& cargo) {
    if (cargo.kind!=EntityKind::GroundItem || cargo.label_a!=2 || cargo.ground_item.flight.slot>=0) return false;
    const Entity* sled=get_entity(game,cargo.entity_a);
    return sled && sled->kind==EntityKind::Sled && sled->health>0 && sled->cell==cargo.cell &&
        sled->entity_b==handle_of(game,cargo);
}
void step_sled_cargo(Game& game,int slot) {
    Entity& cargo=game.entities[static_cast<std::size_t>(slot)];
    if (cargo.kind==EntityKind::GroundItem && cargo.label_a==2 && !sled_cargo(game,cargo)) {
        cargo.label_a=0; cargo.entity_a={};
    }
}
void clear_sled_links(Game& game,Entity& sled) {
    if (sled.kind!=EntityKind::Sled) return;
    Entity* cargo=get_entity(game,sled.entity_b);
    if (cargo && cargo->kind==EntityKind::GroundItem && cargo->label_a==2 &&
        cargo->entity_a==handle_of(game,sled)) { cargo->label_a=0; cargo->entity_a={}; }
    sled.entity_a=sled.entity_b={};
}
void board_sled(Game& game,int player_slot) {
    Entity& player=game.entities[static_cast<std::size_t>(player_slot)];
    if (player.health<=0 || ridden_sled(game,player) || ridden_river_raft(game,player)) return;
    for (Entity& sled : game.entities) {
        if (sled.kind!=EntityKind::Sled || sled.health<=0 || sled.cell!=player.cell) continue;
        sync_passengers(game,sled);
        if (sled.entity_a.slot>=0) continue;
        sled.entity_a={player_slot,player.generation};
        sled.point_a=sled.cell;
        sled.label_a=1; sled.timer_b=travel_beat(game.stage.at_or_border(sled.cell));
        emit_sound(game,SoundId::SledBoard,sled.cell);
        return;
    }
}
void sled_player_input(Game& game,int player_slot,Input& input) {
    Entity& player=game.entities[static_cast<std::size_t>(player_slot)];
    const Entity* found=ridden_sled(game,player);
    if (!found) return;
    Entity& sled=game.entities[static_cast<std::size_t>(handle_of(game,*found).slot)];
    if (input.reload) {
        stop_sled(game,sled); sled.entity_a={};
        input.move={}; input.reload=false;
        return;
    }
    if (input.move!=Cell{}) {
        const Cell direction=input.move.x!=0 ? Cell{input.move.x,0} : Cell{0,input.move.y};
        player.facing=direction;
        if (sled.label_a==0 && player.vitals.rooted==0 &&
            game.stage.at_or_border(sled.cell).surface.still_ticks==0 &&
            free_landing(game,sled,sled.cell+direction)) {
            sled.facing=direction; sled.label_a=1;
            sled.timer_b=travel_beat(game.stage.at_or_border(sled.cell));
        }
    }
    input.move={}; // Aim and item actions remain independent of the runners' heading.
}
void sled_contact(Game& game,int slot) {
    Entity& sled=game.entities[static_cast<std::size_t>(slot)];
    if (sled.kind!=EntityKind::Sled || sled.health<=0) return;
    const Tile* tile=game.stage.at(sled.cell);
    if (!tile) return;
    if (surface_wet(*tile)) { sled.burn_ticks=sled.scorch_ticks=0; return; }
    bool flame=tile->surface.fire_ticks>0;
    for (const Entity& fire : game.entities)
        if (fire.kind==EntityKind::Campfire && fire.fire_tramples<5 && fire.cell==sled.cell) flame=true;
    if (flame) {
        if (sled.scorch_ticks==0) emit_sound(game,SoundId::SledBurn,sled.cell);
        sled.scorch_ticks=300;
    }
    if (sled.scorch_ticks>0 || sled.burn_ticks>0) ignite_surface(game,sled.cell);
}
void step_sled(Game& game,int slot) {
    Entity& sled=game.entities[static_cast<std::size_t>(slot)];
    if (sled.health<=0) { clear_sled_links(game,sled); return; }
    if (!sled_ground(game.stage.at(sled.cell))) {
        clear_sled_links(game,sled); stop_sled(game,sled); return;
    }
    sync_passengers(game,sled);
    // EXTERNAL PUSH: Leave passengers where the shove actually left them.
    if (sled.point_a!=sled.cell) {
        clear_sled_links(game,sled); stop_sled(game,sled); sled.point_a=sled.cell;
    }
    load_cargo(game,sled);
    Entity* rider=get_entity(game,sled.entity_a);
    const Tile& floor=game.stage.at_or_border(sled.cell);
    if (floor.surface.still_ticks>0 || (rider && rider->vitals.rooted>0)) stop_sled(game,sled);
    if (sled.label_a==0 || sled.timer_b>0) return;
    const Cell next=sled.cell+sled.facing;
    if (!free_landing(game,sled,next)) { stop_sled(game,sled); return; }
    Entity* cargo=get_entity(game,sled.entity_b);
    // COMMIT: All real cells move together before any landing event can inspect them.
    sled.cell=sled.point_a=next;
    if (cargo) cargo->cell=next;
    if (rider) {
        rider->cell=next; rider->vitals.slide_momentum=0;
        enter_actor_cell(game,sled.entity_a.slot);
    }
    if (sled.kind!=EntityKind::Sled || sled.health<=0) return;
    sync_passengers(game,sled); load_cargo(game,sled);
    sled_contact(game,slot);
    const Tile& landing=game.stage.at_or_border(next);
    sled.timer_b=travel_beat(landing);
    emit_sound(game,SoundId::SledSlide,next);
    hear_echo_hounds(game,next,landing.kind==TileKind::Snow ? 2 : 5);
    if (shallow_water(landing.kind) || landing.surface.still_ticks>0 ||
        (rider && (rider->health<=0 || rider->cell!=next || rider->vitals.rooted>0))) stop_sled(game,sled);
}
bool haul_sled(Game& game,int slot,Cell direction) {
    Entity& sled=game.entities[static_cast<std::size_t>(slot)];
    if (sled.kind!=EntityKind::Sled || sled.health<=0 || sled.toss.ticks || distance({},direction)!=1) return false;
    sync_passengers(game,sled);
    Entity* rider=get_entity(game,sled.entity_a);
    const Cell next=sled.cell+direction;
    if (!free_landing(game,sled,next) || (rider && (rider->vitals.rooted || rider->vitals.grip))) return false;
    stop_sled(game,sled);
    Entity* cargo=get_entity(game,sled.entity_b);
    // Preserve the reciprocal passenger handles before applying landing hazards.
    sled.cell=sled.point_a=next;
    if (cargo) cargo->cell=next;
    if (rider) {rider->cell=next;rider->vitals.slide_momentum=0;enter_actor_cell(game,sled.entity_a.slot);}
    if (sled.kind==EntityKind::Sled && sled.health>0) {sync_passengers(game,sled);load_cargo(game,sled);sled_contact(game,slot);}
    return true;
}
Item recoverable_sled(const Game& game,Cell cell) {
    for (const Entity& sled : game.entities) {
        if (sled.kind!=EntityKind::Sled || sled.cell!=cell || sled.health<=0 || sled.label_a!=0 ||
            sled.scorch_ticks>0 || sled.burn_ticks>0) continue;
        const Entity* rider=get_entity(game,sled.entity_a);
        if (rider && ridden_sled(game,*rider)==&sled) continue;
        bool cargo=false;
        for (const Entity& other : game.entities)
            if (other.kind==EntityKind::GroundItem && other.cell==cell) cargo=true;
        if (cargo) continue;
        Item item=sled.ground_item; item.durability=sled.health;
        return item;
    }
    return {};
}
int release_sled(Game& game,Cell cell) {
    const Item item=recoverable_sled(game,cell);
    if (item.kind==ItemKind::None) return -1;
    for (Entity& sled : game.entities) {
        if (sled.kind!=EntityKind::Sled || sled.cell!=cell || sled.health<=0) continue;
        const Handle handle=handle_of(game,sled);
        clear_sled_links(game,sled);
        sled={}; sled.kind=EntityKind::GroundItem; sled.cell=cell;
        sled.generation=handle.generation; sled.birth_tick=game.tick;
        sled.ground_item=item; sled.sprite=Sprite::Sled;
        return handle.slot;
    }
    return -1;
}
bool hit_sled(Game& game,Cell cell,int damage,Cell source) {
    bool hit=false;
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& sled=game.entities[static_cast<std::size_t>(slot)];
        if (sled.kind!=EntityKind::Sled || sled.health<=0 || sled.cell!=cell || damage<=0) continue;
        damage_entity(game,slot,damage,source); hit=true;
    }
    return hit;
}
bool valid_sled(const Entity& sled) {
    return sled.kind!=EntityKind::Sled || (sled.label_a>=0 && sled.label_a<=1 &&
        sled.timer_b>=0 && sled.timer_b<=16 && distance({},sled.facing)==1 &&
        sled.health<=sled.max_health && (sled.max_health==45 || sled.max_health==90) &&
        sled.ground_item.kind==ItemKind::Sled && sled.ground_item.count==1 &&
        sled.ground_item.max_durability==sled.max_health && !sled.impassable && !sled.hard_blocker &&
        sled.move_interval==0);
}
