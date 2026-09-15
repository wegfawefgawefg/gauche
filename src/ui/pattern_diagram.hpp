#pragma once

#include "../item_pattern.hpp"

struct PatternDiagramLayout {
    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    int columns = 0, rows = 0;
    float x = 0.0F, y = 0.0F, cell_size = 0.0F;
};

PatternDiagramLayout pattern_diagram_layout(ItemPattern pattern,
                                             float x, float y,
                                             float width, float height);
void draw_pattern_diagram(SDL_Renderer* renderer, const Item& item,
                          float x, float y, float width, float height, const Entity* user = nullptr);
