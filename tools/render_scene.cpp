#include "../src/game.hpp"
#include "../src/render.hpp"
#include "../src/input.hpp"
#include "../src/particles/templates.hpp"
#include "../src/props/interaction.hpp"
#include "../src/ui/interaction.hpp"

#include <SDL3_image/SDL_image.h>

#include <cstdio>
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
        spawn_footprint(cosmetics, {8 + i, 11}, EntityKind::Player, i % 2 == 0,
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
    if (argc < 2 || argc > 3) {
        std::fprintf(stderr, "Usage: gauche_render_scene output.png [hud|inventory|reward]\n");
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
    const std::string_view mode = argc == 3 ? argv[2] : "terrain";
    InteractionUi interaction;
    Entity& player = *get_entity(game, game.players[0]);
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::Buckler));
    insert_item(player.inventory, make_item(ItemKind::Pickaxe, 1, ItemAttribute::Big));
    player.inventory.slots[1].durability = 17;
    if (mode == "inventory") {
        interaction.inventory_open = true;
        interaction.slide = 1;
        interaction.slot_focus = 2;
    } else if (mode == "reward") {
        game.run.phase = RunPhase::Reward;
        game.run.offers[0] = {Reward{RewardKind::Item, ItemKind::Pickaxe, ArtifactKind::None, 1},
            Reward{RewardKind::Health, ItemKind::None, ArtifactKind::None, 15},
            Reward{RewardKind::Item, ItemKind::Bow, ArtifactKind::None, 1}};
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    render_game(renderer, graphics, game, 0, 2.0F, &cosmetics, {}, mode == "hud", true);
    draw_interaction(renderer, graphics, game, 0, interaction);
    SDL_RenderPresent(renderer);
    const bool saved = IMG_SavePNG(surface, argv[1]);
    unload_graphics(graphics);
    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(surface);
    SDL_Quit();
    return saved ? 0 : 1;
}
