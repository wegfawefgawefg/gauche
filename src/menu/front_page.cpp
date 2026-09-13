#include "front_page.hpp"
#include "../graphics.hpp"

#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <cstdio>
#include <string_view>
#include <utility>

namespace {

// These recipe values come from the feature/gview-menu-integration stone theme.
gview::PartPresentation stone_part(gview::PresentationState state, gview::Color tint) {
    gview::PartPresentation part;
    part.part = gview::WidgetPart::Frame;
    part.state = state;
    part.asset = "stone-panel";
    part.image_mode = gview::ImageMode::NineSlice;
    part.slice_margins = {41.5F, 41.5F, 41.5F, 41.5F};
    part.slice_scale = 0.67F;
    part.slice_modes = {gview::SliceTileMode::Repeat, gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat, gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat};
    part.tint = tint;
    part.draw_box_underlay = false;
    return part;
}

gview::Theme stone_theme() {
    gview::Theme theme;
    theme.id = "gauche-stone";
    gview::WidgetSkin buttons;
    buttons.control = gview::ControlKind::Button;
    buttons.parts = {
        stone_part(gview::PresentationState::Normal, {205, 205, 200, 255}),
        stone_part(gview::PresentationState::Hovered, {255, 230, 170, 255}),
        stone_part(gview::PresentationState::Focused, {255, 211, 112, 255}),
        stone_part(gview::PresentationState::Pressed, {255, 170, 52, 255}),
    };
    theme.widgets.push_back(std::move(buttons));
    return theme;
}

glayout::GraphNode row(std::string id, float height) {
    glayout::GraphNode node;
    node.id = std::move(id);
    node.size.width = {glayout::LengthKind::Fill, 1.0F};
    node.size.height = {glayout::LengthKind::Pixels, height};
    node.padding = {16.0F, 0.0F, 16.0F, 0.0F};
    return node;
}

gview::View build_view(int width, int height) {
    const float scale = std::clamp(std::min(static_cast<float>(width) / 1280.0F,
                                              static_cast<float>(height) / 720.0F),
                                   0.75F, 1.5F);
    gview::View view;
    view.id = view.label = "gauche-front";
    view.active_theme = "gauche-stone";
    view.themes.push_back(stone_theme());
    view.layout.id = "gauche-front-layout";
    view.layout.width = width;
    view.layout.height = height;
    glayout::GraphNode& root = view.layout.root;
    root.id = "root";
    root.container = glayout::ContainerKind::Stack;
    root.size.width = root.size.height = {glayout::LengthKind::Fill, 1.0F};

    glayout::GraphNode card;
    card.id = "front-card";
    card.container = glayout::ContainerKind::Column;
    card.size.width = {glayout::LengthKind::Pixels, 520.0F * scale};
    card.size.height = {glayout::LengthKind::Pixels, 385.0F * scale};
    card.align = glayout::Align::Center;
    card.gap = 9.0F * scale;
    card.padding = {18.0F * scale, 10.0F * scale, 18.0F * scale, 10.0F * scale};
    gview::NodeSpec card_spec;
    card_spec.layout_id = "front-card";
    card_spec.style.normal.fill = {10, 12, 11, 185};
    card_spec.style.normal.border = {96, 83, 57, 230};
    card_spec.style.normal.border_width = 1.0F;
    view.nodes.push_back(std::move(card_spec));

    glayout::GraphNode title = row("front-title", 100.0F * scale);
    title.padding = {};
    card.children.push_back(std::move(title));
    gview::NodeSpec title_spec;
    title_spec.layout_id = "front-title";
    title_spec.content = gview::ContentKind::Text;
    title_spec.text = "GAUCHE";
    title_spec.text_style.size = 68.0F * scale;
    title_spec.text_style.horizontal = gview::TextAlign::Center;
    title_spec.text_style.vertical = gview::TextAlign::Center;
    title_spec.style.normal.text = {235, 225, 195, 255};
    view.nodes.push_back(std::move(title_spec));

    struct Entry { const char* id; const char* label; const char* action; };
    constexpr Entry entries[]{
        {"front-play", "Play", "play"},
        {"front-quick", "Quick Run", "quick"},
        {"front-settings", "Settings", "settings"},
        {"front-quit", "Quit", "quit"},
    };
    for (const Entry& entry : entries) {
        card.children.push_back(row(entry.id, 51.0F * scale));
        gview::NodeSpec spec;
        spec.layout_id = entry.id;
        spec.content = gview::ContentKind::Text;
        spec.control = gview::ControlKind::Button;
        spec.text = entry.label;
        spec.action = entry.action;
        spec.focus_group = "front-actions";
        spec.focusable = true;
        spec.text_style.size = 26.0F * scale;
        spec.text_style.horizontal = gview::TextAlign::Center;
        spec.text_style.vertical = gview::TextAlign::Center;
        spec.style.normal.text = {225, 225, 215, 255};
        spec.style.hovered.text = {255, 244, 192, 255};
        spec.style.focused.text = {255, 235, 157, 255};
        spec.style.pressed.text = {255, 245, 190, 255};
        view.nodes.push_back(std::move(spec));
    }
    root.children.push_back(std::move(card));
    view.focus_groups.push_back({"front-actions", "front-play", "front-card", true, true});
    return view;
}

void nav(gview::InputFrame& frame, bool down, gview::NavAction action) {
    if (down) frame.navigation.push_back(action);
}

} // namespace

