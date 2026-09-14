#pragma once
#include "../src/entities/bell_diver.hpp"

inline void arrange_diver_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {y >= 13 ? TileKind::Snow : TileKind::Ice, 0, 0};
    player.cell = {21, 12};
    player.inventory = {};
    player.scorch_ticks = 0;
    constexpr int phases[]{DiverSwim, DiverBell, DiverRise, DiverHunt, DiverSwing, DiverSink};
    constexpr Sprite sprites[]{Sprite::DiverBubbles, Sprite::DiverBell, Sprite::DiverRise,
        Sprite::BellDiver, Sprite::DiverSwing, Sprite::DiverSink};
    for (int index = 0; index < 6; ++index) {
        const Cell cell{17 + index % 3 * 3, index < 3 ? 10 : 14};
        if (index != 0 && index != 3 && index != 4) *game.stage.at(cell) = {TileKind::IceHole, 0, 0};
        Entity* diver = get_entity(game, spawn_entity(game, EntityKind::BellDiver, cell));
        diver->label_a = phases[index];
        diver->sprite = sprites[index];
        diver->timer_a = 18;
        diver->facing = {1, 0};
        diver->point_b = cell + diver->facing;
        diver->impassable = !diver_submerged(*diver);
    }
    cosmetics.camera = {20, 12};
    cosmetics.camera_ready = true;
}
