#pragma once
#include "renderer/device.hpp"

#include "../src/particles/motion.hpp"
#include "../src/ui/text.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

inline void render_camera_path(tr::Renderer* renderer) {
    // RECORDED POSES: Examine only the camera filter. No game stepping or inputs.
    Entity player;
    player.kind = EntityKind::Player;
    player.move_interval = 7;
    EntityPose pose;
    step_camera_guide(pose, player, false);
    std::vector<ViewCamera> bodies{ViewCamera{player.cell}}, cameras{pose.camera_position};
    float min_cross = 100, max_cross = -100;
    for (int tick = 1; tick <= 330; ++tick) {
        if (tick <= 28 && tick % 7 == 0) ++player.cell.x;
        else if (tick > 28 && tick <= 224 && tick % 7 == 0) {
            if (((tick - 28) / 7) % 2 != 0) ++player.cell.y;
            else ++player.cell.x;
        }
        step_camera_guide(pose, player, true);
        pose.cell = player.cell;
        bodies.push_back(ViewCamera{player.cell});
        cameras.push_back(pose.camera_position);
        if (tick >= 100 && tick < 215) {
            const float cross = (pose.camera_position.x - pose.camera_position.y - 3.5F) / std::sqrt(2.0F);
            min_cross = std::min(min_cross, cross); max_cross = std::max(max_cross, cross);
        }
    }
    const auto screen = [](ViewCamera cell) -> SDL_FPoint {
        return {55 + cell.x * 22, 300 - cell.y * 16};
    };
    tr::set_color_bytes(renderer, 12, 17, 18, 255); tr::clear(renderer);
    const auto line = [&](const std::vector<ViewCamera>& positions, SDL_Color color) {
        tr::set_color_bytes(renderer, color.r, color.g, color.b, color.a);
        for (std::size_t i = 1; i < positions.size(); ++i) {
            const auto a = screen(positions[i-1]), b = screen(positions[i]);
            tr::line(renderer, a.x, a.y, b.x, b.y);
        }
    };
    line(bodies, {97, 108, 104, 255}); line(cameras, {222, 183, 98, 255});
    small_ui_text(renderer, 20, 12, "CAMERA PATH   RECORDED CARDINAL CELLS");
    small_ui_text(renderer, 20, 29, "GREY ACTOR TILES   GOLD CAMERA   STRAIGHT / DIAGONAL / STOP");
    char caption[180];
    const float settled = std::hypot(pose.camera_position.x-static_cast<float>(player.cell.x),
        pose.camera_position.y-static_cast<float>(player.cell.y));
    std::snprintf(caption, sizeof(caption), "CROSS-PATH SWAY %.3f TILES   SETTLED ERROR %.5f",
        max_cross-min_cross, settled);
    small_ui_text(renderer, 20, 334, caption);
    std::puts(caption);
}
