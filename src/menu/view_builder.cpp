#include "view_builder.hpp"

#include "theme.hpp"

#include <algorithm>
#include <unordered_set>

namespace teeming_menu {

namespace {

gview::BoxStyle box(gview::Color fill, gview::Color border, gview::Color text) {
    gview::BoxStyle style;
    style.fill = fill;
    style.border = border;
    style.text = text;
    style.border_width = 1.0f;
    return style;
}

glayout::Length scale_pixels(glayout::Length length, float scale) {
    if (length.kind == glayout::LengthKind::Pixels)
        length.value *= scale;
    return length;
}

} // namespace

// Builds one resolution-aware view through public GLayout/GView authoring
// structs.
ViewBuilder::ViewBuilder(std::string id, int width, int height) {
    width_ = width;
    height_ = height;
    scale_ = std::clamp(
        std::min(static_cast<float>(width) / 1280.0f, static_cast<float>(height) / 720.0f), 0.75f,
        1.5f) * 0.72F;
    view_.id = std::move(id);
    view_.label = view_.id;
    view_.layout.id = view_.id + "_layout";
    view_.layout.width = width;
    view_.layout.height = height;
    view_.layout.root.id = "root";
    view_.layout.root.container = glayout::ContainerKind::Stack;
    view_.themes = default_themes();
    view_.active_theme = "teeming-stone-unified";
}

float ViewBuilder::scale() const {
    return scale_;
}
bool ViewBuilder::compact() const {
    return width_ < 1000 || view_.layout.height < 600;
}
bool ViewBuilder::phone() const {
    return width_ < 600 || static_cast<float>(height_) > static_cast<float>(width_) * 1.25f;
}
glayout::ContainerKind ViewBuilder::split() const {
    return phone() ? glayout::ContainerKind::Column : glayout::ContainerKind::Row;
}
gview::View ViewBuilder::finish() {
    return std::move(view_);
}

glayout::GraphNode& ViewBuilder::layout(std::string_view id) {
    return *glayout::find_graph_node(view_.layout, id);
}

gview::NodeSpec& ViewBuilder::spec(std::string_view id) {
    const auto found =
        std::find_if(view_.nodes.begin(), view_.nodes.end(),
                     [&](const gview::NodeSpec& item) { return item.layout_id == id; });
    return *found;
}

// What is: The one shallow tab strip used below the page title.
void ViewBuilder::subnavigation(std::string_view parent, std::string_view base,
                                std::string_view action_prefix, std::string_view selected_label,
                                std::vector<std::string> labels) {
    const std::string tabs = std::string(base) + "-tabs";
    layout_container(parent, tabs, glayout::ContainerKind::Row, {glayout::LengthKind::Fill, 1.0F},
                     {glayout::LengthKind::Pixels, 48.0F}, 4.0F);
    for (const std::string& label : labels) {
        const std::string id = std::string(base) + "-tab-" + label;
        button(tabs, id, label, std::string(action_prefix) + label, tabs, 48.0F,
               gview::ActivationPolicy::OnFocus);
        spec(id).selected = label == selected_label;
    }
}

// What is: The shared split and pane geometry used by menu work areas.
void ViewBuilder::workspace(std::string_view parent, std::string id) {
    layout_container(parent, std::move(id), split(), {glayout::LengthKind::Fill, 1.0F},
                     {glayout::LengthKind::Fill, 1.0F}, 16.0F);
}

void ViewBuilder::workspace_pane(std::string_view parent, std::string id, glayout::Length width) {
    const std::string pane_id = id;
    container(parent, std::move(id), glayout::ContainerKind::Column, width,
              {glayout::LengthKind::Fill, 1.0F}, 8.0F, {14.0F, 12.0F, 14.0F, 12.0F});
    spec(pane_id).style_class = "large-main-panel";
}

gview::NodeSpec ViewBuilder::base_spec(std::string id) const {
    gview::NodeSpec spec;
    spec.layout_id = std::move(id);
    const gview::BoxStyle normal =
        box({244, 236, 211, 255}, {124, 101, 66, 255}, {58, 42, 24, 255});
    spec.style.normal = normal;
    spec.style.selected = box({65, 72, 35, 255}, {193, 158, 70, 255}, {255, 226, 145, 255});
    spec.style.hovered = box({239, 226, 193, 255}, {145, 115, 73, 255}, {48, 34, 20, 255});
    spec.style.focused = box({65, 72, 35, 255}, {218, 179, 78, 255}, {255, 230, 153, 255});
    spec.style.selected_focused = box({76, 82, 39, 255}, {229, 189, 82, 255}, {255, 237, 170, 255});
    spec.style.pressed = box({83, 88, 42, 255}, {238, 200, 98, 255}, {255, 241, 183, 255});
    spec.style.disabled = box({218, 208, 183, 255}, {145, 130, 102, 255}, {132, 118, 94, 255});
    spec.text_style.size = 16.0f * scale_;
    spec.text_style.vertical = gview::TextAlign::Center;
    spec.text_style.wrap = true;
    return spec;
}

void ViewBuilder::append(std::string_view parent, glayout::GraphNode node, gview::NodeSpec spec) {
    glayout::GraphNode* target = glayout::find_graph_node(view_.layout, parent);
    target->children.push_back(std::move(node));
    view_.nodes.push_back(std::move(spec));
}

void ViewBuilder::append_layout(std::string_view parent, glayout::GraphNode node) {
    glayout::GraphNode* target = glayout::find_graph_node(view_.layout, parent);
    target->children.push_back(std::move(node));
}

// What is: Shared geometry construction for visible and layout-only regions.
glayout::GraphNode ViewBuilder::container_node(std::string id, glayout::ContainerKind kind,
                                               glayout::Length width, glayout::Length height,
                                               float gap, glayout::Insets padding) const {
    glayout::GraphNode node;
    node.id = std::move(id);
    node.container = kind;
    node.size.width = scale_pixels(width, scale_);
    node.size.height = scale_pixels(height, scale_);
    node.gap = gap * scale_;
    node.padding = {padding.left * scale_, padding.top * scale_, padding.right * scale_,
                    padding.bottom * scale_};
    return node;
}

// What is: A region that participates in both geometry and presentation.
void ViewBuilder::container(std::string_view parent, std::string id, glayout::ContainerKind kind,
                            glayout::Length width, glayout::Length height, float gap,
                            glayout::Insets padding) {
    glayout::GraphNode node = container_node(id, kind, width, height, gap, padding);
    gview::NodeSpec spec = base_spec(id);
    spec.style_class = "group-inner";
    append(parent, std::move(node), std::move(spec));
}

// What is: Pure layout structure with no paint or interaction node.
void ViewBuilder::layout_container(std::string_view parent, std::string id,
                                   glayout::ContainerKind kind, glayout::Length width,
                                   glayout::Length height, float gap, glayout::Insets padding) {
    append_layout(parent, container_node(std::move(id), kind, width, height, gap, padding));
}

void ViewBuilder::label(std::string_view parent, std::string id, std::string text, float height,
                        float size, gview::TextAlign align) {
    glayout::GraphNode node;
    node.id = id;
    node.size.width = {glayout::LengthKind::Fill, 1.0f};
    node.size.height = {glayout::LengthKind::Pixels, height * scale_};
    node.padding = {12.0f * scale_, 0.0f, 12.0f * scale_, 0.0f};
    gview::NodeSpec spec = base_spec(id);
    spec.content = gview::ContentKind::Text;
    spec.style_class = "menu-label";
    spec.text = std::move(text);
    spec.text_style.size = size * scale_;
    spec.text_style.horizontal = align;
    spec.style.normal.fill = {0, 0, 0, 0};
    spec.style.normal.border = {0, 0, 0, 0};
    append(parent, std::move(node), std::move(spec));
}

void ViewBuilder::button(std::string_view parent, std::string id, std::string text,
                         std::string action, std::string group, float height,
                         gview::ActivationPolicy activation) {
    glayout::GraphNode node;
    node.id = id;
    node.size.width = {glayout::LengthKind::Fill, 1.0f};
    node.size.height = {glayout::LengthKind::Pixels, height * scale_};
    node.padding = {14.0f * scale_, 0.0f, 14.0f * scale_, 0.0f};
    gview::NodeSpec spec = base_spec(id);
    spec.content = gview::ContentKind::Text;
    spec.control = gview::ControlKind::Button;
    spec.text = std::move(text);
    spec.action = std::move(action);
    spec.focus_group = std::move(group);
    spec.focusable = true;
    spec.activation = activation;
    append(parent, std::move(node), std::move(spec));
}

void ViewBuilder::toggle(std::string_view parent, std::string id, std::string text,
                         std::string binding, std::string group, float height) {
    button(parent, id, std::move(text), {}, std::move(group), height);
    gview::NodeSpec& spec = view_.nodes.back();
    spec.control = gview::ControlKind::Toggle;
    spec.binding = std::move(binding);
}

void ViewBuilder::slider(std::string_view parent, std::string id, std::string text,
                         std::string binding, std::string group, double minimum, double maximum,
                         double step, float height) {
    button(parent, id, std::move(text), {}, std::move(group), height);
    gview::NodeSpec& spec = view_.nodes.back();
    spec.control = gview::ControlKind::Slider;
    spec.binding = std::move(binding);
    spec.minimum = minimum;
    spec.maximum = maximum;
    spec.step = step;
}

void ViewBuilder::select(std::string_view parent, std::string id, std::string text,
                         std::string binding, std::vector<std::string> options, std::string group,
                         float height) {
    button(parent, id, std::move(text), {}, std::move(group), height);
    gview::NodeSpec& spec = view_.nodes.back();
    spec.control = gview::ControlKind::Select;
    spec.binding = std::move(binding);
    for (const std::string& option : options)
        spec.options.push_back(gview::SelectOption{option, option, option});
}

void ViewBuilder::text_input(std::string_view parent, std::string id, std::string label,
                             std::string binding, std::string group, float height,
                             std::string placeholder) {
    button(parent, std::move(id), std::move(label), {}, std::move(group), height);
    gview::NodeSpec& spec = view_.nodes.back();
    spec.control = gview::ControlKind::TextInput;
    spec.binding = std::move(binding);
    spec.placeholder = std::move(placeholder);
}

void ViewBuilder::image(std::string_view parent, std::string id, std::string asset, float height) {
    label(parent, id, {}, height);
    view_.nodes.back().content = gview::ContentKind::Image;
    view_.nodes.back().asset = std::move(asset);
}

void ViewBuilder::surface(std::string_view parent, std::string id, std::string asset) {
    container(parent, id, glayout::ContainerKind::Absolute, {glayout::LengthKind::Fill, 1.0f},
              {glayout::LengthKind::Fill, 1.0f});
    gview::NodeSpec& item = spec(id);
    item.content = gview::ContentKind::CustomSurface;
    item.asset = std::move(asset);
    item.style.normal.fill = {0, 0, 0, 0};
    item.style.normal.border = {0, 0, 0, 0};
    item.style_class.clear();
}

void ViewBuilder::surface(std::string_view parent, std::string id, std::string asset,
                          float height) {
    label(parent, std::move(id), {}, height);
    gview::NodeSpec& item = view_.nodes.back();
    item.content = gview::ContentKind::CustomSurface;
    item.asset = std::move(asset);
    item.style.normal.fill = {0, 0, 0, 0};
    item.style.normal.border = {0, 0, 0, 0};
    item.style_class.clear();
}

void ViewBuilder::focus_group(std::string id, std::string entry, std::string owner) {
    view_.focus_groups.push_back({std::move(id), std::move(entry), std::move(owner), true, true});
}

// Treats a layout subtree as one local navigation scope without coupling its
// content identities to the focus graph.
void ViewBuilder::focus_scope(std::string_view root, std::string group) {
    const glayout::GraphNode* scope = glayout::find_graph_node(view_.layout, root);
    if (!scope)
        return;
    std::unordered_set<std::string> ids;
    const auto collect = [&](const auto& self, const glayout::GraphNode& node) -> void {
        ids.insert(node.id);
        for (const glayout::GraphNode& child : node.children)
            self(self, child);
    };
    collect(collect, *scope);
    for (gview::NodeSpec& node : view_.nodes)
        if (node.focusable && ids.contains(node.layout_id))
            node.focus_group = group;
}

void ViewBuilder::edge(std::string from, gview::NavAction action, std::string to) {
    view_.focus_edges.push_back({std::move(from), action, std::move(to)});
}

void ViewBuilder::group_edge(std::string from, gview::NavAction action, std::string to) {
    view_.focus_group_edges.push_back({std::move(from), action, std::move(to)});
}

void ViewBuilder::scrolling(std::string_view id) {
    glayout::GraphNode& node = layout(id);
    node.clip = true;

    // What is: A dedicated right gutter for the passive scrollbar. The
    // owning pane's padding remains authoritative so scrollable and static
    // workspaces align to the same left and top edges.
    const float scrollbar_gutter = 23.0f * scale_;
    node.padding.right = std::max(node.padding.right, scrollbar_gutter);

    const auto found =
        std::find_if(view_.nodes.begin(), view_.nodes.end(),
                     [&](const gview::NodeSpec& item) { return item.layout_id == id; });
    if (found != view_.nodes.end()) {
        found->control = gview::ControlKind::ScrollArea;
        return;
    }

    // What is: A paint-free semantic scroll host for otherwise structural
    // layout nodes such as the compact shell content area.
    gview::NodeSpec scroll = base_spec(std::string(id));
    scroll.control = gview::ControlKind::ScrollArea;
    scroll.style.normal.fill = {0, 0, 0, 0};
    scroll.style.normal.border = {0, 0, 0, 0};
    scroll.style.normal.border_width = 0.0f;
    view_.nodes.push_back(std::move(scroll));
}

} // namespace teeming_menu
