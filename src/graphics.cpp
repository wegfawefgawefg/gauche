#include "graphics.hpp"
#include "sound.hpp"
#include "ambience/catalog.hpp"

#include <SDL3_image/SDL_image.h>

#include <array>
#include <string_view>

namespace {

constexpr std::array<std::string_view, static_cast<std::size_t>(Sprite::Count)> sprite_names{
    "reticle", "cursor", "selected_arrow",
    "player", "player_dead", "player_footprint",
    "grass", "wall", "ruin", "water1", "water2", "water3", "water4",
    "chick", "hen", "rooster",
    "zombie", "zombie_angry", "zombie_scratch1", "zombie_dead", "zombie_gib1", "zombie_footprint",
    "blood_small", "blood_medium", "cloud1", "cloud2", "cloud3",
    "fist", "medkit", "bandage", "bandaid", "conductor_hat",
    "train_head", "train_car_a", "train_car_b", "caboose", "rail", "rail_crossing",
    "train_blinkensign", "train_car_block_pole",
    "buckler", "pistol", "musket", "bow", "rocket_launcher", "ammo", "bomb",
    "key", "door", "exit", "spawner",
    "forest_floor_a", "forest_floor_b", "forest_floor_c", "forest_grass",
    "forest_ruin", "forest_wall", "lava_tile", "ice_tile",
    "bat", "wolf", "bear", "bunny", "ember", "frost_bat", "sleep_meds",
    "stick", "shotgun", "smg", "bear_trap", "mine", "switch",
    "pickaxe", "raw_meat", "cooked_meat", "campfire", "den", "crusher", "dog",
    "bear_trap_open", "campfire_ash", "flame_a", "flame_b",
    "leaves", "twigs", "fern", "tall_grass", "puffball", "rotten_log", "crate", "nest", "clay_pot",
    "debris_oak_leaf", "debris_birch_leaf", "debris_pine_needle", "debris_twig", "debris_bark",
    "debris_wood_chip", "debris_root", "debris_fern_leaf", "debris_grass_blade", "debris_mushroom_cap",
    "debris_mushroom_stem", "debris_spore", "debris_acorn", "debris_seed_husk", "debris_feather",
    "debris_bone_chip", "debris_pottery", "debris_cloth", "debris_brass_case", "debris_stone_chip", "encounter_gate", "grave_vent", "coins",
    "boar", "thorn_snail", "thorn_snail_closed", "lantern_moth", "spore_toad", "spore_toad_swollen", "crate_mimic",
    "throwing_rock", "hatchet", "hunting_spear", "crossbow", "blunderbuss", "wooden_maul", "rake", "flint_knife",
    "status_sleep", "status_stun", "status_chill",
    "shallow_water_a", "shallow_water_b", "spring_a", "spring_b", "arrow", "bomb_lit", "bow_drawn", "canopy_oak", "canopy_pine",
    "torch", "lighter", "oil_flask", "sap_jar", "water_flask", "mushroom_spores", "smoke_pot", "honey_pot",
    "rocket", "bolt", "egg", "fried_egg", "root_turret", "root_turret_coiled", "bramble_guard", "bramble_guard_swing",
    "mosquito", "mosquito_fed", "owl", "owl_flying", "woodpecker", "woodpecker_drilling",
    "wasp_nest", "wasp_nest_stirring", "wasp_nest_empty", "wasp", "wasp_sting",
    "forager_goblin", "forager_goblin_knife", "carrion_crow", "carrion_crow_snatch",
    "burrow_worm_head", "burrow_worm_body", "burrow_worm_bite",
    "forest_tree", "forest_timber", "tree_stump", "timber_broken",
    "digging_claws", "resin_glue", "seed_bag", "lantern_seed", "shoot", "shoot_tall", "root_cover", "lantern_plant",
    "herb_bag", "splint", "bitter_root", "chili", "fungal_bread",
    "bird_seed", "thorn_caltrops", "bird_seed_pile", "thorn_patch",
    "hunting_horn", "rope_hook", "hook_head",
    "root_drill", "drill_root", "swap_seed", "boomerang", "rope_snare", "snare_set", "snare_tight", "spring_trap", "spring_ready", "acorn_mine", "acorn_ready", "throwing_net", "net_flight", "net_caught", "sticky_boots", "rabbit_charm", "hand_bell", "firecracker", "firecracker_lit", "stink_bomb", "rotten_fruit", "pitch_bomb", "pitch_bomb_lit", "status_nausea", "shield_lantern", "reflecting_pan", "pan_ready", "scarecrow_bundle", "scarecrow", "debris_straw", "straw_decoy_bundle", "straw_decoy",
};

std::filesystem::path named_asset(const std::filesystem::path& root,
                                  std::string_view folder, std::string_view name,
                                  std::string_view extension) {
    return root / folder / (std::string{name} + std::string{extension});
}

bool require_file(const std::filesystem::path& path, std::string& error) {
    if (std::filesystem::is_regular_file(path)) {
        return true;
    }
    error = "Missing asset: " + path.string();
    return false;
}

} // namespace

void unload_graphics(GameGraphics& graphics) {
    SDL_DestroyTexture(graphics.overhead_canvas);
    graphics.overhead_canvas = nullptr;
    SDL_DestroyTexture(graphics.interaction_canvas);
    graphics.interaction_canvas = nullptr;
    for (SDL_Texture*& texture : graphics.textures) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

GameGraphics::~GameGraphics() { unload_graphics(*this); }

std::filesystem::path asset_root() {
    const std::filesystem::path beside_executable =
        std::filesystem::path{SDL_GetBasePath()} / "assets";
    if (std::filesystem::is_directory(beside_executable)) {
        return beside_executable;
    }
    return std::filesystem::path{GAUCHE_SOURCE_DIR} / "assets";
}

bool validate_assets(const std::filesystem::path& root, std::string& error) {
    for (std::string_view name : sprite_names) {
        if (!require_file(named_asset(root, "graphics", name, ".png"), error)) {
            return false;
        }
    }
    for (std::string_view name : sound_names) {
        if (!require_file(named_asset(root, "sounds", name, ".ogg"), error)) {
            return false;
        }
    }
    for (const AmbientSpec& spec : ambient_specs) {
        if (!require_file(named_asset(root, "ambience", spec.name, ".ogg"), error)) return false;
    }
    for (std::string_view name : {"title", "playing"}) {
        if (!require_file(named_asset(root, "music", name, ".ogg"), error)) {
            return false;
        }
    }
    return true;
}

bool load_graphics(GameGraphics& graphics, SDL_Renderer* renderer,
                   const std::filesystem::path& root, std::string& error) {
    for (std::size_t index = 0; index < sprite_names.size(); ++index) {
        const auto path = named_asset(root, "graphics", sprite_names[index], ".png");
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.string().c_str());
        if (texture == nullptr) {
            error = "Unable to load " + path.string() + ": " + SDL_GetError();
            return false;
        }
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        graphics.textures[index] = texture;
    }
    return true;
}

SDL_Texture* texture_for(const GameGraphics& graphics, Sprite sprite) {
    return graphics.textures[static_cast<std::size_t>(sprite)];
}
