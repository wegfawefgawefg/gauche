#pragma once
#include "../game.hpp"
#include <span>
// Deterministic four-neighbor route through positive cell costs; zero is blocked.
std::vector<Cell> route_cost_field(Cell from,Cell to,int width,int height,std::span<const unsigned> costs);
