#pragma once
#include "../game.hpp"
#include <vector>

// SCENERY: Derived local decoration; no collision, loot, heat or saved state.
enum class IceSceneKind { Window, Chain, Hatch, Pipe, Wheel, Motor, Flag, Drip, Chimney };
struct IceScene {
    IceSceneKind kind{};
    Cell cell{}, support{};
    TileKind floor = TileKind::Empty;
    PropKind prop = PropKind::None;
};
struct IceScenery {
    std::uint64_t key = 0;
    std::vector<IceScene> sites;
};
std::vector<IceScene> make_ice_scenes(const Game& game);
bool ice_scene_alive(const IceScene& scene, const Game& game);
void prepare_ice_scenery(IceScenery& scenery, const Game& game);
