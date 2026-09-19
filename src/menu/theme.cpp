#include "theme.hpp"

namespace teeming_menu {

namespace {

gview::PartPresentation asset_part(gview::WidgetPart part, std::string asset,
                                   gview::ImageMode mode, float opacity = 1.0f,
                                   gview::Color tint = {255, 255, 255, 255}) {
  gview::PartPresentation result;
  result.part = part;
  result.asset = std::move(asset);
  result.image_mode = mode;
  result.opacity = opacity;
  result.tint = tint;
  result.draw_box_underlay = false;
  return result;
}

// What is: Three-slice-style control strips expressed through asymmetric
// nine-slice source caps.
gview::PartPresentation sliced_part(gview::WidgetPart part, std::string asset,
                                    float left, float top, float right,
                                    float bottom) {
  gview::PartPresentation result =
      asset_part(part, std::move(asset), gview::ImageMode::NineSlice);
  result.slice_margins = {left, top, right, bottom};
  return result;
}

// What is: One generated 23 px cut rendered at its authored source scale.
gview::PartPresentation nine_slice_part(gview::WidgetPart part,
                                        std::string asset,
                                        gview::PresentationState state,
                                        float border_scale,
                                        float opacity = 1.0f) {
  gview::PartPresentation result =
      asset_part(part, std::move(asset), gview::ImageMode::NineSlice, opacity);
  result.state = state;
  result.slice = 23.0f;
  result.slice_scale = border_scale;
  return result;
}

// What is: Normal control and region frame shorthand.
gview::PartPresentation nine_slice(std::string asset,
                                   gview::PresentationState state,
                                   float border_scale, float opacity = 1.0f) {
  return nine_slice_part(gview::WidgetPart::Frame, std::move(asset), state,
                         border_scale, opacity);
}

gview::WidgetSkin slider_skin() {
  gview::WidgetSkin skin;
  skin.control = gview::ControlKind::Slider;
  skin.parts = {sliced_part(gview::WidgetPart::Track, "ui-slider-track", 8.0f,
                            3.0f, 8.0f, 3.0f),
                sliced_part(gview::WidgetPart::Fill, "ui-slider-fill", 8.0f,
                            3.0f, 8.0f, 3.0f),
                asset_part(gview::WidgetPart::Thumb, "ui-slider-thumb",
                           gview::ImageMode::Contain)};
  return skin;
}

// What is: Value imagery independent from the toggle row's hover and focus.
gview::WidgetSkin toggle_indicator_skin() {
  gview::WidgetSkin skin;
  skin.control = gview::ControlKind::Toggle;
  auto off = asset_part(gview::WidgetPart::Indicator, "ui-toggle-off",
                        gview::ImageMode::Contain);
  off.state = gview::PresentationState::Off;
  auto on = asset_part(gview::WidgetPart::Indicator, "ui-toggle-on",
                       gview::ImageMode::Contain);
  on.state = gview::PresentationState::On;
  skin.parts = {std::move(off), std::move(on)};
  return skin;
}

// What is: Passive scrollbar presentation shared by every clipped scroll area.
gview::WidgetSkin scrollbar_skin() {
  gview::WidgetSkin skin;
  skin.control = gview::ControlKind::ScrollArea;
  skin.parts = {sliced_part(gview::WidgetPart::Track, "ui-scrollbar-track",
                            4.0f, 8.0f, 4.0f, 8.0f),
                sliced_part(gview::WidgetPart::Thumb, "ui-scrollbar-thumb",
                            4.0f, 8.0f, 4.0f, 8.0f)};
  return skin;
}

gview::WidgetSkin control_skin(gview::ControlKind control) {
  gview::WidgetSkin skin;
  skin.control = control;
  constexpr float button_border = 1.0f;
  skin.parts = {
      nine_slice("ui-button-light", gview::PresentationState::Normal,
                 button_border),
      nine_slice("ui-button-light", gview::PresentationState::Hovered,
                 button_border),
      nine_slice("ui-action-green", gview::PresentationState::Focused,
                 button_border),
      nine_slice("ui-action-green-dark", gview::PresentationState::Selected,
                 button_border),
      nine_slice("ui-action-green", gview::PresentationState::SelectedFocused,
                 button_border),
      nine_slice("ui-action-green", gview::PresentationState::Pressed,
                 button_border),
      nine_slice("ui-action-green", gview::PresentationState::Open,
                 button_border),
      nine_slice("ui-button-light", gview::PresentationState::Disabled,
                 button_border, 0.55f)};
  if (control == gview::ControlKind::Select) {
    auto popup =
        nine_slice_part(gview::WidgetPart::Popup, "ui-action-green",
                        gview::PresentationState::Normal, button_border);
    skin.parts.push_back(std::move(popup));
    skin.parts.push_back(
        nine_slice_part(gview::WidgetPart::Option, "ui-button-light",
                        gview::PresentationState::Normal, button_border));
    skin.parts.push_back(
        nine_slice_part(gview::WidgetPart::Option, "ui-action-green",
                        gview::PresentationState::Selected, button_border));
    skin.parts.push_back(sliced_part(gview::WidgetPart::Track,
                                     "ui-scrollbar-track", 4.0f, 8.0f, 4.0f,
                                     8.0f));
    skin.parts.push_back(sliced_part(gview::WidgetPart::Thumb,
                                     "ui-scrollbar-thumb", 4.0f, 8.0f, 4.0f,
                                     8.0f));
  }
  return skin;
}

// What is: A semantic region recipe shared by every matching layout node.
gview::WidgetSkin region_skin(std::string style_class, std::string asset,
                              float border_scale) {
  gview::WidgetSkin skin;
  skin.any_control = true;
  skin.style_class = std::move(style_class);
  skin.parts = {nine_slice(std::move(asset), gview::PresentationState::Normal,
                           border_scale)};
  return skin;
}

gview::BoxStyle text_box(gview::Color text) {
  gview::BoxStyle result;
  result.text = text;
  return result;
}

constexpr char stone_surface[] = "ui-stone-large-panel";
constexpr gview::SliceMargins stone_slice{41.5F, 41.5F, 41.5F, 41.5F};
constexpr float stone_scale = 0.48F;

constexpr gview::Color normal_stone{205, 205, 200, 255};
constexpr gview::Color hovered_stone{255, 230, 170, 255};
constexpr gview::Color focused_stone{255, 211, 112, 255};
constexpr gview::Color selected_stone{236, 194, 111, 255};
constexpr gview::Color active_stone{255, 196, 74, 255};
constexpr gview::Color pressed_stone{255, 170, 52, 255};
constexpr gview::Color disabled_stone{125, 125, 122, 255};

gview::PartPresentation stone_part(gview::WidgetPart part,
                                   gview::PresentationState state,
                                   gview::Color tint, gview::Color text,
                                   float opacity = 1.0F) {
  gview::PartPresentation result =
      asset_part(part, stone_surface, gview::ImageMode::NineSlice, opacity, tint);
  result.state = state;
  result.slice_margins = stone_slice;
  result.slice_scale = stone_scale;
  result.slice_modes = {gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat,
                        gview::SliceTileMode::Repeat};
  result.override_box = true;
  result.box = text_box(text);
  // The stone surface already contains an opaque center. Drawing a rectangular
  // box beneath it makes the transparent pixels around its irregular border
  // appear as a dark rectangular halo against the title scene.
  result.draw_box_underlay = false;
  return result;
}

gview::WidgetSkin stone_control_skin(gview::ControlKind control) {
  constexpr gview::Color normal_text{222, 222, 216, 255};
  constexpr gview::Color selected_text{255, 235, 157, 255};
  gview::WidgetSkin skin;
  skin.control = control;
  skin.parts = {
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Normal,
                 normal_stone, normal_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Hovered,
                 hovered_stone, normal_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Focused,
                 focused_stone, selected_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Selected,
                 selected_stone, selected_text),
      stone_part(gview::WidgetPart::Frame,
                 gview::PresentationState::SelectedFocused, active_stone,
                 selected_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Pressed,
                 pressed_stone, selected_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Open,
                 active_stone, selected_text),
      stone_part(gview::WidgetPart::Frame, gview::PresentationState::Disabled,
                 disabled_stone, gview::Color{142, 143, 140, 255}, 0.72F),
  };
  if (control == gview::ControlKind::Select) {
    skin.parts.push_back(stone_part(gview::WidgetPart::Popup,
                                    gview::PresentationState::Normal,
                                    normal_stone, normal_text));
    skin.parts.push_back(stone_part(gview::WidgetPart::Option,
                                    gview::PresentationState::Normal,
                                    normal_stone, normal_text));
    skin.parts.push_back(stone_part(gview::WidgetPart::Option,
                                    gview::PresentationState::Selected,
                                    active_stone, selected_text));
    auto indicator =
        asset_part(gview::WidgetPart::Indicator, "ui-stone-select-down",
                   gview::ImageMode::Contain);
    skin.parts.push_back(std::move(indicator));
    auto active_indicator =
        asset_part(gview::WidgetPart::Indicator, "ui-stone-select-down-active",
                   gview::ImageMode::Contain);
    active_indicator.state = gview::PresentationState::Focused;
    skin.parts.push_back(std::move(active_indicator));
  }
  return skin;
}

gview::WidgetSkin stone_toggle_indicator_skin() {
  gview::WidgetSkin skin;
  skin.control = gview::ControlKind::Toggle;
  auto off = asset_part(gview::WidgetPart::Indicator, "ui-stone-toggle-off",
                        gview::ImageMode::Contain);
  off.state = gview::PresentationState::Off;
  auto on = asset_part(gview::WidgetPart::Indicator, "ui-stone-toggle-on",
                       gview::ImageMode::Contain);
  on.state = gview::PresentationState::On;
  skin.parts = {std::move(off), std::move(on)};
  return skin;
}

gview::WidgetSkin stone_region_skin(std::string style_class) {
  gview::WidgetSkin skin;
  skin.any_control = true;
  skin.style_class = std::move(style_class);
  skin.parts = {stone_part(gview::WidgetPart::Frame,
                           gview::PresentationState::Normal, {255, 255, 255, 255},
                           {222, 222, 216, 255})};
  return skin;
}

gview::WidgetSkin stone_text_skin() {
  gview::WidgetSkin skin;
  skin.any_control = true;
  skin.style_class = "menu-label";
  gview::PartPresentation frame;
  frame.part = gview::WidgetPart::Frame;
  frame.override_box = true;
  frame.box = text_box({222, 222, 216, 255});
  frame.draw_box_underlay = false;
  skin.parts.push_back(std::move(frame));
  return skin;
}

} // namespace