bool init_front_page(FrontPage& page, SDL_Renderer* renderer) {
    if (page.painter) return true;
    const auto root = asset_root();
    page.painter = std::make_unique<gview::Sdl3Renderer>(
        renderer, (root / "fonts" / "PixelOperator.ttf").string());
    if (!page.painter->ready()) return false;
    page.stone = IMG_LoadTexture(renderer, (root / "ui" / "menu" / "stone-panel.png").string().c_str());
    if (page.stone == nullptr) return false;
    page.painter->register_texture("stone-panel", page.stone);
    return true;
}

void shutdown_front_page(FrontPage& page) {
    if (page.painter) page.painter->unregister_texture("stone-panel");
    SDL_DestroyTexture(page.stone);
    page.stone = nullptr;
    page.painter.reset();
}

void front_page_event(FrontPage& page, const SDL_Event& event, const GubsyFrame& frame) {
    if (frame.window == nullptr || frame.render_width <= 0 || frame.render_height <= 0) return;
    float mouse_x = 0.0F;
    float mouse_y = 0.0F;
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mouse_x = event.motion.x; mouse_y = event.motion.y;
        page.input.pointer.moved = true;
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        mouse_x = event.button.x; mouse_y = event.button.y;
        if (event.button.button == SDL_BUTTON_LEFT) {
            page.input.pointer.pressed |= event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            page.input.pointer.released |= event.type == SDL_EVENT_MOUSE_BUTTON_UP;
        }
    } else return;
    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSize(frame.window, &window_width, &window_height);
    const float scale = std::min(static_cast<float>(window_width) /
                                     static_cast<float>(frame.render_width),
                                 static_cast<float>(window_height) /
                                     static_cast<float>(frame.render_height));
    if (scale <= 0.0F) return;
    page.input.pointer.x = (mouse_x - (static_cast<float>(window_width) -
                           static_cast<float>(frame.render_width) * scale) * 0.5F) / scale;
    page.input.pointer.y = (mouse_y - (static_cast<float>(window_height) -
                           static_cast<float>(frame.render_height) * scale) * 0.5F) / scale;
}

FrontAction update_front_page(FrontPage& page, const MenuInputState& input,
                              int width, int height) {
    if (!page.painter) return FrontAction::None;
    if (!page.compiled || width != page.width || height != page.height) {
        const gview::CompileResult result = gview::compile_view(build_view(width, height));
        if (!result.ok) {
            std::fprintf(stderr, "Gauche GView front page compile failed: %s\n",
                         result.diagnostics.empty() ? "unknown" :
                         result.diagnostics.front().message.c_str());
            return FrontAction::None;
        }
        if (page.compiled) page.runtime.reconcile(std::move(result.view));
        else page.runtime.reset(std::move(result.view));
        page.width = width;
        page.height = height;
        page.compiled = true;
    }
    nav(page.input, input.up, gview::NavAction::Up);
    nav(page.input, input.down, gview::NavAction::Down);
    nav(page.input, input.left, gview::NavAction::Left);
    nav(page.input, input.right, gview::NavAction::Right);
    nav(page.input, input.select, gview::NavAction::Confirm);
    gview::Host host;
    host.action = [&page](std::string_view action, gview::NodeIndex) {
        if (action == "play") page.action = FrontAction::Play;
        if (action == "quick") page.action = FrontAction::QuickRun;
        if (action == "settings") page.action = FrontAction::Settings;
        if (action == "quit") page.action = FrontAction::Quit;
    };
    glayout::ResolveInput resolution{};
    resolution.viewport = {0.0F, 0.0F, static_cast<float>(width),
                           static_cast<float>(height)};
    page.runtime.frame(resolution, page.input, host);
    page.input = {};
    const FrontAction action = page.action;
    page.action = FrontAction::None;
    return action;
}

void render_front_page(FrontPage& page) {
    if (page.compiled && page.painter) page.painter->render(page.runtime.paint());
}
