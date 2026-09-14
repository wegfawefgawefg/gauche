#include "../src/lighting/field.hpp"

#include <cmath>
#include <cstdio>

namespace {

bool check(bool value, const char* message) {
    if (!value) std::fprintf(stderr, "FAIL: %s\n", message);
    return value;
}

Game closed_stage() {
    Game game;
    game.stage.width = game.stage.height = 9;
    game.stage.tiles.assign(81, {TileKind::Wall, 100, 0});
    game.run.phase = RunPhase::Playing;
    game.run.exit = {999, 999};
    return game;
}

bool near(float left, float right) {
    return std::abs(left - right) < 0.00001F;
}

} // namespace

int main() {
    Game game = closed_stage();
    LightingCache cache;
    build_lighting(cache, game, {4, 4}, 2.0F);
    const float closed = cache.ambient[cache.index({3, 3})];
    const float unlit = light_at_cell(cache, {7, 7}).red;
    if (!check(unlit < 0.25F, "unlit tiles are too bright")) return 1;
    game.stage.at({2, 2})->kind = TileKind::Empty;
    build_lighting(cache, game, {4, 4}, 2.0F);
    if (!check(cache.ambient[cache.index({3, 3})] > closed,
               "diagonal openness did not lift ambient light") ||
        !check(cache.ambient[cache.index({7, 7})] == closed,
               "openness leaked beyond adjacent tiles")) return 1;

    // BORDER: Game collision remains bounded while its visible wall is lit.
    game.run.roof_lights[0].cell = {1, 1};
    game.run.roof_light_count = 1;
    build_lighting(cache, game, {0, 1}, 2.0F);
    const LightColor inside = light_at_cell(cache, {0, 1});
    const LightColor edge = light_at_cell(cache, {-1, 1});
    const LightColor far = light_at_cell(cache, {-4, 1});
    if (!check(game.stage.at({-1, 1}) == nullptr &&
               game.stage.at_or_border({-1, 1}).kind == TileKind::Wall,
               "exterior material changed gameplay bounds") ||
        !check(inside.red > 0.8F && edge.red > far.red + 0.2F &&
               inside.red > far.red &&
               edge.red > edge.blue,
               "colored light failed to cross or fade along the border")) return 1;
    const LightColor joint = light_at_corner(cache, {0, 1});
    const LightColor expected_a = light_at_cell(cache, {-1, 0});
    const LightColor expected_b = light_at_cell(cache, {0, 0});
    const LightColor expected_c = light_at_cell(cache, {-1, 1});
    const LightColor expected_d = light_at_cell(cache, {0, 1});
    if (!check(near(joint.red, (expected_a.red + expected_b.red +
                                expected_c.red + expected_d.red) * 0.25F),
               "shared tile corner did not use adjacent world samples")) return 1;
    game.run.roof_lights[0].light = {4, 1200, {20, 255, 20}};
    build_lighting(cache, game, {0, 1}, 2.0F);
    const LightColor fixture_green = light_at_cell(cache, {1, 1});
    if (!check(fixture_green.green > fixture_green.red + 0.5F,
               "roof fixture's own light color was ignored")) return 1;

    // EFFECTS: A local flash changes presentation but leaves stage state alone.
    game.run.roof_light_count = 0;
    const LightColor before = light_at_cell(cache, {4, 4});
    const LightFlash flash{{{4, 4}, 5, 1.2F, {1.0F, 0.2F, 0.1F}}, 8, 8};
    build_lighting(cache, game, {4, 4}, 2.0F, std::span{&flash, 1});
    const LightColor after = light_at_cell(cache, {4, 4});
    if (!check(after.red > before.red && after.red > after.blue &&
               game.stage.at({4, 4})->hp == 100,
               "transient light altered rules or failed to color the field")) return 1;

    // EMITTERS: The instance's light values work regardless of entity kind.
    Entity& lamp = game.entities[0];
    lamp.kind = EntityKind::Zombie;
    lamp.cell = {4, 4};
    lamp.health = lamp.max_health = 10;
    lamp.light = {4, 1500, {25, 90, 255}};
    build_lighting(cache, game, {4, 4}, 2.0F);
    const LightColor blue = light_at_cell(cache, lamp.cell);
    if (!check(blue.blue > 0.9F && blue.blue > blue.red + 0.5F,
               "entity instance light was ignored")) return 1;
    lamp.health = 0;
    build_lighting(cache, game, {4, 4}, 2.0F);
    if (!check(light_at_cell(cache, lamp.cell).blue < 0.25F,
               "dead actor kept casting its light")) return 1;
    Entity& ground = game.entities[1];
    ground.kind = EntityKind::GroundItem;
    ground.cell = {4, 4};
    ground.ground_item.light = {3, 1200, {25, 255, 25}};
    build_lighting(cache, game, {4, 4}, 2.0F);
    const LightColor green = light_at_cell(cache, ground.cell);
    if (!check(green.green > 0.9F && green.green > green.red + 0.5F,
               "ground item instance light was ignored")) return 1;
    std::puts("lighting rules passed");
    return 0;
}
