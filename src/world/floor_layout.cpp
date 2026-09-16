#include "fissures.hpp"
#include "light_towers.hpp"
#include "lava_eruptions.hpp"
#include "tall_trees.hpp"
#include "ice_pillars.hpp"
#include "industrial_geometry.hpp"
#include "ice_shelves.hpp"
#include "ice_thaw.hpp"
#include "chasm.hpp"
#include "roof_scenes.hpp"
#include "shipping_yard.hpp"
#include "reactor.hpp"
#include "workfront.hpp"
#include "freight_exchange.hpp"
#include "assembly.hpp"
#include "../items/ice_anchor.hpp"
#include "route.hpp"
#include "water.hpp"
#include "currents.hpp"
#include "ice_terrain.hpp"
#include "encounter.hpp"
#include "../props/interaction.hpp"
#include "../items/pocket_door.hpp"

#include <array>
#include <utility>

void generate_world_floor(Game& game, FloorLayout layout, PopulationReport* report) {
    // Party: carry each joined player across the new stage.
    std::array<Entity, 4> previous{};
    std::array<bool, 4> joined{};
    for (std::size_t owner = 0; owner < 4; ++owner) {
        if (const Entity* player = get_entity(game, game.players[owner])) {
            previous[owner] = *player;
            for (Item& item : previous[owner].inventory.slots) sync_ice_anchor(game,item);
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
    game.run.layout=FloorLayout::Generated;
    const bool reactor=(layout==FloorLayout::Automatic || layout==FloorLayout::LastShift) &&
        make_last_shift(game,layout==FloorLayout::LastShift);
    const bool freight=!reactor && (layout==FloorLayout::Automatic || layout==FloorLayout::FreightExchange) &&
        make_freight_exchange(game,layout==FloorLayout::FreightExchange);
    const bool haunted = !reactor && !freight && (layout==FloorLayout::Automatic || layout==FloorLayout::HauntedHouse) &&
        make_haunted_floor(game, layout == FloorLayout::HauntedHouse);
    FloorPlan plan;
    if (!haunted && !freight && !reactor) {
        plan = plan_floor(game);
        carve_floor(game, plan);
        place_forest_terrain(game, plan);
        place_water_scenes(game, plan);
        place_ice_terrain(game, plan);
        place_workfront_terrain(game,plan);
        place_assembly_belts(game,plan);
        place_water_currents(game,plan);
        game.run.spawn = plan.rooms[0].center;
        game.run.exit = plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
        game.run.has_key = false;
        game.run.objective = (game.run.floor - 1) % 2 == 0 ? ObjectiveKind::Key : ObjectiveKind::Switch;
        place_chasms(game,plan);
        carve_shelf_reward(game,plan);
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
        if (previous[owner].kind == EntityKind::None) {
            player->inventory = {};
            insert_item(player->inventory, make_item(ItemKind::Fist));
            insert_item(player->inventory, make_item(ItemKind::Bandage, 3));
        } else {
            player->inventory = previous[owner].inventory;
            // EXIT: Returning weapons rejoin their reserved slots as the old floor is discarded.
            for (Item& item : player->inventory.slots) {
                item.flight = {};
                fold_unused_door(item);
                fold_ice_anchor(item);
            }
            player->light = previous[owner].light;
            player->self_light = previous[owner].self_light;
            player->max_health = previous[owner].max_health;
            player->health = previous[owner].health > 0 ? previous[owner].health : player->max_health;
            player->move_interval = previous[owner].move_interval;
            player->artifacts = previous[owner].artifacts;
        }
    }

    if (reactor) populate_last_shift(game);
    else if (freight) populate_freight_exchange(game);
    else if (haunted) populate_haunted_house(game);
    else { populate_industrial_links(game,plan); populate_ice_thaw(game,plan); populate_shelf_reward(game,plan); populate_rooms(game, plan,report); place_shipping_containers(game,plan); scatter_room_props(game, plan); place_ice_pillars(game,plan); place_tall_trees(game,plan); place_light_towers(game,plan); place_roof_scenes(game,plan); }
    place_fissures(game,plan);
    place_lava_vents(game);
    emit_sound(game, SoundId::LevelStart, game.run.spawn, false);
}
