#pragma once
#include "../game.hpp"
#include <array>
#include <string>

struct TestLevel { const char* name; int floor; FloorLayout layout; };
inline constexpr std::array<TestLevel, 13> test_levels{{
    {"Forest 1-1",1,FloorLayout::Generated}, {"Forest 1-2",2,FloorLayout::Generated},
    {"Forest 1-3",3,FloorLayout::Generated}, {"Forest 1-4",4,FloorLayout::Generated},
    {"Fire 2-1",5,FloorLayout::Generated}, {"Fire 2-2",6,FloorLayout::Generated},
    {"Fire 2-3",7,FloorLayout::Generated}, {"Fire 2-4",8,FloorLayout::Generated},
    {"Ice 3-1",9,FloorLayout::Generated}, {"Ice 3-2",10,FloorLayout::Generated},
    {"Ice 3-3",11,FloorLayout::Generated}, {"Ice 3-4",12,FloorLayout::Generated},
    {"Haunted House (Forest 1-3)",3,FloorLayout::HauntedHouse},
}};
void jump_to_test_level(Game& game, int selection);

std::string playtest_summary();
