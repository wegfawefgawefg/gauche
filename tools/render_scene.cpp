#include "../src/game.hpp"
#include "../src/items/fire.hpp"
#include "floor_overview.hpp"
#include "enemy_scene.hpp"
#include "plant_scene.hpp"
#include "flier_scene.hpp"
#include "scavenger_scene.hpp"
#include "woodland_scene.hpp"
#include "woodland_tools_scene.hpp"
#include "footprint_scene.hpp"
#include "water_scene.hpp"
#include "projectile_scene.hpp"
#include "material_scene.hpp"
#include "motion_scene.hpp"
#include "camera_path.hpp"
#include "light_gradient_scene.hpp"
#include "remedy_scene.hpp"
#include "ground_tools_scene.hpp"
#include "displacement_scene.hpp"
#include "root_relics_scene.hpp"
#include "recoverable_scene.hpp"
#include "woodland_trap_scene.hpp"
#include "movement_scene.hpp"
#include "../src/scenery/overhead.hpp"
#include "../src/debug/panels.hpp"
#include "../src/world/encounter.hpp"
#include "../src/world/loot.hpp"
#include "../src/render.hpp"
#include "../src/input.hpp"
#include "../src/particles/templates.hpp"
#include "../src/props/interaction.hpp"
#include "../src/ui/interaction.hpp"
#include "../src/ui/frame_rate.hpp"

#include <SDL3_image/SDL_image.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

