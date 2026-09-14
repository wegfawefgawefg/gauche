#pragma once

#include "../src/world/route.hpp"
#include "../src/ui/text.hpp"

#include <algorithm>
#include <cstdio>

inline void render_floor_overview(SDL_Renderer* renderer, std::uint64_t seed) {
    Game game;
    game.rng = seed == 0 ? 1 : seed;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    FloorPlan plan = plan_floor(game);
    carve_floor(game, plan);
    game.run.spawn = plan.rooms[0].center;
    game.run.exit = plan.rooms[static_cast<std::size_t>(plan.exit_room)].center;
    const Handle player = spawn_entity(game, EntityKind::Player, game.run.spawn);
    game.players[0] = player;
    populate_rooms(game, plan);
    scatter_room_props(game, plan);
    const float scale = std::min(580.0F / static_cast<float>(plan.width),
                                 310.0F / static_cast<float>(plan.height));
    const float left = (640 - static_cast<float>(plan.width) * scale) * .5F;
    const float top = 25;
    for (int y = 0; y < plan.height; ++y)
        for (int x = 0; x < plan.width; ++x) {
            const Tile& tile = *game.stage.at({x, y});
            SDL_Color color{23, 28, 26, 255};
            if (walkable(tile.kind)) color = {94, 107, 89, 255};
            if (tile.kind == TileKind::Grass) color = {55, 90, 51, 255};
            if (tile.kind == TileKind::Water) color = {41, 69, 111, 255};
            if (tile.kind == TileKind::Ruin) color = {102, 93, 73, 255};
            if (tile.prop.kind != PropKind::None) color = {133, 112, 65, 255};
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            const SDL_FRect rect{left + static_cast<float>(x) * scale,
                                 top + static_cast<float>(y) * scale, scale, scale};
            SDL_RenderFillRect(renderer, &rect);
        }
    for (const Entity& entity : game.entities) {
        SDL_Color color{225, 91, 85, 255};
        if (entity.kind == EntityKind::None) continue;
        if (entity.kind == EntityKind::GroundItem) color = {210, 205, 164, 255};
        if (entity.kind == EntityKind::Player) color = {130, 215, 255, 255};
        if (entity.kind == EntityKind::Key) color = {255, 225, 80, 255};
        if (entity.kind == EntityKind::Exit) color = {125, 255, 140, 255};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        const SDL_FRect rect{left + static_cast<float>(entity.cell.x) * scale,
                             top + static_cast<float>(entity.cell.y) * scale, scale, scale};
        SDL_RenderFillRect(renderer, &rect);
    }
    for (const RoomPlan& room : plan.rooms)
        small_ui_text(renderer, left + static_cast<float>(room.center.x - room.half_width) * scale,
                      top + static_cast<float>(room.center.y - room.half_height - 2) * scale,
                      room_name(room.role));
    char header[160];
    std::snprintf(header, sizeof(header), "SEED %llu   %zu ROOMS   %zu LINKS   %dx%d   ROUTE %s",
        static_cast<unsigned long long>(seed), plan.rooms.size(), plan.edges.size(),
        plan.width, plan.height, floor_reachable(game) ? "VALID" : "BLOCKED");
    small_ui_text(renderer, 15, 8, header);
    small_ui_text(renderer, 15, 346, "BLUE SPAWN    GOLD KEY    GREEN EXIT    RED ACTORS / DOOR");
    std::puts(header);
}
