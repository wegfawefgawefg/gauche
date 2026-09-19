#include "page_chrome.hpp"
#include <algorithm>

namespace teeming_menu {

// PAGE CHROME: Shared title, content area and quiet bottom-left navigation.
void frame(ViewBuilder& ui, std::string title, float width, float height) {
    ui.layout_container("root", "shell", glayout::ContainerKind::Stack,
                        {glayout::LengthKind::Fill, 1.0F},
                        {glayout::LengthKind::Fill, 1.0F});
    ui.container("shell", "card", glayout::ContainerKind::Column,
                 {glayout::LengthKind::Pixels, width},
                 {glayout::LengthKind::Pixels, height}, 8.0F,
                 {24.0F, 20.0F, 24.0F, 20.0F});
    ui.layout("card").align = glayout::Align::Center;
    ui.spec("card").style_class = "large-main-panel";
    ui.label("card", "heading", std::move(title), 70.0F, 36.0F,
             gview::TextAlign::Center);
}

void button(ViewBuilder& ui, std::string id, std::string label,
            std::string action, float height) {
    const std::string node_id = id;
    ui.button("card", std::move(id), std::move(label), std::move(action),
              "menu", height);
    ui.spec(node_id).text_style.size = 19.0F * ui.scale();
}

void footer(ViewBuilder& ui, std::string first) {
    // FOOTER: Fill unused space, but let an existing scrolling body own its height.
    const auto& children = ui.layout("card").children;
    if (std::none_of(children.begin(), children.end(), [](const auto& node) {
            return node.size.height.kind == glayout::LengthKind::Fill;
        }))
        ui.layout_container("card", "footer-space", glayout::ContainerKind::Stack,
            {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Fill, 1});
    ui.layout_container("card", "footer", glayout::ContainerKind::Row,
        {glayout::LengthKind::Fill, 1}, {glayout::LengthKind::Pixels, 40});
    ui.button("footer", "back", "Back", "back", "menu", 40);
    ui.layout("back").size.width = {glayout::LengthKind::Pixels, 112 * ui.scale()};
    ui.focus_group("menu", std::move(first), "card");
}

} // namespace teeming_menu
