#pragma once
#include "game.hpp"
#include "view.hpp"

enum class ScenePass { Ground, Bodies };
enum class BodyKind { Entity, Prop, RoofRow };
struct BodyDraw {
    std::size_t slot=0; // Entity slot or roof index; unused for props.
    Cell cell{};       // Ground anchor, independent of sprite height/fall pose.
    BodyKind kind=BodyKind::Entity;
    int priority=0;
};
std::vector<BodyDraw> body_draw_order(const Game& game,ViewCamera camera,float zoom,ScenePass pass);
