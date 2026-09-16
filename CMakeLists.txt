cmake_minimum_required(VERSION 3.24)
project(gauche VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GAUCHE_STRICT "Enable strict compiler warnings" ON)
option(GAUCHE_WARN_AS_ERROR "Treat warnings as errors" ON)
# Published revisions used by this game, including the transitive layout library.
set(GAUCHE_GUBSY_REV c4d5408c74e61914f3886099db799919a2637696)
set(GAUCHE_GVIEW_REV 71956a3f3884a7e542b65cadd413637b9b3cff0e)
set(GAUCHE_GLAYOUT_REV 50861b524e58195f937e48984b33c1ca65e9631d)
option(GAUCHE_USE_LOCAL_DEPS "Use developer checkouts instead of published dependency pins" OFF)
set(GAUCHE_GUBSY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Splonks/gubsy"
    CACHE PATH "Developer Gubsy checkout (requires GAUCHE_USE_LOCAL_DEPS)")
set(GAUCHE_GVIEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../gview"
    CACHE PATH "Developer GView checkout (requires GAUCHE_USE_LOCAL_DEPS)")
set(GAUCHE_GLAYOUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../glayout"
    CACHE PATH "Developer GLayout checkout (requires GAUCHE_USE_LOCAL_DEPS)")
if(GAUCHE_USE_LOCAL_DEPS)
    foreach(source IN ITEMS "${GAUCHE_GUBSY_DIR}" "${GAUCHE_GVIEW_DIR}" "${GAUCHE_GLAYOUT_DIR}")
        if(NOT EXISTS "${source}/CMakeLists.txt")
            message(FATAL_ERROR "Missing developer dependency: ${source}. Disable GAUCHE_USE_LOCAL_DEPS to fetch the published pins.")
        endif()
    endforeach()
    message(STATUS "Gauche: using developer dependency checkouts")
else()
    message(STATUS "Gauche: fetching published Gubsy/GView/GLayout pins")
endif()

