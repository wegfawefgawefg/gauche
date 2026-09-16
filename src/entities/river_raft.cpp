#include "river_raft.hpp"
#include "../items/sled.hpp"
#include "../world/currents.hpp"
#include "../world/floating_items.hpp"
#include "../world/water.hpp"

namespace {
Handle handle_of(const Game& game,const Entity& actor) {
    return {static_cast<int>(&actor-game.entities.data()),actor.generation};
}
bool afloat(const Game& game,const Entity& raft) {
    const auto& tile=game.stage.at_or_border(raft.cell);
    return raft.kind==EntityKind::RiverRaft && raft.health>0 && !raft.toss.ticks &&
        river_water(tile.kind) && !prop_blocks(tile.prop);
}
void sync_passengers(Game& game,Entity& raft) {
    if (const auto* rider=get_entity(game,raft.entity_a);!rider || ridden_river_raft(game,*rider)!=&raft) raft.entity_a={};
    if (const auto* cargo=get_entity(game,raft.entity_b);!cargo || ridden_river_raft(game,*cargo)!=&raft) raft.entity_b={};
}
bool landing_free(const Game& game,const Entity& raft,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || !river_water(tile->kind) || prop_blocks(tile->prop)) return false;
    for (const auto& other:game.entities) {
        if (&other==&raft || other.kind==EntityKind::None || other.cell!=cell) continue;
        if (other.impassable || other.kind==EntityKind::RiverRaft || other.kind==EntityKind::Sled) return false;
        if (other.kind==EntityKind::GroundItem && raft.entity_b.slot>=0) return false;
    }
    return true;
}
void load_cargo(Game& game,Entity& raft) {
    if (raft.entity_b.slot>=0) return;
    for (auto& cargo:game.entities) {
        if (cargo.kind!=EntityKind::GroundItem || cargo.toss.ticks || cargo.cell!=raft.cell || cargo.ground_item.count<=0 || cargo.ground_item.kind==ItemKind::None ||
            cargo.ground_item.kind==ItemKind::RiverFish || cargo.ground_item.flight.slot>=0 ||
            sled_cargo(game,cargo) || ridden_river_raft(game,cargo)) continue;
        stop_item_float(game,cargo);cargo.label_a=3;cargo.entity_a=handle_of(game,raft);
        raft.entity_b=handle_of(game,cargo);return;
    }
}
}

