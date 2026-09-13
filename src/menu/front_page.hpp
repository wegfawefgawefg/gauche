#pragma once

#include <gview/gview.hpp>
#include <gview/sdl3_renderer.hpp>
#include <gubsy/runtime.hpp>

#include <memory>

// The front page uses the GView renderer and stone controls from the Splonks UI workspace.
enum class FrontAction { None, Play, QuickRun, Settings, Quit };

struct FrontPage {
    gview::Runtime runtime{};
    std::unique_ptr<gview::Sdl3Renderer> painter;
    SDL_Texture* stone = nullptr;
    gview::InputFrame input{};
    FrontAction action = FrontAction::None;
    int width = 0;
    int height = 0;
    bool compiled = false;
};

bool init_front_page(FrontPage& page, SDL_Renderer* renderer);
void shutdown_front_page(FrontPage& page);
void front_page_event(FrontPage& page, const SDL_Event& event, const GubsyFrame& frame);
FrontAction update_front_page(FrontPage& page, const MenuInputState& input,
                              int width, int height);
void render_front_page(FrontPage& page);
