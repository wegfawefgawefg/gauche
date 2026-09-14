#include "route.hpp"
#include "encounter.hpp"
#include "../props/interaction.hpp"

#include <array>
#include <utility>

void generate_world_floor(Game& game) {
    // Party: carry each joined player across the new stage.
    std::array<Entity, 4> previous{};
    std::array<bool, 4> joined{};
    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (const Entity* player = get_entity(game, game.players[owner])) {
            previous[owner] = *player;
            joined[owner] = true;
        }
    }
    if (!game.started) joined[0] = true;
    Game next;
    next.rng = game.rng;
    next.tick = game.tick;
    next.run = game.run;
    next.started = true;
    for (int slot = 0; slot < max_entities; ++slot)
        next.entities[static_cast<std::size_t>(slot)].generation =
            game.entities[static_cast<std::size_t>(slot)].generation;
    game = std::move(next);
    game.run.phase = RunPhase::Playing;

    // ROUTE: Geometry and role pools share one seeded plan; no content can block its dry paths.
    const bool haunted = make_haunted_floor(game);
    FloorPlan plan;
    if (!haunted) {
        plan = plan_floor(game);
        carve_floor(game, plan);
        game.run.spawn = plan.rooms[0].center;
        game.run.exit = plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
        game.run.has_key = false;
        game.run.objective = (game.run.floor - 1) % 2 == 0 ? ObjectiveKind::Key : ObjectiveKind::Switch;
    }

    // Loadouts: a new adventurer starts light; survivors keep what they found.
    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (!joined[owner]) continue;
        if (previous[owner].kind != EntityKind::None && previous[owner].health <= 0 &&
            game.run.death_policy == DeathPolicy::NoRespawn)
            continue;
        const Handle handle = spawn_entity(game, EntityKind::Player,
                                            game.run.spawn + Cell{static_cast<int>(owner), 0});
        game.players[owner] = handle;
        Entity* player = get_entity(game, handle);
        player->owner = static_cast<int>(owner);
        player->impassable = game.run.online[owner];
        if (game.run.floor == 1 && previous[owner].kind == EntityKind::None) {
            player->inventory = {};
            insert_item(player->inventory, make_item(ItemKind::Fist));
            insert_item(player->inventory, make_item(ItemKind::Bandage, 3));
        } else {
            player->inventory = previous[owner].inventory;
            player->light = previous[owner].light;
            player->self_light = previous[owner].self_light;
            player->max_health = previous[owner].max_health;
            player->health = previous[owner].health > 0 ? previous[owner].health : player->max_health;
            player->move_interval = previous[owner].move_interval;
            player->artifacts = previous[owner].artifacts;
        }
    }

    if (haunted) populate_haunted_house(game);
    else { populate_rooms(game, plan); scatter_room_props(game, plan); }
    emit_sound(game, SoundId::LevelStart, game.run.spawn, false);
}
