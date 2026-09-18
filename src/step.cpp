#include "debug/performance.hpp"
#include "world/fissures.hpp"
#include "world/lava_eruptions.hpp"
#include "scenery/roof.hpp"
#include "world/chasm.hpp"
#include "world/water.hpp"
#include "world/reactor.hpp"
#include "combat/toss.hpp"
#include "world/currents.hpp"
#include "props/conveyor.hpp"
#include "items/borrowed_summer.hpp"
#include "game.hpp"
#include "run/offers.hpp"
#include "entities/echo_hound.hpp"
#include "surfaces/interaction.hpp"
#include "surfaces/temperature.hpp"
#include "items/action.hpp"
#include "items/remedies.hpp"
#include "entities/dispatch.hpp"
#include "props/growth.hpp"

#include <algorithm>

namespace {

bool step_interlude(Game& game, const PlayerInputs& inputs) {
    if (game.run.phase == RunPhase::Reward) {
        for (const auto& [owner, participant] : game.players) {
            const Input& input=input_for(inputs,owner);
            Entity* player = get_entity(game, participant.controlled);
            if (player == nullptr) continue;
            if (input.drop) {
                if (input.select >= 0 && input.select < quick_slots)
                    player->inventory.selected = input.select;
                drop_player_item(game, *player);
            } else if (input.select >= 0 && input.select < 3)
                apply_offer_choice(game, static_cast<int>(owner), input);
            if (game.run.phase!=RunPhase::Reward) break;
        }
        return true;
    }
    if (game.run.phase == RunPhase::Shop) {
        for (const auto& [owner, participant] : game.players) {
            const Input& input=input_for(inputs,owner);
            Entity* player = get_entity(game, participant.controlled);
            if (player != nullptr && input.drop) {
                if (input.select >= 0 && input.select < quick_slots)
                    player->inventory.selected = input.select;
                drop_player_item(game, *player);
            } else if (input.select >= 0 && input.select < 3)
                apply_offer_choice(game, static_cast<int>(owner), input);
            if (input.confirm) player_state(game, owner).shop_ready = true;
        }
        advance_run(game);
        return true;
    }
    return game.run.phase == RunPhase::Won;
}

void step_players(Game& game, const PlayerInputs& inputs) {
    for (const auto& [owner, participant] : game.players) {
        const Input& input=input_for(inputs,owner);
        const Handle handle = player_state(game, owner).controlled;
        Entity* player = get_entity(game, handle);
        if (player == nullptr) continue;
        if (player->health <= 0 || !player_state(game, owner).online) { cancel_item_action(*player); continue; }
        if (player->toss.ticks>0 || player->basic.jump_ticks>0 || player->basic.carried_by.slot>=0) { cancel_item_action(*player); continue; }
        if (player->sleep_ticks > 0 || player->stun_ticks > 0) {
            use_disabled_remedy(game, handle.slot, input);
            continue;
        }
        if (player_state(game, owner).pending_count > 0) {
            cancel_item_action(*player);
            if (input.drop) {
                if (input.select >= 0 && input.select < quick_slots)
                    player->inventory.selected = input.select;
                drop_player_item(game, *player);
            } else if (input.select >= 0 && input.select < 3)
                apply_offer_choice(game, static_cast<int>(owner), input);
            continue;
        }
        if (input.offer_token!=0) { cancel_item_action(*player); continue; }
        step_player(game, handle.slot, input);
    }
}

void step_nonplayers(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind == EntityKind::None || entity.birth_tick == game.tick ||
            (entity.health == 0 && entity.kind != EntityKind::RailLayer &&
             entity.kind != EntityKind::GroundItem) ||
            entity.basic.carried_by.slot>=0 || entity.toss.ticks>0 || entity.sleep_ticks > 0 || entity.stun_ticks > 0) continue;
        step_entity(game, slot);
    }
}

void sweep_dead(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::Player &&
            entity.health == 0 && entity.kind != EntityKind::RailLayer &&
            entity.kind != EntityKind::GroundItem)
            remove_entity(game, {slot, entity.generation});
    }
    bool any_alive = false;
    for (Handle handle : controlled_entities(game)) {
        const Entity* player = get_entity(game, handle);
        any_alive |= player != nullptr && player->health > 0;
    }
    game.game_over = !any_alive && game.run.death_policy != DeathPolicy::Entrance;
}

} // namespace

void emit_sound(Game& game, SoundId sound, Cell cell, bool positional, bool muffled) {
    if (positional && !muffled) hear_world_action(game, sound, cell);
    if (game.sound_count >= static_cast<int>(game.sounds.size())) return;
    const auto sequence = static_cast<std::uint8_t>(game.sound_count);
    game.sounds[static_cast<std::size_t>(game.sound_count++)] =
        {sound, cell, game.tick, sequence, positional, muffled};
}

void step_game(Game& game, const PlayerInputs& inputs) {
    PerfScope tick_scope(PerfZone::Tick);
    if (!game.started || game.game_over) return;
    ++game.tick;
    game.sound_count = 0;
    game.impact_count = 0;
    game.shot_count = 0;
    game.sweep_count = 0;
    game.fall_count=0;
    if (step_interlude(game, inputs)) return;

    PerfScope phase(PerfZone::Temperature);
    step_temperature(game);
    phase.next(PerfZone::Surfaces);
    step_surfaces(game);
    phase.next(PerfZone::Roofs);
    step_roofs(game);
    phase.next(PerfZone::Props);
    step_prop_growth(game);

    phase.next(PerfZone::Timers);
    // TIMERS: A cooldown reaching zero can act on this tick.
    for (int slot = 0; slot < max_entities; ++slot)
        step_entity_timers(game, slot);
    step_actor_tosses(game);
    phase.next(PerfZone::Players);
    step_players(game, inputs);
    step_summer_auras(game);
    if (game.run.phase == RunPhase::Reward) return;

    // ACTORS: Newborns wait a tick; cleanup follows every attack and hazard.
    phase.next(PerfZone::Actors);
    step_nonplayers(game);
    phase.next(PerfZone::Hazards);
    step_water_currents(game);
    step_conveyors(game);
    step_traps(game);
    step_chasm_contacts(game);
    step_deep_river_contacts(game);
    step_lava_eruptions(game);
    step_fissures(game);
    phase.next(PerfZone::Cleanup);
    sweep_dead(game);
    step_reactor(game);
}
