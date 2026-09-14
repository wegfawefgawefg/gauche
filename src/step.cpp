#include "game.hpp"
#include "entities/echo_hound.hpp"
#include "surfaces/interaction.hpp"
#include "surfaces/temperature.hpp"
#include "items/action.hpp"
#include "items/remedies.hpp"
#include "entities/dispatch.hpp"
#include "props/growth.hpp"

#include <algorithm>

namespace {

bool step_interlude(Game& game, const std::array<Input, 4>& inputs) {
    if (game.run.phase == RunPhase::Reward) {
        for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
            Entity* player = get_entity(game, game.players[owner]);
            if (player == nullptr) continue;
            if (inputs[owner].drop) {
                if (inputs[owner].select >= 0 && inputs[owner].select < quick_slots)
                    player->inventory.selected = inputs[owner].select;
                drop_player_item(game, *player);
            } else if (inputs[owner].select >= 0 && inputs[owner].select < 3)
                choose_reward(game, static_cast<int>(owner), inputs[owner].select);
        }
        return true;
    }
    if (game.run.phase == RunPhase::Shop) {
        for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
            Entity* player = get_entity(game, game.players[owner]);
            if (player != nullptr && inputs[owner].drop) {
                if (inputs[owner].select >= 0 && inputs[owner].select < quick_slots)
                    player->inventory.selected = inputs[owner].select;
                drop_player_item(game, *player);
            } else if (inputs[owner].select >= 0 && inputs[owner].select < 3)
                buy_shop_item(game, static_cast<int>(owner), inputs[owner].select);
            if (inputs[owner].confirm) game.run.shop_ready[owner] = true;
        }
        advance_run(game);
        return true;
    }
    return game.run.phase == RunPhase::Won;
}

void step_players(Game& game, const std::array<Input, 4>& inputs) {
    for (std::size_t owner = 0; owner < game.players.size(); ++owner) {
        const Handle handle = game.players[owner];
        Entity* player = get_entity(game, handle);
        if (player == nullptr) continue;
        if (player->health <= 0 || !game.run.online[owner]) { cancel_item_action(*player); continue; }
        if (player->sleep_ticks > 0 || player->stun_ticks > 0) {
            use_disabled_remedy(game, handle.slot, inputs[owner]);
            continue;
        }
        if (game.run.pending_count[owner] > 0) {
            cancel_item_action(*player);
            if (inputs[owner].drop) {
                if (inputs[owner].select >= 0 && inputs[owner].select < quick_slots)
                    player->inventory.selected = inputs[owner].select;
                drop_player_item(game, *player);
            } else if (inputs[owner].select >= 0 && inputs[owner].select < 3)
                choose_pending_reward(game, static_cast<int>(owner), inputs[owner].select);
            continue;
        }
        step_player(game, handle.slot, inputs[owner]);
    }
}

void step_hearth_aura(Game& game) {
    if (game.tick % 60 != 0) return;
    for (Handle source_handle : game.players) {
        const Entity* source = get_entity(game, source_handle);
        if (source == nullptr || source->health <= 0 ||
            !has_artifact(*source, ArtifactKind::Hearth)) continue;
        for (Handle target_handle : game.players) {
            Entity* target = get_entity(game, target_handle);
            if (target != nullptr && target->health > 0 &&
                distance(source->cell, target->cell) <= 4)
                target->health = std::min(target->max_health, target->health + 1);
        }
    }
}

void step_nonplayers(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind == EntityKind::None || entity.birth_tick == game.tick ||
            (entity.health == 0 && entity.kind != EntityKind::RailLayer &&
             entity.kind != EntityKind::GroundItem) ||
            entity.sleep_ticks > 0 || entity.stun_ticks > 0) continue;
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
    for (Handle handle : game.players) {
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

void step_game(Game& game, const std::array<Input, 4>& inputs) {
    if (!game.started || game.game_over) return;
    ++game.tick;
    game.sound_count = 0;
    game.impact_count = 0;
    game.shot_count = 0;
    game.sweep_count = 0;
    if (step_interlude(game, inputs)) return;

    step_temperature(game);
    step_surfaces(game);
    step_prop_growth(game);

    // TIMERS: A cooldown reaching zero can act on this tick.
    for (int slot = 0; slot < max_entities; ++slot)
        step_entity_timers(game, slot);
    step_players(game, inputs);
    step_hearth_aura(game);
    if (game.run.phase == RunPhase::Reward) return;

    // ACTORS: Newborns wait a tick; cleanup follows every attack and hazard.
    step_nonplayers(game);
    step_traps(game);
    sweep_dead(game);
}
