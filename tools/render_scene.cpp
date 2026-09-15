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
#include "ice_scene.hpp"
#include "rime_scene.hpp"
#include "frost_scene.hpp"
#include "diver_scene.hpp"
#include "air_scene.hpp"
#include "cold_scene.hpp"
#include "heat_scene.hpp"
#include "summer_scene.hpp"
#include "siphon_scene.hpp"
#include "thaw_scene.hpp"
#include "leech_scene.hpp"
#include "mason_scene.hpp"
#include "eel_scene.hpp"
#include "snow_scene.hpp"
#include "optics_scene.hpp"
#include "knight_scene.hpp"
#include "prism_scene.hpp"
#include "warden_scene.hpp"
#include "felt_scene.hpp"
#include "echo_scene.hpp"
#include "muffling_scene.hpp"
#include "alarm_scene.hpp"
#include "pilgrim_scene.hpp"
#include "fishing_scene.hpp"
#include "widow_scene.hpp"
#include "seal_scene.hpp"
#include "whiteout_scene.hpp"
#include "brine_scene.hpp"
#include "ram_scene.hpp"
#include "effigy_scene.hpp"
#include "candle_scene.hpp"
#include "keeper_scene.hpp"
#include "shard_scene.hpp"
#include "coal_scene.hpp"
#include "kettle_scene.hpp"
#include "spider_scene.hpp"
#include "boiler_scene.hpp"
#include "ice_ambience_scene.hpp"
#include "ice_equipment_scene.hpp"
#include "flare_scene.hpp"
#include "circuit_scene.hpp"
#include "lantern_scene.hpp"
#include "echo_pebble_scene.hpp"
#include "harpoon_scene.hpp"
#include "sluice_scene.hpp"
#include "../src/debug/playtest.hpp"
#include "../src/ui/stage_announcement.hpp"
#include "quarry_tools_scene.hpp"
#include "projectile_scene.hpp"
#include "material_scene.hpp"
#include "motion_scene.hpp"
#include "camera_path.hpp"
#include "light_gradient_scene.hpp"
#include "remedy_scene.hpp"
#include "cold_remedy_scene.hpp"
#include "ground_tools_scene.hpp"
#include "displacement_scene.hpp"
#include "root_relics_scene.hpp"
#include "thunder_scene.hpp"
#include "pocket_door_scene.hpp"
#include "recoverable_scene.hpp"
#include "woodland_trap_scene.hpp"
#include "movement_scene.hpp"
#include "noisemakers_scene.hpp"
#include "mixtures_scene.hpp"
#include "shield_scene.hpp"
#include "parry_scene.hpp"
#include "decoy_scene.hpp"
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
    if (mode == "debug" || mode == "debug-levels" || mode == "debug-loadout") {
        debug_window = SDL_CreateWindow("Static debug capture", 1920, 1080, SDL_WINDOW_HIDDEN);
        init_debug_panels(debug_window, renderer);
        debug_panels().visible = true;
        debug_panels().combat = mode == "debug";
        debug_panels().status = mode == "debug";
        playtest_tools().levels = mode == "debug-levels";
        playtest_tools().loadouts = mode == "debug-loadout";
        set_loadout_preset(playtest_tools().loadout, 3);
        playtest_tools().preset = 3;
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
    if (mode.starts_with("thaw") || mode.starts_with("siphon") || mode.starts_with("summer") || mode.starts_with("sluice-") || mode.starts_with("harpoon-") || mode == "echo-pebbles" || mode == "echo-items" || mode.starts_with("lantern-") || mode == "circuits" || mode == "circuit-items" || mode == "flares" || mode == "flare-items" || mode == "footing" || mode == "footing-slide" || mode == "footing-world" || mode == "ice-ambience" || mode == "boilers" || mode == "boiler-plugged" || mode == "pressure-items" || mode == "spiders" || mode == "spider-caught" || mode == "kettle" || mode == "kettle-items" || mode == "stoves" || mode == "coal-items" || mode == "shards" || mode == "shard-cut" || mode == "shard-pulse" || mode == "keepers" || mode == "candles" || mode == "candle-items" || mode == "effigies" || mode == "rams" || mode == "brine-items" || mode == "brine" || mode == "drummers" || mode == "whiteout" || mode == "kelp-items" || mode == "globe-items" || mode == "seals" || mode == "widows" || mode == "fish-bait" || mode == "fish-items" || mode == "fishing-cast" || mode == "fishing-reel" || mode == "fishing-items" || mode == "pilgrims" || mode == "alarms" || mode == "alarm-items" || mode == "muffling" || mode == "muffling-hud" || mode == "echo-hounds" || mode == "felt" || mode == "felt-items" || mode == "wardens" || mode == "warden-beam" || mode == "prisms" || mode == "prism-burst" || mode == "prism-items" || mode == "knights" || mode == "knight-beam" || mode == "optics" || mode == "optic-items" || mode == "mirror-item" || mode == "snow-burrowers" || mode == "snow-tools" || mode == "snowball-items" || mode == "eels" || mode == "eel-arcs" || mode == "eel-items" || mode == "quarry-tools" || mode == "quarry-bricks" || mode == "brick-place" || mode == "brick-throw" || mode == "masons" || mode == "ice-terrain" || mode == "ice-floor" || mode == "skaters" || mode == "grit-items" || mode == "frost-bats" || mode == "ice-needles" || mode == "divers" || mode == "air-float" || mode == "air-items" || mode == "cold-pool" || mode == "cold-items" || mode == "heat-patch" || mode == "heat-items" || mode == "leeches" || mode == "leech-release" || mode == "leech-poses")
        arrange_ice_scene(game, cosmetics, mode == "ice-floor",
            argc >= 4 ? std::strtoull(argv[3], nullptr, 10) : 1);
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
    if (mode == "skaters" || mode == "grit-items") arrange_rime_scene(game, cosmetics, player);
    if (mode == "frost-bats" || mode == "ice-needles") arrange_frost_scene(game, cosmetics, player);
    if (mode == "divers") arrange_diver_scene(game, cosmetics, player);
    if (mode == "leeches" || mode == "leech-release" || mode == "leech-poses") {
        arrange_leech_scene(game, cosmetics, player, mode == "leech-release");
        if (mode == "leech-poses") {
            player.cell = {21, 14};
            player.light = {12, 1700, {255, 239, 214}};
        }
    }
    if (mode == "eels" || mode == "eel-arcs" || mode == "eel-items")
        arrange_eel_scene(game, cosmetics, player, mode == "eel-arcs");
    if (mode == "boilers" || mode == "boiler-plugged" || mode == "pressure-items" || mode == "spiders" || mode == "spider-caught" || mode == "kettle" || mode == "kettle-items" || mode == "stoves" || mode == "coal-items" || mode == "shards" || mode == "shard-cut" || mode == "shard-pulse" || mode == "keepers" || mode == "candles" || mode == "candle-items" || mode == "effigies" || mode == "rams" || mode == "brine-items" || mode == "brine" || mode == "drummers" || mode == "whiteout" || mode == "kelp-items" || mode == "globe-items") arrange_whiteout_scene(game, cosmetics, player, mode == "whiteout");
    if (mode == "shards" || mode == "shard-cut" || mode == "shard-pulse")
        arrange_shard_scene(game,cosmetics,player,mode == "shard-cut",mode == "shard-pulse");
    if (mode == "footing" || mode == "footing-slide" || mode == "footing-world")
        arrange_ice_equipment_scene(game,cosmetics,player,mode != "footing");
    if (mode.starts_with("sluice-")) arrange_sluice_scene(game,cosmetics,player);
    if (mode.starts_with("harpoon-")) arrange_harpoon_scene(game,cosmetics,player);
    if (mode == "echo-pebbles" || mode == "echo-items") arrange_echo_pebble_scene(game,cosmetics,player);
    if (mode == "circuits" || mode == "circuit-items") arrange_circuit_scene(game,cosmetics,player);
    if (mode.starts_with("thaw")) arrange_thaw_scene(game,cosmetics,player,mode=="thaw-burst");
    if (mode.starts_with("siphon")) arrange_siphon_scene(game,cosmetics,player,mode=="siphon-fire");
    if (mode.starts_with("summer")) arrange_summer_scene(game,cosmetics,player);
    if (mode.starts_with("lantern-")) arrange_lantern_scene(game,cosmetics,player,mode == "lantern-focus",mode == "lantern-shut");
    if (mode == "flares" || mode == "flare-items") arrange_flare_scene(game,cosmetics,player);
    if (mode == "ice-ambience") arrange_ice_ambience_scene(game,cosmetics,player);
    if (mode == "ice-floor") describe_ice_ambience(game);
    if (mode == "boilers" || mode == "boiler-plugged" || mode == "pressure-items") arrange_boiler_scene(game,cosmetics,player,mode == "boiler-plugged");
    if (mode == "spiders" || mode == "spider-caught") arrange_spider_scene(game,cosmetics,player,mode == "spider-caught");
    if (mode == "kettle" || mode == "kettle-items") arrange_kettle_scene(game,cosmetics,player);
    if (mode == "stoves" || mode == "coal-items") arrange_coal_scene(game,cosmetics,player);
    if (mode == "keepers") arrange_keeper_scene(game,cosmetics,player);
    if (mode == "candles" || mode == "candle-items") arrange_candle_scene(game, cosmetics, player);
    if (mode == "effigies") arrange_effigy_scene(game, cosmetics, player);
    if (mode == "rams") arrange_ram_scene(game, cosmetics, player);
    if (mode == "brine-items" || mode == "brine") arrange_brine_scene(game, cosmetics, player);
    if (mode == "kelp-items") {
        player.inventory = {};
        insert_item(player.inventory, make_item(ItemKind::SmokedFish, 2));
        insert_item(player.inventory, make_item(ItemKind::SaltedKelp, 5));
        player.health = 21; player.vitals.nausea = 180;
    }
    if (mode == "seals") arrange_seal_scene(game, cosmetics, player);
    if (mode == "widows" || mode == "fish-bait" || mode == "fish-items") arrange_widow_scene(game, cosmetics, player, mode != "widows");
    if (mode == "fishing-cast" || mode == "fishing-reel" || mode == "fishing-items") arrange_fishing_scene(game, cosmetics, player, mode != "fishing-cast");
    if (mode == "pilgrims") arrange_pilgrim_scene(game, cosmetics, player);
    if (mode == "alarms" || mode == "alarm-items") arrange_alarm_scene(game, cosmetics, player);
    if (mode == "muffling" || mode == "muffling-hud") arrange_muffling_scene(game, cosmetics, player);
    if (mode == "echo-hounds") arrange_echo_scene(game, cosmetics, player);
    if (mode == "felt" || mode == "felt-items")
        arrange_felt_scene(game, cosmetics, player, mode == "felt");
    if (mode == "wardens" || mode == "warden-beam")
        arrange_warden_scene(game, cosmetics, player, mode == "warden-beam");
    if (mode == "prisms" || mode == "prism-burst" || mode == "prism-items")
        arrange_prism_scene(game, cosmetics, player, mode == "prism-burst");
    if (mode == "knights" || mode == "knight-beam")
        arrange_knight_scene(game, cosmetics, player, mode == "knight-beam");
    if (mode == "optics" || mode == "optic-items" || mode == "mirror-item")
        arrange_optics_scene(game, cosmetics, player, mode == "optics");
    if (mode == "snow-burrowers" || mode == "snow-tools" || mode == "snowball-items")
        arrange_snow_scene(game, cosmetics, player, mode == "snowball-items");
    if (mode == "masons") arrange_mason_scene(game, cosmetics, player);
    if (mode == "quarry-tools" || mode == "quarry-bricks" || mode == "brick-place" || mode == "brick-throw")
        arrange_quarry_tools(game, cosmetics, player, mode == "quarry-bricks" || mode == "brick-throw",
            mode == "brick-place" || mode == "brick-throw");
    if (mode == "heat-patch" || mode == "heat-items") arrange_heat_scene(game, cosmetics, player);
    if (mode == "cold-pool" || mode == "cold-items") arrange_cold_scene(game, cosmetics, player);
    if (mode == "air-float" || mode == "air-items") arrange_air_scene(game, cosmetics, player);
    if (mode == "decoy-items" || mode == "decoys")
        arrange_decoys(game, cosmetics, player, mode == "decoys", true);
    if (mode == "ward-items" || mode == "wards")
        arrange_decoys(game, cosmetics, player, mode == "wards");
    if (mode == "parry-items" || mode == "parries")
        arrange_parries(game, cosmetics, player, mode == "parries");
    if (mode == "shield-items" || mode == "shields")
        arrange_shields(game, cosmetics, player, mode == "shields");
    if (mode == "mixtures" || mode == "mixture-items" || mode == "mixture-big" || mode == "fruit-item")
        arrange_mixtures(game, cosmetics, player, mode == "mixtures", mode == "mixture-big");
    if (mode == "whistle" || mode == "whistle-items")
        arrange_noisemakers(game, cosmetics, player, false, true);
    if (mode == "noisemakers" || mode == "noise-items" || mode == "noise-big")
        arrange_noisemakers(game, cosmetics, player, mode == "noise-big");
    if (mode == "movement-tools" || mode == "movement-items" || mode == "movement-big" || mode == "rabbit-item")
        arrange_movement_tools(game, cosmetics, player, mode == "movement-tools", mode == "movement-big");
    if (mode == "woodland-traps" || mode == "trap-items" || mode == "trap-big")
        arrange_woodland_traps(game, cosmetics, player, mode == "trap-big");
    if (mode == "recoverables" || mode == "recoverable-items")
        arrange_recoverables(game, cosmetics, player);
    if (mode == "pockets" || mode == "pocket-items")
        arrange_pocket_doors(game, cosmetics, player, mode == "pockets");
    if (mode == "thunder" || mode == "thunder-items")
        arrange_thunder(game, cosmetics, player, mode == "thunder");
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
    if (mode == "cold-remedies" || mode == "cold-poultice" || mode == "cold-recovery" || mode == "poultice-recovery")
        arrange_cold_remedies(game, player, mode == "cold-recovery" || mode == "poultice-recovery",
            mode == "cold-poultice" || mode == "poultice-recovery");
    if (mode == "artifacts") {
        player.artifacts = (1U << 1) | (1U << 2) | (1U << 3) | (1U << 4);
        player.move_interval = 6;
    }
    if (mode == "thaw-items" || mode == "siphon-items" || mode == "summer-items" || mode == "sluice-items" || mode == "harpoon-items" || mode == "echo-items" || mode == "lantern-items" || mode == "circuit-items" || mode == "flare-items" || mode == "footing" || mode == "footing-slide" || mode == "pressure-items" || mode == "kettle-items" || mode == "coal-items" || mode == "candle-items" || mode == "brine-items" || mode == "kelp-items" || mode == "globe-items" || mode == "fish-items" || mode == "fishing-items" || mode == "alarm-items" || mode == "muffling" || mode == "felt-items" || mode == "prism-items" || mode == "optic-items" || mode == "mirror-item" || mode == "snow-tools" || mode == "snowball-items" || mode == "eel-items" || mode == "quarry-tools" || mode == "quarry-bricks" || mode == "cold-remedies" || mode == "cold-poultice" || mode == "heat-items" || mode == "cold-items" || mode == "air-items" || mode == "ice-needles" || mode == "grit-items" || mode == "pocket-items" || mode == "thunder-items" || mode == "whistle-items" || mode == "decoy-items" || mode == "ward-items" || mode == "parry-items" || mode == "shield-items" || mode == "mixture-items" || mode == "mixture-big" || mode == "fruit-item" || mode == "noise-items" || mode == "noise-big" || mode == "movement-items" || mode == "movement-big" || mode == "rabbit-item" || mode == "trap-items" || mode == "trap-big" || mode == "recoverable-items" || mode == "root-relic-items" || mode == "displacement-items" || mode == "ground-tool-items" || mode == "remedies" || mode == "remedy-root" || mode == "wood-tool-items" || mode == "lit-stick-inventory" || mode == "food" || mode == "inventory" || mode == "artifacts" || mode == "items" || mode == "stacks" || mode == "bow" || mode == "material-items") {
        interaction.inventory_open = true;
        interaction.slide = 1;
        interaction.slot_focus = mode == "sluice-items" ? 1 : mode == "harpoon-items" ? 1 : mode == "echo-items" ? 1 : mode == "lantern-items" ? 1 : mode == "circuit-items" ? 1 : mode == "flare-items" ? 1 : mode == "footing" ? 2 : mode == "footing-slide" ? 1 : mode == "pressure-items" ? 1 : mode == "kettle-items" ? 1 : mode == "coal-items" ? 1 : mode == "candle-items" ? 1 : mode == "brine-items" || mode == "kelp-items" || mode == "globe-items" ? 1 : mode == "fish-items" ? 1 : mode == "fishing-items" ? 1 : mode == "alarm-items" ? 0 : mode == "muffling" ? 0 : mode == "felt-items" ? 0 : mode == "prism-items" ? 1 : mode == "mirror-item" ? 1 : mode == "snow-tools" ? 1 : mode == "snowball-items" ? 2 : mode == "quarry-tools" ? 0 : mode == "quarry-bricks" ? 2 : mode == "cold-remedies" ? 0 : mode == "cold-poultice" ? 2 : mode == "fruit-item" ? 2 : mode == "mixture-items" || mode == "mixture-big" ? 1 : mode == "noise-items" || mode == "noise-big" ? 1 : mode == "rabbit-item" ? 3 : mode == "movement-items" || mode == "movement-big" ? 1 : mode == "trap-items" || mode == "trap-big" ? 3 : mode == "recoverable-items" ? 1 : mode == "remedies" ? 1 : mode == "remedy-root" ? 3 : mode == "lit-stick-inventory" ? 0 : mode == "food" || mode == "stacks" || mode == "bow" ? 1 : 2;
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
        render_game(renderer, graphics, game, 0, 2.0F, &cosmetics, {}, (mode.starts_with("siphon") || mode == "summer" || mode == "muffling-hud" || mode == "cold-recovery" || mode == "poultice-recovery" || mode == "skaters" || mode == "pockets" || mode == "thunder" || mode == "whistle" || mode == "decoys" || mode == "wards" || mode == "parries" || mode == "shields" || mode == "mixtures" || mode == "noisemakers" || mode == "movement-tools" || mode == "woodland-traps" || mode == "recoverables" || mode == "root-relics" || mode == "displacement" || mode == "ground-tools" || mode == "remedy-status" || mode == "wood-tools" || mode == "lit-stick" || mode == "hud" || mode == "fps" || mode == "status" || mode == "debug"), true);
        draw_interaction(renderer, graphics, game, 0, interaction);
    }
    if (mode == "stage-banner") {
        StageAnnouncement banner;
        update_stage_announcement(banner, game, 0, true, 0);
        banner.age = 1;
        draw_stage_announcement(renderer, banner);
    }
    if (mode == "fps") draw_frame_rate(renderer, 60);
    if (mode == "debug" || mode == "debug-levels" || mode == "debug-loadout") {
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
