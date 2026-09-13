#pragma once

#include "../game.hpp"

constexpr int room_width = 12;
constexpr int room_height = 10;

void carve_room_area(Stage& stage, int left, int top, int right, int bottom, TileKind kind);
void stamp_room(Game& game, int column, int row, bool main_route);
void connect_branch(Game& game, int column, int row);
void place_ground_item(Game& game, Cell cell, ItemKind kind, int count = 1);
void populate_floor(Game& game, int columns, Cell branch, Cell extra);
