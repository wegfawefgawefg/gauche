#include "text_edit.hpp"

// TEXT FOCUS: Commit before pointer focus moves away. GView normally commits only
// on Confirm, leaving an unfocused text editor open after an outside click.
void finish_text_edit(FrontPage& page, const MenuInputState& input,
                      const glayout::ResolveInput& resolution, gview::Host& host) {
    const auto focus = page.runtime.focus();
    if (focus == gview::invalid_node || !page.runtime.state()[focus].editing) return;
    const auto& node = page.runtime.view().nodes[focus];
    if (node.source.control != gview::ControlKind::TextInput) return;
    const auto& rect = page.runtime.geometry()[node.layout_index].border;
    const auto& pointer = page.input.pointer;
    const bool outside = pointer.pressed && (pointer.x < rect.x || pointer.y < rect.y ||
        pointer.x >= rect.x + rect.w || pointer.y >= rect.y + rect.h);
    const bool name = node.source.binding == "profile-name";
    if (!outside && !input.up && !input.down && !input.back) return;
    gview::InputFrame finish;
    finish.text = std::move(page.input.text);
    finish.navigation.push_back(input.back && !name ?
        gview::NavAction::Back : gview::NavAction::Confirm);
    page.runtime.frame(resolution, finish, host);
    // What if Back ended name editing? Stay on the page; a second Back leaves it.
}
