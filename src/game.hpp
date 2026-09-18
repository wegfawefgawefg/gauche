#pragma once

#include <map>
#include <ranges>
#include <cstdint>
using PlayerId = std::int32_t;

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
#include <memory>

struct GenerationReport;

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
    bool electric = false, beam = false, magnetic = false, flame = false;
};

constexpr Cell operator+(Cell a, Cell b) { return {a.x + b.x, a.y + b.y}; }
constexpr Cell operator-(Cell a, Cell b) { return {a.x - b.x, a.y - b.y}; }
int distance(Cell a, Cell b);
Cell cardinal_toward(Cell from, Cell to, Cell fallback);

enum class TileKind : std::uint8_t { Empty, Grass, Wall, Ruin, Water, Rail, Lava, Ice, ShallowWater, Spring, Snow, IceHole, Bridge, Chasm, DeepRiver, Count };

// Enterable without a floor: contact resolves flight, support or a fatal fall.
inline bool open_drop(TileKind kind) { return kind==TileKind::Chasm || kind==TileKind::DeepRiver; }
enum class TileMaterial : std::uint8_t { Stone, Timber, Tree, Ice, Root, Count };
enum class BreakRule : std::uint8_t { Unbreakable, Damageable, DigRequired };
enum class TileImpact : std::uint8_t { Strike, Blast, Train };
enum class ItemKind : std::uint8_t;
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
    ItemKind contents{};
    std::uint8_t content_count = 0;
};

enum class RoofKind : std::uint8_t { Log, FrozenLog, Gantry, Container, IceArch, HollowTree, Count };
// Ground and its contents remain independent. One connected roof fades together.
struct RoofSpan {
    Cell start{};
    RoofKind kind = RoofKind::Log;
    std::uint8_t length = 5, vertical = 0, hp = 40;
    std::uint8_t height = 0; // Raised crown: Ice arches 2–4; hollow trees 4–6.
    std::uint8_t width = 3; // Hollow-tree footprint; narrow spans retain three rows.
};

