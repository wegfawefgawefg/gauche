#pragma once

#include "graphics.hpp"

#include <array>
#include <cstdint>
#include <vector>

struct Cell {
    int x = 0;
    int y = 0;
    friend bool operator==(Cell, Cell) = default;
};

constexpr Cell operator+(Cell a, Cell b) { return {a.x + b.x, a.y + b.y}; }
constexpr Cell operator-(Cell a, Cell b) { return {a.x - b.x, a.y - b.y}; }
int distance(Cell a, Cell b);

enum class TileKind : std::uint8_t { Empty, Grass, Wall, Ruin, Water, Rail };
struct Tile {
    TileKind kind = TileKind::Empty;
    std::uint8_t hp = 0;
    std::uint8_t water_phase = 0;
};

struct Stage {
    int width = 0;
    int height = 0;
    std::vector<Tile> tiles;
    bool in_bounds(Cell cell) const;
    Tile* at(Cell cell);
    const Tile* at(Cell cell) const;
};

bool walkable(TileKind kind);
bool buildable(TileKind kind);
bool damage_tile(Stage& stage, Cell cell, int damage);

enum class ItemKind : std::uint8_t {
    None, Wall, Medkit, Bandage, Bandaid, Fist, ConductorHat,
    Buckler, Pistol, Musket, Bow, RocketLauncher, Ammo, Bomb,
};

struct Item {
    ItemKind kind = ItemKind::None;
    int count = 0;
    int cooldown = 0;
    int loaded = 0;
    int spare = 0;
    int durability = 0;
};

constexpr int quick_slots = 6;
struct Inventory {
    std::array<Item, quick_slots> slots{};
    int selected = 0;
    Item* held();
    const Item* held() const;
};

Item make_item(ItemKind kind, int count = 1);
Sprite item_sprite(ItemKind kind);
const char* item_name(ItemKind kind);
bool insert_item(Inventory& inventory, Item item);

enum class EntityKind : std::uint8_t { None, Player, Zombie, Chicken, RailLayer, Train, GroundItem };
struct Entity {
    EntityKind kind = EntityKind::None;
    std::uint32_t generation = 0;
    Cell cell{};
    Cell facing{0, 1};
    Sprite sprite = Sprite::Player;
    int owner = -1;
    int health = 0;
    int max_health = 0;
    int move_wait = 0;
    int move_interval = 0;
    int attack_wait = 0;
    int attack_interval = 0;
    int use_flash = 0;
    int block_ticks = 0;
    int script_tick = 0;
    int train_cars_left = 0;
    Cell train_origin{};
    std::uint64_t birth_tick = 0;
    bool impassable = false;
    bool hard_blocker = false;
    Inventory inventory{};
    Item ground_item{};
};

struct Handle {
    int slot = -1;
    std::uint32_t generation = 0;
    friend bool operator==(Handle, Handle) = default;
};

constexpr int max_entities = 512;
struct Input {
    Cell move{};
    Cell aim{};
    bool use = false;
    bool pickup = false;
    bool drop = false;
    bool reload = false;
    int select = -1;
};

struct Game {
    Stage stage{};
    std::array<Entity, max_entities> entities{};
    std::array<Handle, 4> players{};
    std::uint64_t rng = 1;
    std::uint64_t tick = 0;
    bool started = false;
    bool game_over = false;
};

std::uint32_t random_u32(Game& game);
Handle spawn_entity(Game& game, EntityKind kind, Cell cell);
Entity* get_entity(Game& game, Handle handle);
const Entity* get_entity(const Game& game, Handle handle);
void remove_entity(Game& game, Handle handle);
int entity_at(const Game& game, Cell cell, bool impassable_only = false);
bool move_entity(Game& game, int slot, Cell destination);
void damage_entity(Game& game, int slot, int damage, Cell attacker);
bool use_held_item(Game& game, int user_slot, Cell target);
bool reload_held_item(Game& game, int user_slot);
void start_test_arena(Game& game, std::uint64_t seed);
void step_game(Game& game, const std::array<Input, 4>& inputs);
std::uint64_t game_hash(const Game& game);
