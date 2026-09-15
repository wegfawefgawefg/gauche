#include "encounter.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"

namespace {

void fill(Stage& stage, Cell low, Cell high, TileKind kind) {
    for (int y = low.y; y <= high.y; ++y)
        for (int x = low.x; x <= high.x; ++x)
            *stage.at({x, y}) = {kind, static_cast<std::uint16_t>(kind == TileKind::Wall ? 100 : 0), 0};
}

void shell(Stage& stage) {
    fill(stage, {18, 12}, {57, 52}, TileKind::Wall);
    fill(stage, {19, 13}, {56, 51}, TileKind::Ruin);
    // ROOMS: Galleries flank the entrance hall; upper rooms connect through three doorways.
    fill(stage, {28, 35}, {28, 51}, TileKind::Wall);
    fill(stage, {46, 35}, {46, 51}, TileKind::Wall);
    fill(stage, {19, 28}, {56, 28}, TileKind::Wall);
    for (int x : {26, 37, 49}) fill(stage, {x - 1, 28}, {x + 1, 28}, TileKind::Ruin);
    fill(stage, {28, 42}, {28, 44}, TileKind::Ruin);
    fill(stage, {46, 42}, {46, 44}, TileKind::Ruin);
    for (Cell pillar : {Cell{24, 19}, Cell{49, 19}, Cell{24, 35}, Cell{49, 35}})
        fill(stage, pillar, pillar + Cell{2, 2}, TileKind::Wall);
    fill(stage, {36, 52}, {38, 52}, TileKind::Ruin);
    fill(stage, {37, 12}, {37, 12}, TileKind::Ruin);
}

} // namespace

bool make_haunted_floor(Game& game, bool force) {
    if (!force && (game.run.floor != 3 || random_u32(game) % 3 != 0)) return false;
    game.stage.width = 76;
    game.stage.height = 68;
    game.stage.tiles.assign(76 * 68, {TileKind::Wall, 100, 0});
    fill(game.stage, {3, 3}, {72, 64}, TileKind::Grass);
    fill(game.stage, {35, 4}, {39, 63}, TileKind::Empty);
    fill(game.stage, {6, 9}, {12, 28}, TileKind::Water);
    fill(game.stage, {62, 36}, {68, 55}, TileKind::Water);
    shell(game.stage);
    game.run.spawn = {37, 60};
    game.run.exit = {37, 6};
    game.run.has_key = false;
    game.run.objective = ObjectiveKind::Switch;
    game.run.roof_light_count = 2;
    game.run.roof_lights = {};
    game.run.roof_lights[0] = {{12, 38}, {8, 1350, {240, 224, 176}}};
    game.run.roof_lights[1] = {{63, 21}, {8, 1350, {240, 224, 176}}};
    for (int y = 0; y < 68; ++y)
        for (int x = 0; x < 76; ++x)
            if (x == 0 || x == 75 || y == 0 || y == 67)
                *game.stage.at({x, y}) = {TileKind::Wall, 100, 0, 100, BreakRule::Unbreakable, 0};
    return true;
}

void populate_haunted_house(Game& game) {
    const Handle controller = spawn_entity(game, EntityKind::Encounter, {37, 33});
    Entity& encounter = *get_entity(game, controller);
    encounter.point_a = {19, 13};
    encounter.point_b = {56, 51};
    encounter.counter_a = 3;
    for (Cell cell : {Cell{36, 52}, Cell{37, 52}, Cell{38, 52}, Cell{37, 12}}) {
        Entity& gate = *get_entity(game, spawn_entity(game, EntityKind::EncounterGate, cell));
        gate.entity_a = controller;
    }
    for (Cell cell : {Cell{21, 15}, Cell{54, 15}, Cell{21, 49}, Cell{54, 49}}) {
        Entity& vent = *get_entity(game, spawn_entity(game, EntityKind::WaveVent, cell));
        vent.entity_a = controller;
    }
    Entity& exit = *get_entity(game, spawn_entity(game, EntityKind::Exit, game.run.exit));
    exit.entity_a = controller;
    for (Cell cell : {Cell{32, 58}, Cell{32, 34}, Cell{43, 20}})
        spawn_entity(game, EntityKind::Campfire, cell);
    for (Cell cell : {Cell{22, 42}, Cell{53, 42}, Cell{31, 21}, Cell{44, 21}})
        place_prop(game.stage, cell, PropKind::Crate);
    for (int i = 0; i < 14; ++i) {
        const Cell cell{8 + static_cast<int>(random_u32(game) % 60),
                        56 + static_cast<int>(random_u32(game) % 7)};
        if (distance(cell, game.run.spawn) > 5 && entity_at(game, cell, false) < 0)
            place_prop(game.stage, cell, i % 4 == 0 ? PropKind::RottenLog : PropKind::Leaves);
    }
    place_coins(game, {22, 17}, 14);
    place_coins(game, {52, 17}, 14);
    place_ground_item(game, {23, 45}, ItemKind::Blunderbuss);
    place_ground_item(game, {52, 45}, ItemKind::Buckler);
    place_ground_item(game, {35, 33}, ItemKind::Ammo);
    place_ground_item(game, {39, 33}, ItemKind::Bandage, 3);
    place_ground_item(game, {37, 61}, ItemKind::Stick);
    spawn_entity(game, EntityKind::Wolf, {15, 44});
    spawn_entity(game, EntityKind::Bunny, {64, 26});
}
