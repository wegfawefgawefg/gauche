#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>

enum class Sprite : std::size_t {
    Reticle, Cursor, SelectedArrow,
    Player, PlayerDead, PlayerFootprint,
    Grass, Wall, Ruin, Water1, Water2, Water3, Water4,
    Chick, Hen, Rooster,
    Zombie, ZombieAngry, ZombieScratch1, ZombieDead, ZombieGib1, ZombieFootprint,
    BloodSmall, BloodMedium, Cloud1, Cloud2, Cloud3,
    Fist, Medkit, Bandage, Bandaid, ConductorHat,
    TrainHead, TrainCarA, TrainCarB, Caboose, Rail, RailCrossing,
    TrainBlinkensign, TrainCarBlockPole,
    Buckler, Pistol, Musket, Bow, RocketLauncher, Ammo, Bomb,
    Key, Door, Exit, Spawner,
    ForestFloorA, ForestFloorB, ForestFloorC, ForestGrass, ForestRuin, ForestWall,
    LavaTile, IceTile, Bat, Wolf, Bear, Bunny, Ember, FrostBat, SleepMeds,
    Count,
};

struct GameGraphics {
    std::array<SDL_Texture*, static_cast<std::size_t>(Sprite::Count)> textures{};
    GameGraphics() = default;
    GameGraphics(const GameGraphics&) = delete;
    GameGraphics& operator=(const GameGraphics&) = delete;
    ~GameGraphics();
};

void unload_graphics(GameGraphics& graphics);
std::filesystem::path asset_root();
bool validate_assets(const std::filesystem::path& root, std::string& error);
bool load_graphics(GameGraphics& graphics, SDL_Renderer* renderer,
                   const std::filesystem::path& root, std::string& error);
SDL_Texture* texture_for(const GameGraphics& graphics, Sprite sprite);
