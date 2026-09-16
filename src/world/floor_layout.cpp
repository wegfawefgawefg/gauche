#include "fissures.hpp"
#include "ant_colonies.hpp"
#include "mushroom_settlements.hpp"
#include "forest_boss.hpp"
#include "forest_border.hpp"
#include "forest_outskirts.hpp"
#include "loose_finds.hpp"
#include "room_decisions.hpp"
#include "forest_theme_layers.hpp"
#include "generation_trace.hpp"
#include "open_sectors.hpp"
#include "spider_growth.hpp"
#include "root_growth.hpp"
#include "rivers.hpp"
#include "light_towers.hpp"
#include "lava_eruptions.hpp"
#include "tall_trees.hpp"
#include "ice_pillars.hpp"
#include "ice_arches.hpp"
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

void generate_world_floor(Game& game, FloorLayout layout, PopulationReport* report, GenerationTrace* trace) {
    if (trace) trace->reset();
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
    const auto capture = [&](const char* name) { if (trace) trace->capture(name,game,plan); };
    plan.report.seed=game.run.seed;plan.report.floor=game.run.floor;plan.report.initial_rng=game.rng;
    if (haunted || freight || reactor) for (const auto& rule:generation_rules) {
        FeatureDecision decision;decision.feature=rule.feature;
        decision.denominator=feature_denominator(rule,game.run.floor);
        decision.outcome=decision.denominator ? GenerationOutcome::Suppressed : GenerationOutcome::Ineligible;
        decision.reason=decision.denominator ? "Whole-floor unique replaces ordinary landmark planning" :
            "Outside this feature's biome/stage window";
        plan.report.features.push_back(std::move(decision));
    }
    capture("Special layout selection");
    if (!haunted && !freight && !reactor) {
        plan = plan_floor(game);
        capture("Room and landmark plan");
        const auto planned_rooms=plan.rooms;
        carve_floor(game, plan,trace);
        record_room_revisions(plan,planned_rooms,"Geometry validation / reservation rollback");
        for (auto& decision:plan.report.features) if (decision.outcome==GenerationOutcome::Reserved) {
            decision.outcome=GenerationOutcome::Built;
            decision.reason="Landmark geometry carved; this does not guarantee every later prop or loot placement";
        }
        capture("Base terrain / rooms / landmark geometry");
        place_forest_terrain(game, plan);
        capture("Forest terrain");
        place_water_scenes(game, plan);
        capture("Water scenes");
        place_ice_terrain(game, plan);
        capture("Ice terrain");
        place_workfront_terrain(game,plan);
        capture("Workfront terrain");
        place_assembly_belts(game,plan);
        capture("Assembly belts");
        place_water_currents(game,plan);
        capture("Currents");
        game.run.spawn = plan.rooms[0].center;
        game.run.exit = plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
        game.run.has_key = false;
        game.run.objective = (game.run.floor - 1) % 2 == 0 ? ObjectiveKind::Key : ObjectiveKind::Switch;
        place_chasms(game,plan);
        carve_shelf_reward(game,plan);
        capture("Chasms and shelf routes");
        carve_open_sectors(game,plan,trace);
        capture("Cross-room open sectors");
        carve_forest_river(game,plan,trace);
        capture("River channels and banks");
        grow_spider_habitats(game,plan,trace);
        capture("Spider habitat growth");
        grow_giant_roots(game,plan,trace);
        capture("Giant root growth");
        carve_forest_theme_patches(game,plan);
        capture("Floor identity terrain");
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
    else {
        populate_industrial_links(game,plan);
        populate_ice_thaw(game,plan);
        populate_shelf_reward(game,plan);
        place_forest_boss(game,plan,report);
        capture("Forest boss clearing");
        populate_rooms(game,plan,report,&plan.report);
        capture("Inhabitants and loot");
        populate_open_sectors(game,plan,report);
        populate_forest_river(game,plan,report);
        populate_spider_growth(game,plan,report);
        populate_river_rafts(game,plan);
        capture("Cross-room inhabitants and scenery");
        populate_forest_outskirts(game,plan,report,trace);
        capture("Outlying Forest encounters");
        // Ambient colonies use remaining capacity after all ordinary fighters.
        populate_ant_colonies(game,plan,report);
        capture("Ant colonies and food routes");
        populate_mushroom_settlements(game,plan,report);
        capture("Mushroom woods and households");
        dress_forest_theme_patches(game,plan);
        capture("Floor identity scenery");
        // Reserve structures before loose clutter consumes their clear ground.
        place_shipping_containers(game,plan);
        capture("Containers");
        place_ice_arches(game,plan);
        capture("Ice spans");
        scatter_room_props(game,plan);
        capture("Loose props");
        place_ice_pillars(game,plan);
        capture("Ice pillars");
        place_tall_trees(game,plan);
        capture("Tall trees");
        place_light_towers(game,plan);
        capture("Light towers");
        place_roof_scenes(game,plan);
        capture("Roof scenes");
    }
    if(!reactor && !freight && !haunted) {
        place_forest_border(game,plan);
        capture("Biome border scenery");
        place_forest_loose_finds(game,plan,report,trace);
        capture("Late Forest finds");
    }
    place_fissures(game,plan);
    place_lava_vents(game);
    capture("Fissures and lava vents");
    emit_sound(game, SoundId::LevelStart, game.run.spawn, false);
    game.generation_report=std::make_shared<const GenerationReport>(plan.report);
    capture("Finished");
}
