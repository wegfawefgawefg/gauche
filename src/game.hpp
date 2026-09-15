#pragma once

#include "biome.hpp"
#include "graphics.hpp"
#include "sound.hpp"
#include "props/catalog.hpp"
#include "surfaces/types.hpp"
#include "lighting/emitter.hpp"
#include "status/effects.hpp"

#include <array>
#include <cstdint>
#include <vector>

struct Cell {
    int x = 0;
    int y = 0;
    friend bool operator==(Cell, Cell) = default;
};

struct SoundEvent {
    SoundId sound = SoundId::Confirm;
    Cell cell{};
    std::uint64_t tick = 0;
    std::uint8_t sequence = 0;
    bool positional = true;
    bool muffled = false;
};

struct ImpactEvent {
    Cell cell{}, source{};
    Sprite material = Sprite::Wall;
    int damage = 0;
    bool broken = false;
    PropKind prop = PropKind::None;
};

struct SweepEvent {
    Cell source{}, direction{};
    int reach = 1, half_width = 1;
    bool outward = false;
};

struct ShotEvent {
    Cell source{}, end{};
    bool impact = false, muzzle = false, casing = false;
    bool electric = false, beam = false;
};

constexpr Cell operator+(Cell a, Cell b) { return {a.x + b.x, a.y + b.y}; }
constexpr Cell operator-(Cell a, Cell b) { return {a.x - b.x, a.y - b.y}; }
int distance(Cell a, Cell b);
Cell cardinal_toward(Cell from, Cell to, Cell fallback);

enum class TileKind : std::uint8_t { Empty, Grass, Wall, Ruin, Water, Rail, Lava, Ice, ShallowWater, Spring, Snow, IceHole, Bridge, Count };
enum class TileMaterial : std::uint8_t { Stone, Timber, Tree, Ice, Count };
enum class BreakRule : std::uint8_t { Unbreakable, Damageable, DigRequired };
enum class TileImpact : std::uint8_t { Strike, Blast, Train };
struct Tile {
    TileKind kind = TileKind::Empty;
    std::uint16_t hp = 0;
    std::uint8_t water_phase = 0;
    std::uint16_t max_hp = 100;
    BreakRule break_rule = BreakRule::DigRequired;
    std::uint8_t required_dig_power = 1;
    Prop prop{};
    Surface surface{};
    TileMaterial material = TileMaterial::Stone;
    TileKind thaw_kind = TileKind::Empty;
    std::uint16_t freeze_ticks = 0;
    std::uint8_t current = 0;
};

struct Stage {
    int width = 0;
    int height = 0;
    std::vector<Tile> tiles;
    bool in_bounds(Cell cell) const;
    Tile* at(Cell cell);
    const Tile* at(Cell cell) const;
    const Tile& at_or_border(Cell cell) const;
};

bool walkable(TileKind kind);
bool walkable(const Tile& tile);
bool buildable(TileKind kind);
bool damage_tile(Stage& stage, Cell cell, int damage, int dig_power = 0,
                 TileImpact impact = TileImpact::Strike);

enum class ItemKind : std::uint8_t {
    None, Wall, Medkit, Bandage, Bandaid, Fist, ConductorHat,
    Buckler, Pistol, Musket, Bow, RocketLauncher, Ammo, Bomb, SleepMeds,
    Stick, Shotgun, SMG, BearTrap, Mine, Pickaxe, RawMeat, CookedMeat,
    ThrowingRock, Hatchet, HuntingSpear, Crossbow, Blunderbuss, WoodenMaul, Rake, FlintKnife,
    Torch, Lighter, OilFlask, SapJar, WaterFlask, MushroomSpores, SmokePot, HoneyPot,
    Egg, FriedEgg, DiggingClaws, ResinGlue, SeedBag, LanternSeed,
    HerbBag, Splint, BitterRoot, Chili, FungalBread,
    BirdSeed, ThornCaltrops, HuntingHorn, RopeHook, RootDrill, SwapSeed, Boomerang, RopeSnare, SpringTrap, AcornMine, ThrowingNet, StickyBoots, RabbitCharm, HandBell, Firecracker, StinkBomb, RottenFruit, PitchBomb, ShieldLantern, ReflectingPan, Scarecrow, StrawDecoy, WolfWhistle, ThunderAcorn, PocketDoor, GritPouch, IceNeedle, AirBladder, ColdFlask, HeatCapsule, WoolWrap, HotBroth, IcePoultice, Chisel, IceBrick, EelBattery, SnowScoop, Snowball, LensCarbine, MirrorShard, CrystalLens, PrismBomb, BlackFelt, MufflingFelt, AlarmClock, FishingLine, SmokedFish, SnowGlobe, SaltedKelp, BrineFlask, CandleStub, WickSpool, CoalLump, SteamKettle, PressureValve, Sealant, SkateBlade, Crampons, SignalFlare, CopperWire, GroundingSpike, StormLantern, EchoPebble, HarpoonGun, EmergencyDoorstop, BorrowedSummer, HeatSiphon, ThawCharge, FoldedBridge, TuningFork, StillwaterBell, EffigyMask, IceAnchor, SnowShelter, Sled,
    ForemanWhistle, QuarryCharge, FuseScissors, PressHammer, RubberMallet,
    Count,
};

