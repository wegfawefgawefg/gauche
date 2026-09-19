cmake_minimum_required(VERSION 3.24)
project(teeming_room_service LANGUAGES CXX)

include(FetchContent)
include(../../cmake/legacy_options.cmake)
find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(TEEMING_GUBSY_SOURCE_DIR "" CACHE PATH "Optional existing pinned Gubsy source")

# SERVICE: Compile the existing directory/punch/relay without the SDL game host.
if(NOT TEEMING_GUBSY_SOURCE_DIR)
    FetchContent_Declare(roomd_gubsy
        GIT_REPOSITORY https://github.com/wegfawefgawefg/gubsy.git
        GIT_TAG c4d5408c74e61914f3886099db799919a2637696
        SOURCE_SUBDIR teeming_service_sources_only)
    FetchContent_MakeAvailable(roomd_gubsy)
    set(TEEMING_GUBSY_SOURCE_DIR "${roomd_gubsy_SOURCE_DIR}")
endif()

add_executable(gubsy-roomd
    "${TEEMING_GUBSY_SOURCE_DIR}/tools/room_server/main.cpp"
    "${TEEMING_GUBSY_SOURCE_DIR}/tools/room_server/relay_udp_server.cpp"
    "${TEEMING_GUBSY_SOURCE_DIR}/src/realnet/config.cpp"
    "${TEEMING_GUBSY_SOURCE_DIR}/src/realnet/crypto.cpp"
    "${TEEMING_GUBSY_SOURCE_DIR}/src/realnet/rendezvous.cpp"
    "${TEEMING_GUBSY_SOURCE_DIR}/src/realnet/relay.cpp")
target_include_directories(gubsy-roomd PRIVATE
    "${TEEMING_GUBSY_SOURCE_DIR}/include"
    "${TEEMING_GUBSY_SOURCE_DIR}/third_party")
target_link_libraries(gubsy-roomd PRIVATE Threads::Threads)
if(WIN32)
    target_link_libraries(gubsy-roomd PRIVATE ws2_32)
endif()
install(TARGETS gubsy-roomd RUNTIME DESTINATION bin)
