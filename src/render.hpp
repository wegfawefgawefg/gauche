#pragma once

#include "game.hpp"
#include "view.hpp"

struct WorldRenderOptions {
    ViewCamera camera{};
    bool fullbright = true, overhead = false, roofs = false;
};

struct Cosmetics;
struct PointerState;

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, float zoom,
                 const Cosmetics* cosmetics, const PointerState& pointer,
                 bool show_hud = true, bool compact_details = false,
                 const WorldRenderOptions* inspection = nullptr);
void render_title_backdrop(SDL_Renderer* renderer, const GameGraphics& graphics,
                           const Game& scene);