struct Stage {
    int width = 0;
    int height = 0;
    std::vector<Tile> tiles;
    // Sparse attribution for player-placed props; never pack a player ID into artwork bits.
    std::map<int, PlayerId> prop_owners;
    std::vector<RoofSpan> roofs;
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

enum class ArtifactKind : std::uint8_t { None, AllPiercing, Reflector, Hearth, FleetFeet,
    StrongArms, QuickHands, Vitality, Dodge, Regeneration, CriticalChance, CriticalPower,
    Armor, Technical, Medic, Reusable, Golddigger, Oversized, Sweeping, Iron, Chef, GodHand, Count };
using ArtifactStacks = std::array<std::uint32_t, static_cast<std::size_t>(ArtifactKind::Count)>;
inline constexpr std::uint32_t max_power_stacks = 1000000;

enum class ItemKind : std::uint8_t {
    None, Wall, Medkit, Bandage, Bandaid, Fist, ConductorHat,
    Buckler, Pistol, Musket, Bow, RocketLauncher, Ammo, Bomb, SleepMeds,
    Stick, Shotgun, SMG, BearTrap, Mine, Pickaxe, RawMeat, CookedMeat,
    ThrowingRock, Hatchet, HuntingSpear, Crossbow, Blunderbuss, WoodenMaul, Rake, FlintKnife,
    Torch, Lighter, OilFlask, SapJar, WaterFlask, MushroomSpores, SmokePot, HoneyPot,
    Egg, FriedEgg, DiggingClaws, ResinGlue, SeedBag, LanternSeed,
    HerbBag, Splint, BitterRoot, Chili, FungalBread,
    BirdSeed, ThornCaltrops, HuntingHorn, RopeHook, RootDrill, BlinkSeed, Boomerang, RopeSnare, SpringTrap, AcornMine, ThrowingNet, StickyBoots, RabbitCharm, HandBell, Firecracker, StinkBomb, RottenFruit, PitchBomb, ShieldLantern, ReflectingPan, Scarecrow, StrawDecoy, WolfWhistle, ThunderAcorn, PocketDoor, GritPouch, IceNeedle, AirBladder, ColdFlask, HeatCapsule, WoolWrap, HotBroth, IcePoultice, Chisel, IceBrick, EelBattery, SnowScoop, Snowball, LensCarbine, MirrorShard, CrystalLens, PrismBomb, BlackFelt, MufflingFelt, AlarmClock, FishingLine, SmokedFish, SnowGlobe, SaltedKelp, BrineFlask, CandleStub, WickSpool, CoalLump, SteamKettle, PressureValve, Sealant, SkateBlade, Crampons, SignalFlare, CopperWire, GroundingSpike, StormLantern, EchoPebble, HarpoonGun, EmergencyDoorstop, BorrowedSummer, HeatSiphon, ThawCharge, FoldedBridge, TuningFork, StillwaterBell, EffigyMask, IceAnchor, SnowShelter, Sled,
    ForemanWhistle, QuarryCharge, FuseScissors, PressHammer, RubberMallet, RivetGun, BeltCrank, BrakeShoe, ArcTorch, HorseshoeMagnet, FoldingBarricade, CoolantCan, PocketDrill, TensionSpring, EmergencyFoam, BoltPouch, ChainHook, NailBoard, HandBellows, PocketPump, NozzleElbow, MoldKey, SteamLance, TarFlask, RailSwitchKey, InsulatedBoots, GlowSlag, SteelToeCap, LunchTin,
    IceAxe, TuskPike, RiverFish,
    Slap, ParryPan, Jump, Grapple, Shove, Kick, Elbow, GodFist, Balloon, CrushShield,
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
    std::uint32_t technical_level = 0; // Highest pickup improvement already applied.
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
    WaspNest, Wasp, ForagerGoblin, CarrionCrow, BurrowWorm, PocketDoor, RimeSkater, BellDiver, SteamLeech, IceMason, GlassEel, SnowBurrower, MirrorKnight, LensWarden, EchoHound, FrozenPilgrim, FishingWidow, SealThief, WhiteoutDrummer, AvalancheRam, SnowEffigy, CandleKeeper, ShardColony, IcicleSpider, BoilerPorter, BoilerTank, IceAnchor, Sled, Pickhand, ShiftForeman, PowderMonkey, Strikebreaker, RivetGunner, Yeti, ArcWelder, MagnetCrane, PressureRat, CableCrawler, WalkingKiln, AuditClerk, FurnaceMoth, SlagSnail, AshSleeper, Counterweight, EmergencyPump, MoldThief, CastingMold, TarChoir, RailCart, RailShunter, FrostGoblin, PipeGuard, CoalCutter, CraneOperator, ForestSpider, Snake, Ant, AntNest, AntSugar, AntLoad, Gnome, GnomeHouse, OldGrowthBear, RiverRaft,
    Count,
};

// Forced airborne movement owns no species behavior slots. Positions stay on the grid.
struct ActorToss {
    Cell origin{}, direction{}, source{};
    Handle instigator{};
    int ticks = 0;
};

struct BasicState {
    int jump_ticks=0;
    Cell jump_origin{}, jump_destination{};
    Handle grabbed{}, carried_by{};
    Prop held_prop{};
    Cell prop_cell{}, prop_direction{};
    int prop_ticks=0;
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
    PlayerId owner = -1;
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
    ActorToss toss{};
    int script_tick = 0;
    std::uint32_t artifacts = 0;
    ArtifactStacks powers{};
    int action_fraction = 0, action_steps = 1, move_fraction = 0, regen_progress = 0;
    ItemKind basic_action = ItemKind::Fist;
    BasicState basic{};
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
    int replace_slot = -1;
    ItemKind replace_kind = ItemKind::None;
    std::uint64_t offer_token = 0;
    friend bool operator==(const Input&, const Input&) = default;
};

enum class RunPhase : std::uint8_t { Arena, Playing, Reward, Shop, Won };
using PlayerInputs = std::map<PlayerId, Input>;
inline const Input& input_for(const PlayerInputs& inputs, PlayerId id) {
    static const Input idle{};
    const auto found = inputs.find(id);
    return found == inputs.end() ? idle : found->second;
}

enum class DeathPolicy : std::uint8_t { NoRespawn, Entrance, NextFloor };
enum class ObjectiveKind : std::uint8_t { Key, Switch };
enum class RewardKind : std::uint8_t { Item, Artifact, Health, Speed };
constexpr std::uint32_t artifact_count(const Entity& entity, ArtifactKind kind) {
    const auto count = entity.powers[static_cast<std::size_t>(kind)];
    return count ? count : (entity.artifacts & (1U << static_cast<unsigned int>(kind))) ? 1U : 0U;
}
constexpr bool has_artifact(const Entity& entity, ArtifactKind kind) { return artifact_count(entity, kind) != 0; }
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
enum class FloorLayout { Automatic, Generated, HauntedHouse, FreightExchange, LastShift };
struct PlayerState {
    Handle controlled{};
    int coins = 0;
    bool chosen = false, shop_ready = false, online = false;
    std::array<Reward, 3> offers{};
    int pending_count = 0;
    std::array<std::array<Reward, 3>, 12> pending_offers{};
};
struct Run {
    RunPhase phase = RunPhase::Arena;
    int floor = 0;
    FloorLayout layout = FloorLayout::Generated;
    bool has_key = false;
    ObjectiveKind objective = ObjectiveKind::Key;
    std::array<ItemKind, 3> shop_stock{};
    std::uint64_t seed = 1;
    DeathPolicy death_policy = DeathPolicy::NextFloor;
    Cell spawn{};
    Cell exit{};
    std::array<StageLight, 16> roof_lights{};
    int roof_light_count = 0;
};

struct ReactorEvent {std::uint32_t tile=0;std::uint16_t due=0,warned_at=0;};
struct FallEvent {Handle actor{};Cell cell{};Sprite sprite=Sprite::Player;};

// Installed boiler plumbing: fixed mount, adjacent cargo intake, three-cell
// straight spring feed. Handles prevent a destroyed tank slot binding a new actor.
struct BoilerFeed {
    Handle tank{};
    Cell mount{},source{},delivery{};
    std::uint16_t water=0,dry_ticks=0;
    Cell drive{};
    Handle cutter{};
    std::vector<Cell> belts{}; // Ordered loading -> drive; excludes the dry cargo intake.

};

// A finite manual seam and its loading crew share a real boiler/belt installation.
struct IndustrialShift {
    Handle tank{},foreman{},hauler{};
    Cell origin{},direction{};
};

enum class LavaPhase : std::uint8_t { Cooling, Swelling, Airborne };
// A small floor-local hazard list; target commits before the visible warning.
// No retargeting during windup/flight, and no cosmetic random state is consulted.
struct LavaVent {
    Cell source{},target{};
    std::uint16_t ticks=360;
    LavaPhase phase=LavaPhase::Cooling;
};

enum class FissureKind : std::uint8_t { Steam,Lava };
enum class FissurePhase : std::uint8_t { Idle,Pressure,Release,Cooling };
// Three connected floor cracks share one pressure cycle and drainable heat store.
struct Fissure {
    Cell center{},axis{1,0};
    FissureKind kind=FissureKind::Steam;
    FissurePhase phase=FissurePhase::Idle;
    std::uint16_t ticks=240,heat=0;
};

struct Game {
    // Local generation evidence. Immutable, and intentionally absent from codecs/hashes.
    std::shared_ptr<const GenerationReport> generation_report;
    Stage stage{};
    std::array<Entity, max_entities> entities{};
    std::vector<FlightContact> flight_contacts;
    std::vector<ReactorEvent> reactor_front;
    std::vector<BoilerFeed> boiler_feeds;
    std::vector<IndustrialShift> industrial_shifts;
    std::vector<LavaVent> lava_vents;
    std::vector<Fissure> fissures;
    std::map<PlayerId, PlayerState> players{{0, {}}};
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
    std::array<FallEvent,32> falls{};
    int fall_count=0;
};

bool hit_terrain(Game& game, Cell cell, Cell source, int damage, int dig_power = 0,
                 TileImpact impact = TileImpact::Strike, bool sound = true);

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
struct GenerationTrace;
struct PopulationReport;
void generate_world_floor(Game& game, FloorLayout layout = FloorLayout::Automatic, PopulationReport* report=nullptr, GenerationTrace* trace=nullptr, std::uint64_t inhabitants_seed=0);
bool floor_reachable(const Game& game);
bool interact_with_fixture(Game& game, int owner, Cell target, bool held_use = false);
void finish_floor(Game& game);
void choose_reward(Game& game, int owner, int choice, int replace_slot = -1, ItemKind expected = ItemKind::None);
void choose_pending_reward(Game& game, int owner, int choice, int replace_slot = -1, ItemKind expected = ItemKind::None);
void buy_shop_item(Game& game, int owner, int choice, int replace_slot = -1, ItemKind expected = ItemKind::None);
int shop_price(ItemKind kind);
void advance_run(Game& game);
void step_game(Game& game, const PlayerInputs& inputs);
void step_traps(Game& game);
std::uint64_t game_hash(const Game& game);

inline PlayerState& player_state(Game& game, PlayerId id) { return game.players[id]; }
inline const PlayerState& player_state(const Game& game, PlayerId id) {
    static const PlayerState absent{};
    const auto found = game.players.find(id);
    return found == game.players.end() ? absent : found->second;
}
inline bool has_player(const Game& game, PlayerId id) { return game.players.contains(id); }

inline auto controlled_entities(const Game& game) {
    return game.players | std::views::values | std::views::transform([](const PlayerState& player) { return player.controlled; });
}

Cell player_spawn_cell(const Game& game, Cell center);
bool bind_player_control(Game& game, PlayerId id, Handle controlled);
