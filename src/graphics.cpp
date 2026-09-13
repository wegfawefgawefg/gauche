#include "graphics.hpp"

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
    "pickaxe", "raw_meat", "cooked_meat", "campfire", "den", "crusher",
};

constexpr std::array<std::string_view, 41> sound_names{
    "ape_scream", "ball_bounce1", "ball_bounce2", "ball_bounce3", "ball_bounce4",
    "ball_drop", "ball_hit_paddle", "ball_hit_paddle_er", "ball_wall_bounce",
    "confirm", "death", "explosion", "explosion1", "explosion2", "explosion3",
    "animal_crush1", "animal_crush2", "hit_block1", "level_loss", "level_start",
    "level_win", "small_laser", "sturdy_block_bounced_on", "super_confirm",
    "step1", "step2", "box_break", "block_land", "zombie_growl1", "zombie_growl2",
    "zombie_scratch1", "punch1", "cloth_rip", "cant_use", "chick", "hen", "rooster",
    "distant_train_sound", "rail_place", "train_passing", "drop",
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
