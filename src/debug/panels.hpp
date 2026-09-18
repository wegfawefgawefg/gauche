#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

struct DebugPanels {
    bool visible = false;
    bool selector = true;
    bool combat = false;
    bool status = false;
    bool unlocked_zoom = false;
    bool contact_shadows = true;
    bool shadow_entities = true;
    bool shadow_props = true;
    bool shadow_debris = true;
    bool world_items = false;
    bool world_enemies = false;
};

DebugPanels& debug_panels();
void init_debug_panels(SDL_Window* window, tr::Renderer* renderer);
void shutdown_debug_panels();
bool debug_event(const SDL_Event& event);
void draw_debug_panels(const Game& game, int owner, bool offline = true);
bool debug_captures_input();
