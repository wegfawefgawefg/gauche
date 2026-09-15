#include "ice.hpp"
#include "../world/ice_terrain.hpp"
#include "../world/water.hpp"

std::vector<IceScene> make_ice_scenes(const Game& game) {
    std::vector<IceScene> sites;
    if (!ice_floor(game.run.floor) || game.run.phase == RunPhase::Arena) return sites;
    const auto add = [&](IceSceneKind kind, Cell cell, Cell support, PropKind prop = PropKind::None) {
        int count = 0;
        for (const auto& site : sites) {
            if (site.cell == cell) return;
            if (site.kind != kind) continue;
            if (distance(site.cell, cell) < 16) return;
            ++count;
        }
        if (count < 4) sites.push_back({kind, cell, support, game.stage.at_or_border(cell).kind, prop});
    };
    // FIXTURES: Keep their sound and ornament anchored to the actual apparatus.
    for (int y = 1; y < game.stage.height - 1; ++y)
        for (int x = 1; x < game.stage.width - 1; ++x) {
            const Cell cell{x,y}; const auto& tile = game.stage.at_or_border(cell);
            if (tile.prop.broken) continue;
            if (tile.prop.kind == PropKind::BeamLamp)
                add(IceSceneKind::Motor,cell,cell,PropKind::BeamLamp);
            if (tile.prop.kind == PropKind::Stove)
                add(IceSceneKind::Chimney,cell,cell,PropKind::Stove);
        }
    // BANKS: A wall-fed pool, a mooring chain and a sealed hatch read as small scenes.
    for (int y = 1; y < game.stage.height - 1; ++y)
        for (int x = 1; x < game.stage.width - 1; ++x) {
            const Cell cell{x,y}, wall=cell+Cell{0,-1};
            const auto& tile = game.stage.at_or_border(cell);
            const unsigned bits = static_cast<unsigned>(x*73+y*137) ^ static_cast<unsigned>(game.run.seed);
            if (tile.prop.kind != PropKind::None) continue;
            const bool bank = game.stage.at_or_border(wall).kind == TileKind::Wall;
            if (bank && (tile.kind == TileKind::Water || shallow_water(tile.kind))) {
                add(bits%3==0 ? IceSceneKind::Wheel : IceSceneKind::Drip,cell,wall);
                continue;
            }
            if (tile.kind == TileKind::Water && bits%7==0) {
                add(IceSceneKind::Hatch,cell,cell); continue;
            }
            if (tile.kind == TileKind::Ruin && bank) {
                const bool shore = shallow_water(game.stage.at_or_border(cell+Cell{1,0}).kind) ||
                    shallow_water(game.stage.at_or_border(cell+Cell{-1,0}).kind);
                add(shore ? IceSceneKind::Chain : bits%2==0 ? IceSceneKind::Window : IceSceneKind::Pipe,cell,wall);
            }
            if (tile.kind == TileKind::Snow && bank && bits%5==0)
                add(IceSceneKind::Flag,cell,wall);
        }
    return sites;
}

bool ice_scene_alive(const IceScene& scene, const Game& game) {
    const auto* tile=game.stage.at(scene.cell); const auto* support=game.stage.at(scene.support);
    if (!tile || !support || tile->kind != scene.floor) return false;
    if (scene.prop != PropKind::None)
        return tile->prop.kind==scene.prop && !tile->prop.broken;
    if (scene.cell != scene.support && support->kind != TileKind::Wall) return false;
    return tile->prop.kind==PropKind::None;
}

void prepare_ice_scenery(IceScenery& scenery, const Game& game) {
    const auto key = game.run.seed ^ (static_cast<std::uint64_t>(game.run.floor)<<48) ^
        (static_cast<std::uint64_t>(game.stage.width)<<32) ^ static_cast<std::uint64_t>(game.stage.height) ^
        (static_cast<std::uint64_t>(game.run.phase)<<40);
    if (scenery.key==key) return;
    scenery.key=key; scenery.sites=make_ice_scenes(game);
}