// RAFT SLOTS: label_a 0 lily / 1 log; timer_a drift beat; point_a expected cell;
// entity_a one player, entity_b one real item. GroundItem label_a=3 is raft cargo,
// with entity_a the reciprocal generation-qualified raft handle. No ownership by slot alone.
void init_river_raft(const Game& game,Entity& raft) {
    raft.sprite=Sprite::RiverLily;raft.health=raft.max_health=30;
    raft.point_a=raft.cell;raft.timer_a=water_current_beat(game.stage.at_or_border(raft.cell),18);
}
const Entity* ridden_river_raft(const Game& game,const Entity& actor) {
    if (actor.toss.ticks>0) return nullptr;
    const auto handle=handle_of(game,actor);
    const auto valid=[&](const Entity& raft) {
        return afloat(game,raft) && raft.point_a==raft.cell && actor.cell==raft.cell;
    };
    if (actor.kind==EntityKind::GroundItem && actor.label_a==3 && actor.ground_item.flight.slot<0) {
        const auto* raft=get_entity(game,actor.entity_a);
        return raft && valid(*raft) && raft->entity_b==handle ? raft : nullptr;
    }
    if (actor.kind!=EntityKind::Player || actor.health<=0 || actor.owner<0 || actor.owner>=4 ||
        !game.run.online[static_cast<std::size_t>(actor.owner)]) return nullptr;
    for (const auto& raft:game.entities) if (raft.entity_a==handle && valid(raft)) return &raft;
    return nullptr;
}
void step_raft_cargo(Game& game,Entity& cargo) {
    if (cargo.kind==EntityKind::GroundItem && cargo.label_a==3 && !ridden_river_raft(game,cargo)) {
        cargo.label_a=0;cargo.entity_a={};
    }
}
void clear_river_raft(Game& game,Entity& raft) {
    if (raft.kind!=EntityKind::RiverRaft) return;
    auto* cargo=get_entity(game,raft.entity_b);
    if (cargo && cargo->kind==EntityKind::GroundItem && cargo->label_a==3 && cargo->entity_a==handle_of(game,raft)) {
        cargo->label_a=0;cargo->entity_a={};
    }
    raft.entity_a=raft.entity_b={};
}
void board_river_raft(Game& game,int player_slot) {
    auto& player=game.entities[static_cast<std::size_t>(player_slot)];
    if (player.kind!=EntityKind::Player || player.health<=0 || player.toss.ticks || player.owner<0 || player.owner>=4 ||
        !game.run.online[static_cast<std::size_t>(player.owner)] ||
        ridden_sled(game,player) || ridden_river_raft(game,player)) return;
    for (auto& raft:game.entities) {
        if (!afloat(game,raft) || raft.point_a!=raft.cell || raft.cell!=player.cell) continue;
        sync_passengers(game,raft);if (raft.entity_a.slot>=0) continue;
        raft.entity_a={player_slot,player.generation};emit_sound(game,SoundId::WaterStep1,raft.cell);return;
    }
}
void step_river_raft(Game& game,int slot) {
    auto& raft=game.entities[static_cast<std::size_t>(slot)];
    if (!afloat(game,raft) || raft.point_a!=raft.cell) {
        clear_river_raft(game,raft);raft.point_a=raft.cell;
        raft.timer_a=water_current_beat(game.stage.at_or_border(raft.cell),18);
        return;
    }
    sync_passengers(game,raft);
    // Landing hooks and teleports can arrive without an ordinary walking step.
    for (Handle player:game.players) if (auto* rider=get_entity(game,player);rider && rider->cell==raft.cell) board_river_raft(game,player.slot);
    load_cargo(game,raft);
    auto* rider=get_entity(game,raft.entity_a);
    const Cell flow=water_current(game.stage.at_or_border(raft.cell));
    if (raft.timer_a>0 || raft.freeze_ticks>0 || flow==Cell{} || (rider && (rider->vitals.rooted || rider->vitals.grip))) return;
    raft.timer_a=water_current_beat(game.stage.at_or_border(raft.cell),18);
    const Cell next=raft.cell+flow;if (!landing_free(game,raft,next)) return;
    // All passengers move atomically; normal landing damage still applies.
    raft.cell=raft.point_a=next;raft.facing=flow;
    raft.timer_a=water_current_beat(game.stage.at_or_border(next),18);
    if (auto* cargo=get_entity(game,raft.entity_b)) cargo->cell=next;
    if (rider) {rider->cell=next;rider->vitals.slide_momentum=0;enter_actor_cell(game,raft.entity_a.slot);}
    sync_passengers(game,raft);load_cargo(game,raft);
    if ((next.x+next.y)%4==0) emit_sound(game,SoundId::CurrentDrift,next);
}
void catch_river_raft(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.kind==EntityKind::Player) {board_river_raft(game,slot);return;}
    if (actor.kind!=EntityKind::GroundItem) return;
    for (auto& raft:game.entities) if (afloat(game,raft) && raft.point_a==raft.cell && raft.cell==actor.cell) {
        sync_passengers(game,raft);load_cargo(game,raft);
        if (ridden_river_raft(game,actor)) return;
    }
}
bool valid_river_raft(const Entity& raft) {
    return raft.kind!=EntityKind::RiverRaft || (raft.label_a>=0 && raft.label_a<=1 &&
        raft.timer_a>=0 && raft.timer_a<=18 && raft.max_health==30 && raft.health<=30 &&
        !raft.impassable && !raft.hard_blocker && raft.move_interval==0);
}