enum class ItemAttribute : std::uint8_t {
    None, Strong, Agile, Durable, Fragile, Heavy, Big,
    Long, Piercing, Restorative,
};

struct Handle {
    int slot = -1;
    std::uint32_t generation = 0;
    friend bool operator==(Handle, Handle) = default;
};

struct FlightContact { Handle projectile{}, victim{}; };

struct Item {
    ItemKind kind = ItemKind::None;
    ItemAttribute attribute = ItemAttribute::None;
    int count = 0;
    int max_count = 0;
    bool consume_on_use = false;
    int cooldown = 0;
    int loaded = 0;
    int spare = 0;
    int durability = 0;
    int max_durability = 0;
    int uses = 0;
    int max_uses = 0;
    bool opened = false;
    LightEmitter light{};
    int dig_power = 0;
    int flame_ticks = 0;
    std::uint8_t muffled_uses = 0;
    Handle flight{};
    Handle anchor{}; // A placed fixture owned by this item, independent of flight reservations.
};

constexpr int quick_slots = 6;
struct Inventory {
    std::array<Item, quick_slots> slots{};
    int selected = 0;
    Item* held();
    const Item* held() const;
};

Item make_item(ItemKind kind, int count = 1,
               ItemAttribute attribute = ItemAttribute::None);
Sprite item_sprite(ItemKind kind);
Sprite item_sprite(const Item& item);
const char* item_name(ItemKind kind);
bool insert_item(Inventory& inventory, Item item);
bool item_stackable(const Item& item);
int transfer_item(Inventory& inventory, Item& item);

enum class EntityKind : std::uint8_t {
    None, Player, Zombie, Chicken, RailLayer, Train, GroundItem,
    Key, Door, Exit, Spawner, Bat, Wolf, Bear, Bunny, Ember, FrostBat, Trap,
    Switch, Campfire, Den, Crusher, Dog, ZombieStack, Encounter, EncounterGate, WaveVent, Coins,
    Boar, ThornSnail, LanternMoth, SporeToad, CrateMimic, Projectile, RootTurret, BrambleGuard, Mosquito, Owl, Woodpecker,
    WaspNest, Wasp, ForagerGoblin, CarrionCrow, BurrowWorm, PocketDoor, RimeSkater, BellDiver, SteamLeech, IceMason, GlassEel, SnowBurrower, MirrorKnight, LensWarden, EchoHound, FrozenPilgrim, FishingWidow, SealThief, WhiteoutDrummer, AvalancheRam, SnowEffigy, CandleKeeper, ShardColony, IcicleSpider, BoilerPorter, BoilerTank, IceAnchor, Sled, Pickhand, ShiftForeman, PowderMonkey, Strikebreaker,
    Count,
};

struct Entity {
    EntityKind kind = EntityKind::None;
    std::uint32_t generation = 0;
    Cell cell{};
    Cell facing{0, 1};
    // BEHAVIOR: Each kind names its slots beside its own init/step code.
    Handle entity_a{}, entity_b{};
    Handle encounter{}; // Room ownership must not overwrite an enemy's behavior references.
    // ATTENTION: c point/label/timer hold noise/scent; a/b remain species-owned.
    Cell point_a{}, point_b{}, point_c{};
    int counter_a = 0, counter_b = 0, counter_c = 0;
    int label_a = 0, label_b = 0, label_c = 0;
    int timer_a = 0, timer_b = 0, timer_c = 0; // Shared countdowns; decremented even during stun/sleep.
    Sprite sprite = Sprite::Player;
    LightEmitter light{};
    LightTint self_light{0, 0, 0};
    int owner = -1;
    int health = 0;
    int max_health = 0;
    int move_wait = 0;
    int move_interval = 0;
    int attack_wait = 0;
    int attack_interval = 0;
    int use_flash = 0;
    int block_ticks = 0;
    int guard_slot = -1; // Guard belongs to the slot that was raised.
    int burn_ticks = 0;
    int scorch_ticks = 0;
    int fire_dim_ticks = 0;
    std::uint8_t fire_tramples = 0;
    int freeze_ticks = 0;
    int sleep_ticks = 0;
    int stun_ticks = 0;
    VitalEffects vitals{};
    int script_tick = 0;
    std::uint32_t artifacts = 0;
    int train_cars_left = 0;
    int spawn_wait = 0;
    Cell train_origin{};
    std::uint64_t birth_tick = 0;
    bool impassable = false;
    bool hard_blocker = false;
    bool fixture_open = false;
    Inventory inventory{};
    Item ground_item{};
};

