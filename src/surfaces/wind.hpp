#pragma once
#include "../item_pattern.hpp"
std::vector<Cell> gust_cells(const Game& game,Cell source,Cell direction,ItemPattern pattern);
void blow_surface_air(Game& game,const std::vector<Cell>& cells,Cell direction);
