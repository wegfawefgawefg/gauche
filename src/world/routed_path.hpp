#pragma once
#include "../game.hpp"
#include <span>
// Deterministic four-neighbor route through positive cell costs; zero is blocked.
std::vector<Cell> route_cost_field(Cell from,Cell to,int width,int height,std::span<const unsigned> costs);

// Join successive waypoints into a simple cardinal cycle; no repeated end cell.
std::vector<Cell> route_closed_cost_field(std::span<const Cell> guide,int width,int height,
                                         std::span<const unsigned> costs);
