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
        GIT_TAG f95d5644fc254d85d8406a47df23def8f89dc0dd)
    FetchContent_MakeAvailable(gubsy)
    set(GAUCHE_GUBSY_DIR "${gubsy_SOURCE_DIR}")
endif()

add_executable(gauche
    src/main.cpp src/graphics.cpp src/stage.cpp src/entity.cpp
    src/inventory.cpp src/item.cpp src/step.cpp src/run.cpp src/input.cpp
    src/render.cpp src/audio.cpp)
target_link_libraries(gauche PRIVATE gubsy::engine SDL3::SDL3
    SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer)
target_compile_definitions(gauche PRIVATE
    GAUCHE_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    GAUCHE_GUBSY_ASSETS_DIR="${GAUCHE_GUBSY_DIR}/src/assets")

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
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:gauche>/assets")

include(CTest)
if(BUILD_TESTING)
    add_executable(gauche_tests
        tests/game_test.cpp src/stage.cpp src/entity.cpp src/inventory.cpp
        src/item.cpp src/step.cpp src/run.cpp)
    target_link_libraries(gauche_tests PRIVATE SDL3::SDL3)
    target_compile_options(gauche_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    add_test(NAME game_rules COMMAND gauche_tests)
endif()