set(GUB_MODE consumer CACHE STRING "" FORCE)
set(GUB_BUILD_SAMPLE OFF CACHE BOOL "" FORCE)
set(GUB_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(GUB_ENABLE_LUA_MOD_HOST OFF CACHE BOOL "" FORCE)
set(GUB_ENABLE_STEAM OFF CACHE BOOL "" FORCE)
set(GUB_FETCH_DEPS ON CACHE BOOL "" FORCE)

if(GAUCHE_USE_LOCAL_DEPS)
    add_subdirectory("${GAUCHE_GUBSY_DIR}" gubsy)
else()
    FetchContent_Declare(gubsy
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gubsy.git
        GIT_TAG ${GAUCHE_GUBSY_REV})
    FetchContent_MakeAvailable(gubsy)
    set(GAUCHE_GUBSY_DIR "${gubsy_SOURCE_DIR}")
endif()

set(GLAYOUT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLAYOUT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLAYOUT_BUILD_SDL_DEMO OFF CACHE BOOL "" FORCE)
set(GLAYOUT_WITH_IMGUI OFF CACHE BOOL "" FORCE)
set(GVIEW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GVIEW_BUILD_AUTHORING OFF CACHE BOOL "" FORCE)
set(GVIEW_WITH_IMGUI OFF CACHE BOOL "" FORCE)
set(GVIEW_WITH_SDL3_RENDERER ON CACHE BOOL "" FORCE)
set(GVIEW_SDL3_TARGET SDL3::SDL3 CACHE STRING "" FORCE)
# Provide glayout::graph before GView, so its older fallback pin is never selected.
if(GAUCHE_USE_LOCAL_DEPS)
    add_subdirectory("${GAUCHE_GLAYOUT_DIR}" glayout)
    add_subdirectory("${GAUCHE_GVIEW_DIR}" gview)
else()
    FetchContent_Declare(glayout
        GIT_REPOSITORY https://github.com/wegfawefgawefg/glayout.git
        GIT_TAG ${GAUCHE_GLAYOUT_REV})
    FetchContent_MakeAvailable(glayout)
    FetchContent_Declare(gview
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gview.git
        GIT_TAG ${GAUCHE_GVIEW_REV})
    FetchContent_MakeAvailable(gview)
endif()

set(GAUCHE_CODEC_SOURCES
    src/net_codec.cpp src/net/packet.cpp src/net/entity_codec.cpp)

set(GAUCHE_ENTITY_SOURCES
    src/props/industrial.cpp
    src/entities/strikebreaker.cpp src/entities/breaker_render.cpp
    src/entities/rivet_gunner.cpp
    src/world/rivet_post.cpp
    src/world/assembly.cpp
    src/props/conveyor_render.cpp
    src/props/conveyor.cpp
    src/items/reload.cpp
    src/items/coolant.cpp
    src/world/cooling_works.cpp
    src/items/barricade.cpp
    src/items/magnet.cpp
    src/items/arc_torch.cpp
    src/items/belt_tools.cpp
    src/items/rivet_gun.cpp
    src/items/industrial_melee.cpp
    src/entities/powder_monkey.cpp src/entities/powder_route.cpp
    src/items/quarry_charge.cpp src/items/fuse_scissors.cpp src/projectiles/exposed_fuse.cpp
    src/projectiles/coal_spit.cpp
    src/items/foreman_whistle.cpp
    src/entities/mine_crew.cpp src/entities/mine_route.cpp
    src/items/effigy_mask.cpp src/items/ice_anchor.cpp src/items/snow_shelter.cpp src/items/sled.cpp
    src/items/stillwater_bell.cpp src/world/currents.cpp
    src/items/tuning_fork.cpp
    src/items/folded_bridge.cpp
    src/items/thaw_charge.cpp
    src/items/heat_siphon.cpp src/items/heat_extraction.cpp
    src/items/borrowed_summer.cpp src/items/storm_lantern.cpp src/items/echo_pebble.cpp
    src/items/harpoon.cpp src/projectiles/harpoon.cpp
    src/items/doorstop.cpp src/props/doorstop.cpp src/entities/gate.cpp src/world/sluice.cpp
    src/items/circuits.cpp src/props/circuits.cpp
    src/items/candles.cpp src/props/candle.cpp src/items/coal.cpp src/props/stove.cpp src/items/kettle.cpp src/items/heated_water.cpp src/items/steam_lance.cpp src/items/tar_flask.cpp
    src/items/brine.cpp src/props/lunch_tin.cpp
    src/items/heat_capsule.cpp src/items/cold_flask.cpp src/items/air_bladder.cpp src/world/floating_items.cpp
    src/items/quarry_tools.cpp src/items/ice_brick.cpp src/projectiles/ice_brick.cpp
    src/entities/lens_warden.cpp src/entities/lens_watch.cpp src/world/lens_watch.cpp
    src/items/alarms.cpp src/props/alarm_clock.cpp
    src/items/fishing_line.cpp src/projectiles/fishing.cpp
    src/entities/shard_colony.cpp src/entities/shard_links.cpp src/world/crystal_gallery.cpp
    src/entities/icicle_spider.cpp src/entities/spider_strand.cpp src/entities/spider_site.cpp
    src/entities/boiler_porter.cpp src/entities/boiler_tank.cpp src/entities/boiler_splash.cpp src/items/pressure.cpp src/world/boiler_room.cpp
    src/entities/candle_keeper.cpp src/entities/keeper_tending.cpp src/world/chapel.cpp
    src/entities/snow_effigy.cpp
    src/items/pocket_drill.cpp
    src/entities/cable_crawler.cpp src/world/cable_trench.cpp
    src/entities/walking_kiln.cpp src/world/kiln_court.cpp
    src/entities/audit_clerk.cpp src/world/pay_office.cpp
    src/items/tension_spring.cpp src/props/tension_spring.cpp
    src/items/emergency_foam.cpp
    src/entities/coal_cutter.cpp src/entities/cutter_render.cpp
    src/entities/rail_cart.cpp src/entities/rail_shunter.cpp src/entities/freight_render.cpp src/world/freight_siding.cpp src/props/rail_points.cpp src/props/rail_render.cpp src/items/rail_switch_key.cpp src/items/insulated_boots.cpp src/items/glow_slag.cpp src/items/steel_toe.cpp src/items/lunch_tin.cpp src/props/streetlamp.cpp src/props/streetlamp_render.cpp src/world/streetlights.cpp src/scene_entities.cpp
    src/entities/tar_choir.cpp src/entities/choir_render.cpp src/projectiles/tar_spit.cpp src/world/settling_tanks.cpp
    src/entities/mold_thief.cpp src/entities/casting_mold.cpp src/items/mold_key.cpp src/world/casting_floor.cpp src/entities/mold_render.cpp src/items/machine_fittings.cpp src/items/machine_fitting_render.cpp src/entities/emergency_pump.cpp src/entities/pump_render.cpp src/entities/furnace_moth.cpp src/entities/counterweight.cpp src/entities/counterweight_render.cpp src/world/hoist_shaft.cpp src/world/room_supplies.cpp src/entities/ash_sleeper.cpp src/world/ash_loft.cpp src/entities/slag_snail.cpp src/surfaces/slag.cpp src/world/slag_bank.cpp src/world/lamp_alcove.cpp
    src/entities/pressure_rat.cpp
    src/entities/magnet_crane.cpp
    src/entities/arc_welder.cpp
    src/props/ice_pillar.cpp src/props/ice_pillar_render.cpp src/world/ice_pillars.cpp
    src/entities/brawler.cpp src/world/brawlers.cpp
    src/world/chasm.cpp src/world/chasm_render.cpp src/world/chasm_generation.cpp
    src/entities/yeti.cpp
    src/combat/toss.cpp
    src/entities/avalanche_ram.cpp
    src/entities/whiteout_drummer.cpp src/items/snow_globe.cpp src/surfaces/whiteout.cpp
    src/entities/death_sound.cpp
    src/entities/seal_thief.cpp src/entities/seal_route.cpp
    src/items/fish.cpp src/entities/fishing_widow.cpp src/projectiles/widow_hook.cpp
    src/items/muffling.cpp
    src/entities/echo_hound.cpp src/entities/echo_hearing.cpp
    src/entities/mirror_knight.cpp
    src/combat/beams.cpp src/props/cloth.cpp src/items/optics.cpp src/projectiles/prism.cpp
    src/entities/snow_burrower.cpp src/world/snow.cpp src/items/snow_tools.cpp src/projectiles/snowball.cpp
    src/entities/glass_eel.cpp src/surfaces/conduction.cpp src/items/eel_battery.cpp
    src/entities/ice_mason.cpp src/props/ice_cover.cpp
    src/entities/steam_leech.cpp src/entities/leech_heat.cpp
    src/entities/frozen_pilgrim.cpp src/entities/pilgrim_heat.cpp
    src/entities/bell_diver.cpp src/entities/diver_route.cpp
    src/entities/frost_bat.cpp src/projectiles/frost.cpp src/surfaces/temperature.cpp
    src/entities/rime_skater.cpp src/items/ice_footing.cpp src/items/ice_equipment.cpp src/items/flare.cpp src/items/ice_projectiles.cpp src/items/catalog.cpp src/items/pocket_pump.cpp src/surfaces/liquid_transfer.cpp src/items/hand_bellows.cpp src/surfaces/wind.cpp src/items/nail_board.cpp src/traps/contact.cpp src/traps/nail_board.cpp src/items/chain_hook.cpp src/projectiles/chain_hook.cpp src/items/bolt_pouch.cpp src/items/industrial_use.cpp src/items/use_completion.cpp
    src/entities/pocket_door.cpp src/items/pocket_door.cpp
    src/projectiles/thunder.cpp
    src/entities/targeting.cpp
    src/items/decoys.cpp src/props/scarecrow.cpp
    src/combat/parry.cpp src/combat/ranged.cpp
    src/items/shields.cpp
    src/items/mixtures.cpp src/surfaces/scent.cpp
    src/entities/hearing.cpp src/items/noisemakers.cpp
    src/items/movement_tools.cpp src/projectiles/net.cpp src/surfaces/slip.cpp
    src/items/woodland_traps.cpp src/traps/woodland.cpp
    src/items/root_relics.cpp src/projectiles/root_drill.cpp src/projectiles/blink.cpp
    src/items/displacement.cpp src/projectiles/hook.cpp src/debris/air.cpp
    src/items/ground_tools.cpp src/entities/bird_feeding.cpp
    src/status/recovery.cpp src/items/cold_remedies.cpp src/status/effects.cpp src/items/remedies.cpp
    src/items/woodland_tools.cpp src/props/growth.cpp
    src/surfaces/interaction.cpp
    src/projectiles/projectile.cpp src/items/bow.cpp src/items/action.cpp src/items/material_catalog.cpp src/items/materials.cpp src/items/firearms.cpp
    src/items/supply.cpp src/items/supply_catalog.cpp src/items/ammunition.cpp
    src/entity.cpp src/combat/damage.cpp src/combat/shove.cpp
    src/combat/melee.cpp src/items/forest.cpp src/items/food.cpp src/items/campfire.cpp src/items/fire.cpp src/projectiles/recoverable.cpp src/items/stacking.cpp src/items/ground_interaction.cpp src/items/fixture_pickup.cpp
    src/items/cooking.cpp src/items/cooking_render.cpp
    src/entities/attacks.cpp src/entities/defense.cpp
    src/entities/boar.cpp src/entities/thorn_snail.cpp src/entities/lantern_moth.cpp
    src/entities/burrow_worm.cpp
    src/entities/wasp_nest.cpp src/entities/wasp.cpp src/entities/scavenging.cpp
    src/entities/forager_goblin.cpp src/entities/carrion_crow.cpp
    src/entities/mosquito.cpp src/entities/owl.cpp src/entities/woodpecker.cpp src/entities/root_turret.cpp src/entities/bramble_guard.cpp src/entities/spore_toad.cpp src/entities/crate_mimic.cpp
    src/world/arena.cpp src/world/water.cpp
    src/entities/dispatch.cpp
    src/entities/player.cpp src/entities/player_movement.cpp
    src/entities/zombie_stack.cpp src/entities/awareness.cpp
    src/entities/zombie.cpp
    src/entities/chicken.cpp src/entities/following.cpp src/entities/foraging.cpp
    src/entities/bat.cpp
    src/entities/wolf_call.cpp
    src/entities/wolf.cpp
    src/entities/dog.cpp
    src/entities/bear.cpp
    src/entities/bunny.cpp
    src/entities/ember.cpp
    src/entities/spawner.cpp
    src/entities/train.cpp
    src/entities/fixtures.cpp
    src/entities/navigation.cpp
    src/entities/behavior.cpp
    src/entities/timers.cpp
    src/entities/contact.cpp
)

set(GAUCHE_WORLD_SOURCES src/world/scrap_yard.cpp src/world/repair_bay.cpp src/world/workfront.cpp src/world/ice_terrain.cpp src/world/floor_layout.cpp src/world/floor_reachability.cpp src/world/roof_scenes.cpp src/scenery/roof.cpp
    src/world/ground_items.cpp src/world/route_graph.cpp src/world/room_shapes.cpp src/world/room_gates.cpp src/world/forest_terrain.cpp
    src/world/ice_shelves.cpp src/world/ice_thaw.cpp src/world/ice_material.cpp src/world/ice_material_render.cpp src/world/room_population.cpp src/world/industrial_population.cpp src/world/industrial_geometry.cpp src/debug/generation_audit.cpp src/world/salvage.cpp src/world/encounter.cpp src/world/encounter_spawn.cpp
    src/world/haunted_house.cpp src/world/freight_exchange.cpp src/world/last_shift.cpp src/world/reactor.cpp src/world/reactor_render.cpp src/world/loot.cpp)

set(GAUCHE_PROP_SOURCES src/props/catalog.cpp src/props/interaction.cpp src/props/placement.cpp)

set(GAUCHE_DEBRIS_SOURCES src/debris/motion.cpp src/debris/sweep.cpp src/debris/scatter.cpp)

add_executable(gauche
    src/main.cpp src/debug/multiplayer.cpp src/debug/headless.cpp src/app/options.cpp src/debug/panels.cpp src/debug/runtime.cpp src/debug/persistence.cpp src/debug/loadout.cpp src/debug/level_editor.cpp src/debug/loadout_editor.cpp src/ui/status.cpp src/ui/stage_announcement.cpp src/graphics.cpp src/input_device.cpp src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES}
    src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES} src/input.cpp src/input/prompts.cpp src/input/icon_set.cpp
    src/entities/shard_render.cpp src/entities/crane_render.cpp src/entities/boiler_render.cpp
    src/render.cpp src/world/floating_render.cpp src/items/fire_render.cpp src/items/quarry_render.cpp src/entities/eel_render.cpp src/entities/leech_render.cpp src/entities/mason_render.cpp src/entities/knight_render.cpp src/entities/drummer_render.cpp src/entities/warden_render.cpp src/entities/flight_render.cpp src/entities/plant_render.cpp src/entities/wolf_render.cpp src/entities/intent_render.cpp src/world/wall_render.cpp src/world/encounter_render.cpp src/props/render.cpp src/props/circuit_render.cpp src/debris/render.cpp ${GAUCHE_DEBRIS_SOURCES}
    src/lighting/field.cpp src/lighting/canopy.cpp src/lighting/sources.cpp
    src/lighting/render.cpp src/lighting/shafts.cpp src/ui/hud.cpp src/ui/panel.cpp src/ui/artifacts.cpp src/artifacts/catalog.cpp src/ui/prompts.cpp src/ui/item_details.cpp src/ui/item_meter.cpp
    src/ui/pattern_diagram.cpp src/ui/interaction.cpp src/ui/offer_flow.cpp
    src/ui/interaction_render.cpp src/ui/range.cpp src/ui/pointer.cpp src/ui/text.cpp
    src/audio.cpp src/ambience/mixer.cpp src/ambience/placement.cpp src/ambience/ice.cpp src/ambience/industrial.cpp src/ambience/source.cpp src/scenery/ice.cpp src/scenery/ice_render.cpp src/particles/system.cpp src/particles/render.cpp src/particles/water.cpp src/projectiles/render.cpp src/entities/gate_render.cpp src/scenery/overhead.cpp src/scenery/roof_render.cpp src/scenery/shadows.cpp src/surfaces/render.cpp src/particles/templates.cpp src/particles/presentation.cpp src/particles/gunfire.cpp src/particles/electricity.cpp src/particles/motion.cpp src/menu_shell.cpp src/rollback.cpp ${GAUCHE_CODEC_SOURCES} src/net_socket.cpp
    src/net_protocol.cpp src/net_session.cpp src/net/diagnostics.cpp src/net/traversal.cpp src/net/party.cpp src/net_snapshot.cpp
    src/net_session_host.cpp src/net_corrections.cpp src/net_session_client.cpp src/menu/front_page.cpp
    src/net/rooms/names.cpp src/net/rooms/http.cpp src/net/rooms/matchmaking.cpp src/net/rooms/worker.cpp src/net/rooms/runtime.cpp src/menu/rooms.cpp
    src/menu/pages.cpp src/menu/control_pages.cpp src/menu/control_reference.cpp src/menu/page_chrome.cpp src/menu/view_builder.cpp src/menu/theme.cpp
    src/menu/actions.cpp src/menu/audio.cpp src/menu/text_edit.cpp src/menu/profiles.cpp src/menu/bindings.cpp)
