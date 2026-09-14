#pragma once

#include <gview/gview.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace gauche_menu {

class ViewBuilder {
  public:
    ViewBuilder(std::string id, int width, int height);

    float scale() const;
    bool compact() const;
    bool phone() const;
    glayout::ContainerKind split() const;
    gview::View finish();
    glayout::GraphNode& layout(std::string_view id);
    gview::NodeSpec& spec(std::string_view id);

    void subnavigation(std::string_view parent, std::string_view base,
                       std::string_view action_prefix, std::string_view selected_label,
                       std::vector<std::string> labels);
    void workspace(std::string_view parent, std::string id);
    void workspace_pane(std::string_view parent, std::string id,
                        glayout::Length width = {glayout::LengthKind::Fill, 1.0F});

    void container(std::string_view parent, std::string id, glayout::ContainerKind kind,
                   glayout::Length width, glayout::Length height, float gap = 0.0f,
                   glayout::Insets padding = {});
    void layout_container(std::string_view parent, std::string id, glayout::ContainerKind kind,
                          glayout::Length width, glayout::Length height, float gap = 0.0f,
                          glayout::Insets padding = {});
    void label(std::string_view parent, std::string id, std::string text, float height,
               float size = 16.0f, gview::TextAlign align = gview::TextAlign::Start);
    void button(std::string_view parent, std::string id, std::string text, std::string action,
                std::string group, float height = 48.0f,
                gview::ActivationPolicy activation = gview::ActivationPolicy::Manual);
    void toggle(std::string_view parent, std::string id, std::string text, std::string binding,
                std::string group, float height = 56.0f);
    void slider(std::string_view parent, std::string id, std::string text, std::string binding,
                std::string group, double minimum, double maximum, double step,
                float height = 62.0f);
    void select(std::string_view parent, std::string id, std::string text, std::string binding,
                std::vector<std::string> options, std::string group, float height = 56.0f);
    void text_input(std::string_view parent, std::string id, std::string label, std::string binding,
                    std::string group, float height = 48.0f, std::string placeholder = {});
    void image(std::string_view parent, std::string id, std::string asset, float height);
    void surface(std::string_view parent, std::string id, std::string asset);
    void surface(std::string_view parent, std::string id, std::string asset, float height);
    void focus_group(std::string id, std::string entry, std::string owner = {});
    void focus_scope(std::string_view root, std::string group);
    void edge(std::string from, gview::NavAction action, std::string to);
    void group_edge(std::string from, gview::NavAction action, std::string to);
    void scrolling(std::string_view id);

  private:
    gview::NodeSpec base_spec(std::string id) const;
    glayout::GraphNode container_node(std::string id, glayout::ContainerKind kind,
                                      glayout::Length width, glayout::Length height, float gap,
                                      glayout::Insets padding) const;
    void append(std::string_view parent, glayout::GraphNode node, gview::NodeSpec spec);
    void append_layout(std::string_view parent, glayout::GraphNode node);

    gview::View view_;
    float scale_ = 1.0f;
    int width_ = 1280;
    int height_ = 720;
};

} // namespace gauche_menu