constexpr int max_entities = 512;
struct Input {
    Cell move{};
    Cell aim{};
    bool use = false;
    bool cancel_use = false;
    bool pickup = false;
    bool drop = false;
    bool reload = false;
    bool interact = false;
    bool confirm = false;
    int select = -1;
    friend bool operator==(const Input&, const Input&) = default;
};

enum class RunPhase : std::uint8_t { Arena, Playing, Reward, Shop, Won };
enum class DeathPolicy : std::uint8_t { NoRespawn, Entrance, NextFloor };
enum class ObjectiveKind : std::uint8_t { Key, Switch };
enum class RewardKind : std::uint8_t { Item, Artifact, Health, Speed };
enum class ArtifactKind : std::uint8_t { None, AllPiercing, Reflector, Hearth, FleetFeet };
constexpr bool has_artifact(const Entity& entity, ArtifactKind kind) {
    return (entity.artifacts & (1U << static_cast<unsigned int>(kind))) != 0;
}
struct Reward {
    RewardKind kind = RewardKind::Item;
    ItemKind item = ItemKind::None;
    ArtifactKind artifact = ArtifactKind::None;
    int amount = 0;
    ItemAttribute attribute = ItemAttribute::None;
};
Item reward_item(Reward reward);
struct StageLight {
    Cell cell{};
    LightEmitter light{7, 1350, {240, 224, 176}};
};
struct Run {
    RunPhase phase = RunPhase::Arena;
    int floor = 0;
    std::array<int, 4> coins{};
    bool has_key = false;
    ObjectiveKind objective = ObjectiveKind::Key;
    std::array<bool, 4> chosen{};
    std::array<bool, 4> shop_ready{};
    std::array<bool, 4> online{};
    std::array<std::array<Reward, 3>, 4> offers{};
    std::array<int, 4> pending_count{};
    std::array<std::array<std::array<Reward, 3>, 12>, 4> pending_offers{};
    std::array<ItemKind, 3> shop_stock{};
    std::uint64_t seed = 1;
    DeathPolicy death_policy = DeathPolicy::NextFloor;
    Cell spawn{};
    Cell exit{};
    std::array<StageLight, 16> roof_lights{};
    int roof_light_count = 0;
};

struct Game {
    Stage stage{};
    std::array<Entity, max_entities> entities{};
    std::vector<FlightContact> flight_contacts;
    std::array<Handle, 4> players{};
    std::uint64_t rng = 1;
    std::uint64_t tick = 0;
    bool started = false;
    bool game_over = false;
    Run run{};
    std::array<SoundEvent, 128> sounds{};
    int sound_count = 0;
    // PRESENTATION: Impacts are regenerated on replay and never enter snapshots or hashes.
    std::array<ImpactEvent, 128> impacts{};
    int impact_count = 0;
    std::array<ShotEvent, 128> shots{};
    int shot_count = 0;
    std::array<SweepEvent, 32> sweeps{};
    int sweep_count = 0;
};

bool hit_terrain(Game& game, Cell cell, Cell source, int damage, int dig_power = 0,
                 TileImpact impact = TileImpact::Strike);

std::uint32_t random_u32(Game& game);
void emit_sound(Game& game, SoundId sound, Cell cell, bool positional = true, bool muffled = false);
Handle spawn_entity(Game& game, EntityKind kind, Cell cell);
Entity* get_entity(Game& game, Handle handle);
const Entity* get_entity(const Game& game, Handle handle);
void remove_entity(Game& game, Handle handle);
int entity_at(const Game& game, Cell cell, bool impassable_only = false);
bool move_entity(Game& game, int slot, Cell destination, bool allow_slip = true);
// Contact direction and a traveling shot's responsible actor are separate.
void damage_entity(Game& game, int slot, int damage, Cell attacker, bool blockable = true,
                   Handle instigator = {});
void enter_actor_cell(Game& game, int slot);
void crush_entity(Game& game, int slot, Cell attacker);
void blast_area(Game& game, Cell center, int radius, int damage, Cell attacker);
bool use_held_item(Game& game, int user_slot, Cell target);
bool reload_held_item(Game& game, int user_slot);
void start_test_arena(Game& game, std::uint64_t seed);
void start_run(Game& game, std::uint64_t seed);
enum class FloorLayout { Automatic, Generated, HauntedHouse };
void generate_world_floor(Game& game, FloorLayout layout = FloorLayout::Automatic);
bool floor_reachable(const Game& game);
bool interact_with_fixture(Game& game, int owner, Cell target, bool held_use = false);
void finish_floor(Game& game);
void choose_reward(Game& game, int owner, int choice);
void choose_pending_reward(Game& game, int owner, int choice);
void buy_shop_item(Game& game, int owner, int choice);
int shop_price(ItemKind kind);
void advance_run(Game& game);
void step_game(Game& game, const std::array<Input, 4>& inputs);
void step_traps(Game& game);
std::uint64_t game_hash(const Game& game);