namespace {

void arrange_terrain(Game& game, Cosmetics& cosmetics) {
    game.started = true;
    game.tick = 120;
    game.run.phase = RunPhase::Playing;
    game.run.floor = 1;
    game.run.online[0] = true;
    game.stage.width = 28;
    game.stage.height = 18;
    game.stage.tiles.assign(28 * 18, {TileKind::Wall, 100, 0});
    for (int y = 3; y < 15; ++y)
        for (int x = 3; x < 25; ++x)
            *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {13, 10};
    cosmetics.camera = {13.5F, 9.0F};
    cosmetics.camera_ready = true;
    for (int x = 10; x < 16; ++x) {
        Tile& tile = *game.stage.at({x, 7});
        tile = {TileKind::Wall, static_cast<std::uint16_t>(100 - (x - 10) * 20), 0};
        if (tile.hp == 0) tile.kind = TileKind::Ruin;
    }
    spawn_entity(game, EntityKind::Campfire, {12, 10});
    Entity* ash = get_entity(game, spawn_entity(game, EntityKind::Campfire, {16, 10}));
    ash->fire_tramples = 5;
    ash->light = {};
    ash->self_light = {0, 0, 0};
    ash->sprite = Sprite::CampfireAsh;
    const Handle player = spawn_entity(game, EntityKind::Player, {12, 10});
    game.players[0] = player;
    get_entity(game, player)->scorch_ticks = 240;
    constexpr PropKind props[]{PropKind::Leaves, PropKind::Twigs, PropKind::Fern,
        PropKind::TallGrass, PropKind::Puffball, PropKind::RottenLog, PropKind::Crate,
        PropKind::Nest, PropKind::ClayPot};
    for (int i = 0; i < 9; ++i)
        place_prop(game.stage, {9 + i, 9}, props[i]);
    prepare_debris(cosmetics.debris, game.stage);
    for (int i = 0; i < 9; ++i)
        scatter_prop_debris(cosmetics.debris, {9 + i, 12}, props[i],
                           static_cast<std::uint64_t>(i + 1), true);
    for (int i = 0; i < 5; ++i)
        spawn_footprint(cosmetics, Cell{8 + i, 11}, EntityKind::Player, i % 2 == 0,
                        static_cast<std::uint64_t>(i + 10));
    for (int age = 0; age < 36; ++age) {
        step_particles(cosmetics);
        if (age % 6 == 0) {
            spawn_flame(cosmetics, {12, 10}, static_cast<std::uint64_t>(age + 1), false);
            spawn_campfire_smoke(cosmetics, {12, 10}, static_cast<std::uint64_t>(age + 1));
        }
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2 || argc > 4) {
        std::fprintf(stderr, "Usage: gauche_render_scene output.png [hud|inventory|reward|canopy|stack|layout|floor|mansion|mansion-map] [seed]\n");
        return 1;
    }
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
    SDL_Surface* surface = SDL_CreateSurface(1920, 1080, SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer* renderer = surface == nullptr ? nullptr : SDL_CreateSoftwareRenderer(surface);
    if (renderer == nullptr) return 1;
    SDL_SetRenderScale(renderer, 3.0F, 3.0F);
    GameGraphics graphics;
    std::string error;
    if (!load_graphics(graphics, renderer,
                       std::filesystem::path{GAUCHE_SOURCE_DIR} / "assets", error)) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    Game game;
    Cosmetics cosmetics;
    arrange_terrain(game, cosmetics);
    const std::string_view mode = argc >= 3 ? argv[2] : "terrain";
    SDL_Window* debug_window = nullptr;
    if (mode == "debug") {
        debug_window = SDL_CreateWindow("Static debug capture", 1920, 1080, SDL_WINDOW_HIDDEN);
        init_debug_panels(debug_window, renderer);
        debug_panels().visible = true;
        debug_panels().combat = true;
        debug_panels().status = true;
    }
    InteractionUi interaction;
    if (mode == "enemies") {
        arrange_enemy_scene(game, cosmetics);
        debug_panels().world_enemies = true;
    }
    if (mode == "footprints") arrange_footprint_scene(game, cosmetics);
    if (mode == "woodland") arrange_woodland_scene(game, cosmetics);
    if (mode == "scavengers") arrange_scavenger_scene(game, cosmetics);
    if (mode == "fliers") arrange_flier_scene(game, cosmetics);
    if (mode == "plants") arrange_plant_scene(game, cosmetics);
    if (mode == "water") arrange_water_scene(game, cosmetics);
    if (mode == "materials" || mode == "material-items") arrange_material_scene(game, cosmetics);
    if (mode == "motion") arrange_motion_scene(game, cosmetics);
    if (mode == "projectiles" || mode == "bow") arrange_projectile_scene(game, cosmetics);
    if (mode == "ballistics") arrange_ballistics_scene(game, cosmetics);
    if (mode == "canopy" || mode == "light-gradient") {
        game.run.roof_light_count = 1;
        game.run.roof_lights[0] = {{17, 6}};
    }
    if (mode == "stack") {
        Entity* stack = get_entity(game, spawn_entity(game, EntityKind::ZombieStack, {14, 10}));
        stack->counter_a = 4;
        Entity* falling = get_entity(game, spawn_entity(game, EntityKind::Zombie, {15, 12}));
        falling->point_a = {14, 12};
        falling->label_b = 1;
        falling->timer_b = 12;
    }
    if (mode == "floor") {
        start_run(game, argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 1);
        cosmetics = {};
    }
    if (mode == "mansion" || mode == "mansion-map") {
        game = {};
        cosmetics = {};
        game.rng = 1;
        game.run.floor = 3;
        game.run.phase = RunPhase::Playing;
        game.run.online[0] = true;
        make_haunted_floor(game, true);
        game.players[0] = spawn_entity(game, EntityKind::Player, {37, 36});
        populate_haunted_house(game);
        for (Entity& fixture : game.entities) {
            if (fixture.kind == EntityKind::Encounter) {
                fixture.label_a = static_cast<int>(EncounterPhase::Countdown);
                fixture.timer_a = 60;
            }
        }
    }
    Entity& player = *get_entity(game, game.players[0]);
    player.owner = 0;
    if (mode == "fire") { player.cell = {15, 10}; player.scorch_ticks = 0; }
    if (mode == "status" || mode == "debug") {
        player.scorch_ticks = 210;
        player.sleep_ticks = 75;
        player.stun_ticks = 30;
        player.freeze_ticks = 90;
    }
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::Buckler));
    insert_item(player.inventory, make_item(ItemKind::Pickaxe, 1, ItemAttribute::Big));
    player.inventory.slots[1].durability = 17;
    if (mode == "materials" || mode == "material-items") {
        player.inventory = {};
        for (ItemKind kind : {ItemKind::Torch, ItemKind::OilFlask, ItemKind::WaterFlask,
                             ItemKind::SapJar, ItemKind::SmokePot, ItemKind::MushroomSpores})
            insert_item(player.inventory, make_item(kind));
    }
    place_coins(game, player.cell + Cell{1, 1}, 12);
    game.run.coins[0] = 27;
    if (mode == "projectiles" || mode == "bow") {
        player.inventory = {};
        insert_item(player.inventory, make_item(ItemKind::Bow));
        insert_item(player.inventory, make_item(ItemKind::Bomb, 3));
        player.counter_a = 12; player.label_b = 1;
    }
    if (mode == "stacks") {
        player.inventory = {};
        insert_item(player.inventory, make_item(ItemKind::Fist));
        insert_item(player.inventory, make_item(ItemKind::Ammo, 3));
        insert_item(player.inventory, make_item(ItemKind::Pickaxe));
        insert_item(player.inventory, make_item(ItemKind::Ammo, 2));
    }
    if (mode == "items") {
        player.inventory = {};
        for (ItemKind kind : {ItemKind::HuntingSpear, ItemKind::Hatchet, ItemKind::Blunderbuss,
                              ItemKind::ThrowingRock, ItemKind::WoodenMaul, ItemKind::FlintKnife})
            insert_item(player.inventory, make_item(kind));
    }
    if (mode == "lit-stick" || mode == "lit-stick-inventory") {
        player.scorch_ticks = 0;
        player.cell = {13, 10};
        player.facing = {-1, 0};
        player.inventory = {};
        insert_item(player.inventory, make_item(ItemKind::Stick));
        light_stick(game, *player.inventory.held(), player.cell);
        Entity* dropped = get_entity(game, spawn_entity(game, EntityKind::GroundItem, {14, 10}));
        dropped->ground_item = *player.inventory.held();
        dropped->sprite = Sprite::Stick;
    }
    if (mode == "food") {
        player.inventory = {};
        for (ItemKind kind : {ItemKind::Fist, ItemKind::Egg, ItemKind::FriedEgg, ItemKind::Rake})
            insert_item(player.inventory, make_item(kind, kind == ItemKind::Egg ? 4 : 1));
        player.inventory.selected = 2;
    }
    if (mode == "movement-tools" || mode == "movement-items" || mode == "movement-big" || mode == "rabbit-item")
        arrange_movement_tools(game, cosmetics, player, mode == "movement-tools", mode == "movement-big");
    if (mode == "woodland-traps" || mode == "trap-items" || mode == "trap-big")
        arrange_woodland_traps(game, cosmetics, player, mode == "trap-big");
    if (mode == "recoverables" || mode == "recoverable-items")
        arrange_recoverables(game, cosmetics, player);
    if (mode == "root-relics" || mode == "root-relic-items")
        arrange_root_relics(game, cosmetics, player);
    if (mode == "displacement" || mode == "displacement-items")
        arrange_displacement(game, cosmetics, player);
    if (mode == "ground-tools" || mode == "ground-tool-items")
        arrange_ground_tools(game, cosmetics, player);
    if (mode == "wood-tools" || mode == "wood-tool-items")
        arrange_woodland_tools(game, cosmetics, player);
    if (mode == "remedies" || mode == "remedy-root" || mode == "remedy-status")
        arrange_remedies(player, mode == "remedy-status");
    if (mode == "artifacts") {
        player.artifacts = (1U << 1) | (1U << 2) | (1U << 3) | (1U << 4);
        player.move_interval = 6;
    }
    if (mode == "movement-items" || mode == "movement-big" || mode == "rabbit-item" || mode == "trap-items" || mode == "trap-big" || mode == "recoverable-items" || mode == "root-relic-items" || mode == "displacement-items" || mode == "ground-tool-items" || mode == "remedies" || mode == "remedy-root" || mode == "wood-tool-items" || mode == "lit-stick-inventory" || mode == "food" || mode == "inventory" || mode == "artifacts" || mode == "items" || mode == "stacks" || mode == "bow" || mode == "material-items") {
        interaction.inventory_open = true;
        interaction.slide = 1;
        interaction.slot_focus = mode == "rabbit-item" ? 3 : mode == "movement-items" || mode == "movement-big" ? 1 : mode == "trap-items" || mode == "trap-big" ? 3 : mode == "recoverable-items" ? 1 : mode == "remedies" ? 1 : mode == "remedy-root" ? 3 : mode == "lit-stick-inventory" ? 0 : mode == "food" || mode == "stacks" || mode == "bow" ? 1 : 2;
    } else if (mode == "reward" || mode == "reward-focus") {
        interaction.offer_focus = mode == "reward-focus" ? 1 : 0;
        game.run.phase = RunPhase::Reward;
        game.run.offers[0] = {Reward{RewardKind::Item, ItemKind::Pickaxe, ArtifactKind::None, 1},
            Reward{RewardKind::Health, ItemKind::None, ArtifactKind::None, 15},
            Reward{RewardKind::Item, ItemKind::Bow, ArtifactKind::None, 1}};
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (mode == "overhead") {
        SDL_SetRenderDrawColor(renderer, 80, 98, 77, 255);
        SDL_RenderClear(renderer);
        for (Tile& tile : game.stage.tiles) tile.kind = TileKind::Wall;
        draw_overhead(renderer, graphics, game, &cosmetics, cosmetics.camera, 2, {});
    } else if (mode == "light-gradient") render_light_gradient(renderer, game, cosmetics.camera);
    else if (mode == "camera-path") render_camera_path(renderer);
    else if (mode == "layout") render_floor_overview(renderer,
        argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 1);
    else if (mode == "mansion-map") render_floor_overview(renderer, 1, &game);
    else {
        render_game(renderer, graphics, game, 0, 2.0F, &cosmetics, {}, (mode == "movement-tools" || mode == "woodland-traps" || mode == "recoverables" || mode == "root-relics" || mode == "displacement" || mode == "ground-tools" || mode == "remedy-status" || mode == "wood-tools" || mode == "lit-stick" || mode == "hud" || mode == "fps" || mode == "status" || mode == "debug"), true);
        draw_interaction(renderer, graphics, game, 0, interaction);
    }
    if (mode == "fps") draw_frame_rate(renderer, 60);
    if (mode == "debug") {
        SDL_SetRenderScale(renderer, 1, 1);
        draw_debug_panels(game, 0);
        draw_debug_panels(game, 0);
        shutdown_debug_panels();
    }
    SDL_RenderPresent(renderer);
    const bool saved = IMG_SavePNG(surface, argv[1]);
    unload_graphics(graphics);
    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(surface);
    SDL_DestroyWindow(debug_window);
    SDL_Quit();
    return saved ? 0 : 1;
}
