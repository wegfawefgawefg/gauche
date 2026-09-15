cmake_minimum_required(VERSION 3.24)
project(gauche_room_service LANGUAGES CXX)

include(FetchContent)
find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(GAUCHE_GUBSY_SOURCE_DIR "" CACHE PATH "Optional existing pinned Gubsy source")

# SERVICE: Compile the existing directory/punch/relay without the SDL game host.
if(NOT GAUCHE_GUBSY_SOURCE_DIR)
    FetchContent_Declare(roomd_gubsy
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gubsy.git
        GIT_TAG c4d5408c74e61914f3886099db799919a2637696
        SOURCE_SUBDIR gauche_service_sources_only)
    FetchContent_MakeAvailable(roomd_gubsy)
    set(GAUCHE_GUBSY_SOURCE_DIR "${roomd_gubsy_SOURCE_DIR}")
endif()

add_executable(gubsy-roomd
    "${GAUCHE_GUBSY_SOURCE_DIR}/tools/room_server/main.cpp"
    "${GAUCHE_GUBSY_SOURCE_DIR}/tools/room_server/relay_udp_server.cpp"
    "${GAUCHE_GUBSY_SOURCE_DIR}/src/realnet/config.cpp"
    "${GAUCHE_GUBSY_SOURCE_DIR}/src/realnet/crypto.cpp"
    "${GAUCHE_GUBSY_SOURCE_DIR}/src/realnet/rendezvous.cpp"
    "${GAUCHE_GUBSY_SOURCE_DIR}/src/realnet/relay.cpp")
target_include_directories(gubsy-roomd PRIVATE
    "${GAUCHE_GUBSY_SOURCE_DIR}/include"
    "${GAUCHE_GUBSY_SOURCE_DIR}/third_party")
target_link_libraries(gubsy-roomd PRIVATE Threads::Threads)
if(WIN32)
    target_link_libraries(gubsy-roomd PRIVATE ws2_32)
endif()
install(TARGETS gubsy-roomd RUNTIME DESTINATION bin)
