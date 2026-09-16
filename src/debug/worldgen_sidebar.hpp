#pragma once
#include "worldgen.hpp"
void focus_worldgen_selection(WorldGenViewer& viewer,const GenerationReport& report,bool component);
void focus_worldgen_bounds(WorldGenViewer& viewer,Cell low,Cell high,float max_zoom=3.0F);
void draw_worldgen_sidebar(SDL_Renderer* renderer,const WorldGenViewer& viewer,const GenerationReport& report);
bool worldgen_sidebar_event(const SDL_Event& event,WorldGenViewer& viewer);