// Defines the neutral trial skin through the same public recipe API as games.
std::vector<gview::Theme> default_themes() {
  gview::Theme base;
  base.id = "gubsy-default";
  base.widgets = {control_skin(gview::ControlKind::Button),
                  control_skin(gview::ControlKind::Toggle),
                  control_skin(gview::ControlKind::Slider),
                  control_skin(gview::ControlKind::Select),
                  control_skin(gview::ControlKind::TextInput),
                  slider_skin(),
                  toggle_indicator_skin(),
                  scrollbar_skin()};
  gview::Theme game;
  game.id = "gubsy-default-game";
  game.extends = "gubsy-default";
  game.widgets = {
      region_skin("bar-dark", "ui-bar-dark", 1.0f),
      region_skin("parchment-ornate", "ui-parchment-ornate", 1.0f),
      region_skin("group-inner", "ui-group-inner", 1.0f),
  };
  gview::Theme stone;
  stone.id = "teeming-stone-unified";
  stone.extends = "gubsy-default";
  stone.widgets = {
      stone_text_skin(),
      stone_control_skin(gview::ControlKind::Button),
      stone_control_skin(gview::ControlKind::Toggle),
      stone_control_skin(gview::ControlKind::Slider),
      stone_control_skin(gview::ControlKind::Select),
      stone_control_skin(gview::ControlKind::TextInput),
      stone_toggle_indicator_skin(),
      stone_region_skin("bar-dark"),
      stone_region_skin("parchment-ornate"),
      stone_region_skin("group-inner"),
      stone_region_skin("hud-bar"),
      stone_region_skin("large-main-panel"),
      stone_region_skin("ornate-inner-group"),
      stone_region_skin("modal-frame"),
      stone_region_skin("toast-frame"),
  };
  return {std::move(base), std::move(game), std::move(stone)};
}

} // namespace teeming_menu
