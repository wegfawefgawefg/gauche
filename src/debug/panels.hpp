#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

struct DebugPanels {
    bool visible = false;
    bool selector = true;
    bool presentation = false;
    bool combat = false;
    bool status = false;
    bool unlocked_zoom = false;
    int canopy_style = 1; // 0: stipple, 1: mask, 2: full, 3: hidden, 4: object fade, 5: slide
    bool canopy_upright = true;
    float canopy_opening = .83F;
    float canopy_fade = .25F;
    int canopy_reference = 0; // 0: sprite center, 1: root
    float canopy_near = .45F;
    float canopy_transition = .40F;
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
