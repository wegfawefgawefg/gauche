#pragma once
#include "../src/entities/shard_colony.hpp"

// DISPLAY: Actual shared link cells, with one endpoint displaced in the cut view.
inline void arrange_shard_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool cut, bool pulse) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    const Handle handle = spawn_shard_colony(game,{{Cell{17,13},Cell{21,13},Cell{21,17}}});
    const ShardGroup group = shard_group(game,handle);
    for (int i=0;i<group.count;++i) {
        Entity& node = *get_entity(game,group.nodes[static_cast<std::size_t>(i)]);
        node.label_a = pulse ? ShardPulse : ShardCharge; node.timer_a = pulse ? 10 : 28;
        if (cut && i==1) node.cell = node.cell+Cell{1,0};
        step_shard_state(node);
    }
    player.cell = {18,17}; player.light = {10,1250,{210,226,224}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{15,16},DebrisKind::CrystalSplinter,4,5803,true);
}
