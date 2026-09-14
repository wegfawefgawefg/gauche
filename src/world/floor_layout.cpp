#include "floor_internal.hpp"
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

    // Route: stamp the main corridor and three side rooms before placing blockers.
    const int columns = 5 + (game.run.floor - 1) % 2;
    game.stage.width = columns * room_width;
    game.stage.height = 3 * room_height;
    game.stage.tiles.assign(static_cast<std::size_t>(game.stage.width * game.stage.height),
                            {TileKind::Wall, 100, 0});
    const int branch = 1 + static_cast<int>(random_u32(game) %
                                                 static_cast<std::uint32_t>(columns - 3));
    const int extra = branch == 1 ? columns - 2 : 1;
    const int extra_row = random_u32(game) % 2 == 0 ? 0 : 2;
    for (int column = 0; column < columns; ++column) {
        stamp_room(game, column, 1, true);
        if (column < columns - 1)
            carve_room_area(game.stage, column * room_width + 10, 14,
                  (column + 1) * room_width + 2, 16, TileKind::Empty);
    }
    connect_branch(game, branch, 0);
    connect_branch(game, branch, 2);
    connect_branch(game, extra, extra_row);
    const int branch_x = branch * room_width + 6;
    const int extra_x = extra * room_width + 6;
    const int extra_y = extra_row * room_height + 5;
    game.run.spawn = {6, 15};
    game.run.exit = {columns * room_width - 6, 15};
    game.run.has_key = false;
    game.run.objective = (game.run.floor - 1) % 2 == 0 ?
        ObjectiveKind::Key : ObjectiveKind::Switch;
    game.run.roof_lights = {};
    game.run.roof_light_count = 0;
    // CANOPY: Only some forest clearings open to the sky; other rooms need actual emitters.
    if (game.run.floor <= 4) {
        for (int column = 1; column < columns; ++column)
            if (column == 1 || random_u32(game) % 3 == 0)
                game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)] =
                    {{column * room_width + 6, 13}};
        if (random_u32(game) % 2 == 0)
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)] =
                {{extra_x, extra_y}};
    }
    // Objective: the key or switch must sit off the main route.
    spawn_entity(game, game.run.objective == ObjectiveKind::Key ?
                 EntityKind::Key : EntityKind::Switch, {branch_x, 5});
    spawn_entity(game, EntityKind::Door, {(columns - 2) * room_width, 15});
    spawn_entity(game, EntityKind::Exit, game.run.exit);

    // Loadouts: a new adventurer starts light; survivors keep what they found.
    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (!joined[owner]) continue;
        if (previous[owner].health <= 0 && game.run.death_policy == DeathPolicy::NoRespawn)
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

    populate_floor(game, columns, {branch_x, 5}, {extra_x, extra_y});
    scatter_forest_props(game);
    // BOUNDARY: Normal tools cannot tunnel off the generated floor.
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x)
            if (x == 0 || y == 0 || x == game.stage.width - 1 || y == game.stage.height - 1)
                *game.stage.at({x, y}) = {TileKind::Wall, 100, 0, 100,
                                        BreakRule::Unbreakable, 0};
    emit_sound(game, SoundId::LevelStart, game.run.spawn, false);
}
