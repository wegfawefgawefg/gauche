#pragma once
#include "../game.hpp"
#include <array>
#include <string>

struct TestLevel { const char* name; int floor; FloorLayout layout; };
inline constexpr std::array<TestLevel, 15> test_levels{{
    {"Forest 1-1",1,FloorLayout::Generated}, {"Forest 1-2",2,FloorLayout::Generated},
    {"Forest 1-3",3,FloorLayout::Generated}, {"Forest 1-4",4,FloorLayout::Generated},
    {"Ice 2-1",5,FloorLayout::Generated}, {"Ice 2-2",6,FloorLayout::Generated},
    {"Ice 2-3",7,FloorLayout::Generated}, {"Ice 2-4",8,FloorLayout::Generated},
    {"Industrial 3-1",9,FloorLayout::Generated}, {"Industrial 3-2",10,FloorLayout::Generated},
    {"Industrial 3-3",11,FloorLayout::Generated}, {"Industrial 3-4",12,FloorLayout::Generated},
    {"Haunted House (Forest 1-3)",3,FloorLayout::HauntedHouse},
    {"Freight Exchange (Industrial 3-2)",10,FloorLayout::FreightExchange},
    {"The Last Shift (Industrial 3-4)",12,FloorLayout::LastShift},
}};
void jump_to_test_level(Game& game, int selection);

std::string playtest_summary();
