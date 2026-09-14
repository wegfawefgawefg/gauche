cmake_minimum_required(VERSION 3.24)
project(gauche VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GAUCHE_STRICT "Enable strict compiler warnings" ON)
option(GAUCHE_WARN_AS_ERROR "Treat warnings as errors" ON)
set(GAUCHE_GUBSY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Splonks/gubsy"
    CACHE PATH "Pinned local Gubsy checkout")

set(GUB_MODE consumer CACHE STRING "" FORCE)
set(GUB_BUILD_SAMPLE OFF CACHE BOOL "" FORCE)
set(GUB_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(GUB_ENABLE_LUA_MOD_HOST OFF CACHE BOOL "" FORCE)
set(GUB_ENABLE_STEAM OFF CACHE BOOL "" FORCE)
set(GUB_FETCH_DEPS ON CACHE BOOL "" FORCE)

if(EXISTS "${GAUCHE_GUBSY_DIR}/CMakeLists.txt")
    add_subdirectory("${GAUCHE_GUBSY_DIR}" gubsy)
else()
    FetchContent_Declare(gubsy
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gubsy.git
        GIT_TAG c4d5408c74e61914f3886099db799919a2637696)
    FetchContent_MakeAvailable(gubsy)
    set(GAUCHE_GUBSY_DIR "${gubsy_SOURCE_DIR}")
endif()

set(GAUCHE_GVIEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../gview"
    CACHE PATH "Pinned local GView checkout")
set(GVIEW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GVIEW_BUILD_AUTHORING OFF CACHE BOOL "" FORCE)
set(GVIEW_WITH_IMGUI OFF CACHE BOOL "" FORCE)
set(GVIEW_WITH_SDL3_RENDERER ON CACHE BOOL "" FORCE)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../glayout/CMakeLists.txt")
    set(GVIEW_GLAYOUT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../glayout"
        CACHE PATH "Pinned local GLayout checkout")
endif()
set(GVIEW_SDL3_TARGET SDL3::SDL3 CACHE STRING "" FORCE)
if(EXISTS "${GAUCHE_GVIEW_DIR}/CMakeLists.txt")
    add_subdirectory("${GAUCHE_GVIEW_DIR}" gview)
else()
    FetchContent_Declare(gview
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gview.git
        GIT_TAG 992224876f590f5ae6722dda99d5b53a84c8c739)
    FetchContent_MakeAvailable(gview)
endif()

set(GAUCHE_ENTITY_SOURCES
    src/items/cold_flask.cpp src/items/air_bladder.cpp src/world/floating_items.cpp
    src/entities/bell_diver.cpp src/entities/diver_route.cpp
    src/entities/frost_bat.cpp src/projectiles/frost.cpp src/surfaces/temperature.cpp
    src/entities/rime_skater.cpp src/items/ice_footing.cpp src/items/ice_projectiles.cpp src/items/catalog.cpp
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
    src/items/root_relics.cpp src/projectiles/root_drill.cpp src/projectiles/swap.cpp
    src/items/displacement.cpp src/projectiles/hook.cpp src/debris/air.cpp
    src/items/ground_tools.cpp src/entities/bird_feeding.cpp
    src/status/effects.cpp src/items/remedies.cpp
    src/items/woodland_tools.cpp src/props/growth.cpp
    src/surfaces/interaction.cpp
    src/projectiles/projectile.cpp src/items/bow.cpp src/items/action.cpp src/items/material_catalog.cpp src/items/materials.cpp src/items/firearms.cpp
    src/entity.cpp src/combat/damage.cpp src/combat/shove.cpp
    src/combat/melee.cpp src/items/forest.cpp src/items/food.cpp src/items/campfire.cpp src/items/fire.cpp src/projectiles/recoverable.cpp src/items/stacking.cpp src/items/ground_interaction.cpp
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

set(GAUCHE_WORLD_SOURCES src/world/ice_terrain.cpp src/world/floor_layout.cpp src/world/floor_reachability.cpp
    src/world/ground_items.cpp src/world/route_graph.cpp src/world/room_shapes.cpp src/world/room_gates.cpp src/world/forest_terrain.cpp
    src/world/room_population.cpp src/world/encounter.cpp src/world/encounter_spawn.cpp
    src/world/haunted_house.cpp src/world/loot.cpp)

set(GAUCHE_PROP_SOURCES src/props/catalog.cpp src/props/interaction.cpp src/props/placement.cpp)

set(GAUCHE_DEBRIS_SOURCES src/debris/motion.cpp src/debris/sweep.cpp src/debris/scatter.cpp)

add_executable(gauche
    src/main.cpp src/app/options.cpp src/debug/panels.cpp src/ui/status.cpp src/graphics.cpp src/input_device.cpp src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES}
    src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES} src/input.cpp src/input/prompts.cpp
    src/render.cpp src/world/floating_render.cpp src/items/fire_render.cpp src/entities/flight_render.cpp src/entities/plant_render.cpp src/entities/wolf_render.cpp src/entities/intent_render.cpp src/world/wall_render.cpp src/world/encounter_render.cpp src/props/render.cpp src/debris/render.cpp ${GAUCHE_DEBRIS_SOURCES}
    src/lighting/field.cpp src/lighting/canopy.cpp src/lighting/sources.cpp
    src/lighting/render.cpp src/lighting/shafts.cpp src/ui/hud.cpp src/ui/artifacts.cpp src/artifacts/catalog.cpp src/ui/prompts.cpp src/ui/item_details.cpp src/ui/item_meter.cpp
    src/ui/pattern_diagram.cpp src/ui/interaction.cpp
    src/ui/interaction_render.cpp src/ui/range.cpp src/ui/pointer.cpp src/ui/text.cpp
    src/audio.cpp src/ambience/mixer.cpp src/ambience/placement.cpp src/particles/system.cpp src/particles/render.cpp src/particles/water.cpp src/projectiles/render.cpp src/scenery/overhead.cpp src/surfaces/render.cpp src/particles/templates.cpp src/particles/presentation.cpp src/particles/gunfire.cpp src/particles/electricity.cpp src/particles/motion.cpp src/menu_shell.cpp src/rollback.cpp src/net_codec.cpp src/net_socket.cpp
    src/net_protocol.cpp src/net_session.cpp src/net_snapshot.cpp
    src/net_session_host.cpp src/net_session_client.cpp src/menu/front_page.cpp
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
        src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
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
        src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_rollback_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_rollback_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME rollback_rules COMMAND gauche_rollback_tests)
    add_executable(gauche_codec_tests
        tests/net_codec_test.cpp src/net_codec.cpp src/stage.cpp ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES}
        src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
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
        tests/net_session_test.cpp src/net_protocol.cpp src/net_session.cpp
        src/net_snapshot.cpp src/net_session_host.cpp src/net_session_client.cpp
        src/net_socket.cpp src/net_codec.cpp src/rollback.cpp src/stage.cpp
        ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
    target_link_libraries(gauche_session_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_session_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if(WIN32)
        target_link_libraries(gauche_session_tests PRIVATE ws2_32)
    endif()
    add_test(NAME direct_session COMMAND gauche_session_tests)
    add_executable(gauche_loss_tests
        tests/net_loss_test.cpp src/net_protocol.cpp src/net_session.cpp
        src/net_snapshot.cpp src/net_session_host.cpp src/net_session_client.cpp
        src/net_socket.cpp src/net_codec.cpp src/rollback.cpp src/stage.cpp
        ${GAUCHE_ENTITY_SOURCES} ${GAUCHE_PROP_SOURCES} src/inventory.cpp src/item.cpp src/item_attribute.cpp src/item_pattern.cpp src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
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
        src/step.cpp src/game_hash.cpp src/trap.cpp src/run.cpp ${GAUCHE_WORLD_SOURCES})
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