target_link_libraries(gauche PRIVATE gubsy::engine SDL3::SDL3
    SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer gview::sdl3)
if(WIN32)
    target_link_libraries(gauche PRIVATE ws2_32)
endif()
target_compile_definitions(gauche PRIVATE
    GAUCHE_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(gauche PRIVATE "${GAUCHE_GUBSY_DIR}" "${GAUCHE_GUBSY_DIR}/imgui")

if(MSVC)
    target_compile_options(gauche PRIVATE /W4 /permissive-)
    if(GAUCHE_WARN_AS_ERROR)
        target_compile_options(gauche PRIVATE /WX)
    endif()
else()
    target_compile_options(gauche PRIVATE -Wall -Wextra -Wpedantic)
    if(GAUCHE_STRICT)
        target_compile_options(gauche PRIVATE -Wconversion -Wsign-conversion -Wshadow)
    endif()
    if(GAUCHE_WARN_AS_ERROR)
        target_compile_options(gauche PRIVATE -Werror)
    endif()
endif()

add_custom_command(TARGET gauche POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:gauche>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${GAUCHE_GUBSY_DIR}/src/assets" "$<TARGET_FILE_DIR:gauche>/assets/gubsy-engine")

include(CTest)

# ART REVIEW: Static arrangements render without input, AI, networking, or playthroughs.
get_target_property(GAUCHE_CAPTURE_SOURCES gauche SOURCES)
list(REMOVE_ITEM GAUCHE_CAPTURE_SOURCES src/main.cpp)
add_executable(gauche_render_scene EXCLUDE_FROM_ALL
    tools/render_scene.cpp ${GAUCHE_CAPTURE_SOURCES})
target_link_libraries(gauche_render_scene PRIVATE gubsy::engine SDL3::SDL3
    SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer gview::sdl3)
target_include_directories(gauche_render_scene PRIVATE "${GAUCHE_GUBSY_DIR}" "${GAUCHE_GUBSY_DIR}/imgui")
target_compile_definitions(gauche_render_scene PRIVATE GAUCHE_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
if(WIN32)
    target_link_libraries(gauche_render_scene PRIVATE ws2_32)
endif()

if(BUILD_TESTING)
    set(GAUCHE_TEST_GAME_SOURCES
        src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp
        src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp
        src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    add_executable(gauche_tests
        tests/game_test.cpp ${GAUCHE_TEST_GAME_SOURCES})
    target_link_libraries(gauche_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME game_rules COMMAND gauche_tests)
    add_executable(gauche_item_tests tests/item_test.cpp
        src/ui/pattern_diagram.cpp ${GAUCHE_TEST_GAME_SOURCES})
    target_link_libraries(gauche_item_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_item_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME item_rules COMMAND gauche_item_tests)
    add_executable(gauche_rollback_tests
        tests/rollback_test.cpp src/rollback.cpp src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES}
        src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_rollback_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_rollback_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME rollback_rules COMMAND gauche_rollback_tests)
    add_executable(gauche_codec_tests
        tests/net_codec_test.cpp ${GAUCHE_CODEC_SOURCES} src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES}
        src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_codec_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_codec_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME snapshot_codec COMMAND gauche_codec_tests)
    add_executable(gauche_socket_tests tests/net_socket_test.cpp src/net_socket.cpp)
    if(WIN32)
        target_link_libraries(gauche_socket_tests PRIVATE ws2_32)
    endif()
    target_compile_options(gauche_socket_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME udp_loopback COMMAND gauche_socket_tests)
    add_executable(gauche_session_tests
        tests/net_session_test.cpp src/net_protocol.cpp src/net_session.cpp src/net/diagnostics.cpp src/net/traversal.cpp src/net/party.cpp
        src/net_snapshot.cpp src/net_session_host.cpp src/net_corrections.cpp src/net_session_client.cpp
        src/net_socket.cpp ${GAUCHE_CODEC_SOURCES} src/rollback.cpp src/stage.cpp
        ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_session_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_session_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if(WIN32)
        target_link_libraries(gauche_session_tests PRIVATE ws2_32)
    endif()
    add_test(NAME direct_session COMMAND gauche_session_tests)
    add_executable(gauche_loss_tests
        tests/net_loss_test.cpp src/net_protocol.cpp src/net_session.cpp src/net/diagnostics.cpp src/net/traversal.cpp src/net/party.cpp
        src/net_snapshot.cpp src/net_session_host.cpp src/net_corrections.cpp src/net_session_client.cpp
        src/net_socket.cpp ${GAUCHE_CODEC_SOURCES} src/rollback.cpp src/stage.cpp
        ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_loss_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_loss_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if(WIN32)
        target_link_libraries(gauche_loss_tests PRIVATE ws2_32)
    endif()
    add_test(NAME lossy_session COMMAND gauche_loss_tests)
    add_executable(gauche_presentation_tests
        ${GAUCHE_DEBRIS_SOURCES}
        tests/presentation_test.cpp src/particles/system.cpp
        src/particles/templates.cpp src/particles/presentation.cpp src/particles/gunfire.cpp src/particles/electricity.cpp src/particles/motion.cpp
        src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp
        src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp src/run/offers.cpp src/artifacts/hearth.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_presentation_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_presentation_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME presentation_rules COMMAND gauche_presentation_tests)
    add_executable(gauche_lighting_tests
        tests/lighting_test.cpp src/lighting/field.cpp src/lighting/canopy.cpp
        src/lighting/sources.cpp src/stage.cpp)
    target_link_libraries(gauche_lighting_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_lighting_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME lighting_rules COMMAND gauche_lighting_tests)
    add_test(NAME border_lighting COMMAND gauche --smoke-border --frames 3)
    set_tests_properties(border_lighting PROPERTIES ENVIRONMENT
        "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    add_test(NAME menu_shell COMMAND gauche --smoke-menu --frames 8)
    set_tests_properties(menu_shell PROPERTIES ENVIRONMENT
        "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    foreach(screen reward inventory)
        add_test(NAME ui_${screen} COMMAND gauche --smoke-${screen} --frames 3)
        set_tests_properties(ui_${screen} PROPERTIES ENVIRONMENT
            "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    endforeach()
    add_test(NAME direct_lobby COMMAND gauche --smoke-lobby --death entrance --frames 8)
    set_tests_properties(direct_lobby PROPERTIES ENVIRONMENT
        "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    add_test(NAME menu_leave COMMAND gauche --smoke-leave --frames 14)
    set_tests_properties(menu_leave PROPERTIES ENVIRONMENT
        "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    foreach(page lobby settings controls profile bindings detail input death victory)
        add_test(NAME menu_${page} COMMAND gauche --smoke-menu-page ${page} --frames 2)
        set_tests_properties(menu_${page} PROPERTIES ENVIRONMENT
            "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
    endforeach()
endif()

# TRANSPORT: Reuse Gubsy's wire protocols without requiring its game runtime in net tools.
add_library(gauche_realnet STATIC
    "${GAUCHE_GUBSY_DIR}/src/realnet/config.cpp"
    "${GAUCHE_GUBSY_DIR}/src/realnet/crypto.cpp"
    "${GAUCHE_GUBSY_DIR}/src/realnet/rendezvous.cpp"
    "${GAUCHE_GUBSY_DIR}/src/realnet/relay.cpp")
target_include_directories(gauche_realnet SYSTEM PUBLIC
    "${GAUCHE_GUBSY_DIR}/include" "${GAUCHE_GUBSY_DIR}/third_party")
foreach(target gauche gauche_render_scene gauche_session_tests gauche_loss_tests)
    if(TARGET ${target})
        target_link_libraries(${target} PRIVATE gauche_realnet)
    endif()
endforeach()

find_package(CURL REQUIRED)
foreach(target gauche gauche_render_scene)
    target_link_libraries(${target} PRIVATE CURL::libcurl)
endforeach()
