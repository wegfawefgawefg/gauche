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
    Stick, Shotgun, SMG, BearTrap, Mine, Switch, Pickaxe, RawMeat, CookedMeat,
    Campfire, Den, Crusher, Dog, BearTrapOpen,
    CampfireAsh, FlameA, FlameB,
    Leaves, Twigs, Fern, TallGrass, Puffball, RottenLog, Crate, Nest, ClayPot,
    DebrisOakLeaf, DebrisBirchLeaf, DebrisPineNeedle, DebrisTwig, DebrisBark, DebrisWoodChip, DebrisRoot,
    DebrisFernLeaf, DebrisGrassBlade, DebrisMushroomCap, DebrisMushroomStem, DebrisSpore, DebrisAcorn, DebrisSeedHusk,
    DebrisFeather, DebrisBoneChip, DebrisPottery, DebrisCloth, DebrisBrassCase, DebrisStoneChip,
    EncounterGate, GraveVent, Coins,
    Boar, ThornSnail, ThornSnailClosed, LanternMoth, SporeToad, SporeToadSwollen, CrateMimic,
    ThrowingRock, Hatchet, HuntingSpear, Crossbow, Blunderbuss, WoodenMaul, Rake, FlintKnife,
    StatusSleep, StatusStun, StatusChill,
    ShallowWaterA, ShallowWaterB, SpringA, SpringB, Arrow, BombLit, BowDrawn, CanopyOak, CanopyPine,
    Torch, Lighter, OilFlask, SapJar, WaterFlask, MushroomSpores, SmokePot, HoneyPot,
    Rocket, Bolt, Egg, FriedEgg, RootTurret, RootTurretCoiled, BrambleGuard, BrambleGuardSwing,
    Mosquito, MosquitoFed, Owl, OwlFlying, Woodpecker, WoodpeckerDrilling,
    WaspNest, WaspNestStirring, WaspNestEmpty, Wasp, WaspSting,
    ForagerGoblin, ForagerGoblinKnife, CarrionCrow, CarrionCrowSnatch,
    BurrowWormHead, BurrowWormBody, BurrowWormBite,
    ForestTree, ForestTimber, TreeStump, TimberBroken,
    DiggingClaws, ResinGlue, SeedBag, LanternSeed, Shoot, ShootTall, RootCover, LanternPlant,
    HerbBag, Splint, BitterRoot, Chili, FungalBread,
    BirdSeed, ThornCaltrops, BirdSeedPile, ThornPatch, HuntingHorn, RopeHook, HookHead, RootDrill, DrillRoot, SwapSeed, Boomerang, RopeSnare, SnareSet, SnareTight, SpringTrap, SpringReady, AcornMine, AcornReady, ThrowingNet, NetFlight, NetCaught, StickyBoots, RabbitCharm, HandBell, Firecracker, FirecrackerLit, StinkBomb, RottenFruit, PitchBomb, PitchBombLit, StatusNausea, ShieldLantern, ReflectingPan, PanReady, ScarecrowBundle, Scarecrow, DebrisStraw, StrawDecoyBundle, StrawDecoy, WolfWhistle, WolfCalled, ThunderAcorn, ThunderAcornLit, PocketDoor, PocketThreshold, PocketThresholdOpen,
    IceFloorA, IceFloorB, IceWall, IceRuin, Snow, ReservoirIce,
    ColdShallowA, ColdShallowB, ColdWaterA, ColdWaterB,
    RimeSkater, RimeSkaterPush, RimeSkaterGlide, GritPouch,
    FrostBatInhale, FrostBatFlying, FrostPuff, IceNeedle,
    IceHole, BellDiver, DiverBubbles, DiverBell, DiverRise, DiverSwing, DiverSink, AirBladder, ItemFloat, ColdFlask, FrozenWater, ThawingWater, HeatCapsule, SteamLeech, LeechLatch, LeechFeed, LeechSwell, LeechSpent, SteamPuff, WoolWrap, HotBroth, IcePoultice, IceMason, MasonCarry, MasonCut, MasonBuild, MasonJab, IceBlock, IceBlockThaw, DebrisIceChip, Chisel, GlassEel, EelCharge, EelSpent, EelStranded, EelBattery, SnowBurrower, SnowMound, SnowWarn, SnowDive, SnowScoop, Snowball, SnowCache, DebrisSnowClump, LensCarbine, MirrorShard, CrystalLens, DebrisMirrorChip, DebrisCrystalSplinter, MirrorKnight, KnightRaise, KnightGuard, KnightSwing, KnightRecover, KnightShield, KnightShieldLit, PrismBomb, PrismBombLit, PrismBombReady, LensWarden, WardenTurn, WardenCharge, WardenRecover, BeamLamp, BeamLampCharged, BlackFelt, FeltCover, LensCase, DebrisFelt, EchoHound, EchoHoundTrail, EchoHoundWarn, EchoHoundRecover, MufflingFelt, AlarmClock, AlarmClockWound, AlarmClockRinging, DebrisClockGear, FrozenPilgrim, PilgrimThawing, PilgrimThawed, PilgrimStrike, PilgrimFreezing, DebrisWoolTuft, FishingLine, FishingHook, DebrisRopeFiber, FishingWidow, WidowWindup, WidowReel, WidowUntangle, WidowHook, SmokedFish, DebrisFishBone, SealThief, SealCarry, SealEat, SealBark, SealRest, FishingCreel, DebrisWicker, DebrisFishingFloat, WhiteoutDrummer, DrummerLeft, DrummerRight, DrummerRest, SnowGlobe, DebrisGlobeGlass, WeatherVane, DebrisCopperCurl, SaltedKelp, DebrisKelpScrap, BrineFlask, FrozenLunchTin, DebrisTinLid, AvalancheRam, RamPaw, RamLunge, RamStagger, SnowEffigy, EffigyStrike, EffigyFrame, EffigyFrameStrike, CandleStub, CandleLit, WickSpool, DebrisWax, DebrisCharredWick, CandleKeeper, KeeperTend, KeeperStrike, KeeperDim, CandleCabinet, ShardNode, ShardCharged, ShardDim, CoalLump, Stove, StoveLit, DebrisCoal, SteamKettle, KettleFull, KettleHot, IcicleSpider, SpiderWeave, SpiderBite, SpiderRest, SpiderStrand, SpiderStrandV, BoilerPorter, PorterPush, PorterWarn, BoilerTank, BoilerHot, BoilerTell, BoilerPlugged, BoilerNozzle, PressureValve, Sealant, MaintenanceLocker, DebrisBrassRivet,
    IceWindow, IceChain, IceHatch, IcePipe, IceWheel, IceMotor, IceFlag, IceDrip, IceChimney,
    SkateBlade, Crampons,
    SignalFlare, FlareBurning, DebrisFlareCinder,
    CopperWire, WireJunction, GroundingSpike, SpikeHot, SpikeSpent,
    StormLantern, LanternOpen, LanternFocused, EchoPebble, EchoPebbleReady, HarpoonGun, HarpoonHead, EmergencyDoorstop, SluiceGate, SluiceOpen, BorrowedSummer, SummerMote, HeatSiphon, SiphonCharged, ThawCharge, ThawChargeLit, ThawChargeReady, FoldedBridge, BridgePlankH, BridgePlankV, TuningFork, CrystalGrowth, StillwaterBell, EffigyMask, MaskWatching, IceAnchor, AnchorPoint, SnowShelter, SnowWallH, SnowWallV, Sled,
    IndustrialFloorA, IndustrialFloorB, IndustrialWall, IndustrialRuin,
    IndustrialLava, IndustrialLavaA, IndustrialLavaB,
    Pickhand, PickhandRaise, PickhandSwing, ShiftForeman, ForemanWhistle, ForemanRaise, ForemanSwing,
    ForemanWhistleItem,
    Stoker, StokerPack, StokerScoop, StokerSwing, CoalSpit, CoalSpitCold,
    QuarryCharge, QuarryChargeLit, QuarryChargeReady, FuseScissors,
    PowderMonkey, PowderLight, PowderRun, PowderScratch,
    PressHammer, RubberMallet,
    Strikebreaker, BreakerPush, BreakerRaise, BreakerRecover, BreakerUnshielded, BreakerShield,
    GrateH, GrateV, ScrapBin, OreBin, DebrisBasaltChip, DebrisOreFlake, DebrisSteelWasher, DebrisTinCurl,
    RivetGun, Rivet, RivetGunner, GunnerBrace, GunnerFire, GunnerReload, DebrisRivetCasing,
    Conveyor, ConveyorMoving, ConveyorOff, ConveyorBrake, BeltCrank, BrakeShoe, DebrisBeltRubber,
    Yeti, ArcTorch, ArcWelder, WelderMask, HorseshoeMagnet, MagnetCrane, CraneHead, FoldingBarricade, BarricadeSection, PressureRat, RatInflate, RatDash, CoolantCan,
    DebrisBentNail, DebrisChainLink, CableCrawler, CrawlerCharge, PocketDrill, WalkingKiln, KilnOpen, KilnCooled, TensionSpring, AuditClerk, ClerkStamp, ClerkAlarm, PayCage, EmergencyFoam, FoamCover, FurnaceMoth, FurnaceMothHot, BoltPouch, SlagSnail, SlagSnailTucked, SlagSnailCooled, ChainHook, NailBoard, AshMound, AshWhirl, HandBellows, Counterweight, WeightBlock, PocketPump, EmergencyPump, NozzleElbow, MoldThief, MoldOpen, MoldClosed, MoldKey, SteamLance, LanceHot, TarFlask, TarSinger, TarGob, RailCart, RailShunter, RailSwitchKey, RailPoints, InsulatedBoots, GlowSlag, SlagCold, StreetLamp, SteelToe, LunchTin, LunchEmpty, ReactorCore, ReactorFlame,
    RoofLogA, RoofLogB, RoofLogC, RoofLogEndA, RoofLogEndB, RoofLogEndC, RoofFrozenLogA, RoofFrozenLogB, RoofFrozenLogC, RoofFrozenLogEndA, RoofFrozenLogEndB, RoofFrozenLogEndC, RoofGantryA, RoofGantryB, RoofGantryC, RoofGantryEndA, RoofGantryEndB, RoofGantryEndC,
    ChapelPew, ChapelAltar, ChapelRunner, ChapelUrn, ChapelPewBroken, ChapelWax,
    IcePillar, IcePillarCracked, IceRubble,
    FrostGoblin, FrostGoblinRaise, FrostGoblinSwing, PipeGuard, PipeGuardRaise, PipeGuardSwing,
    WolfCrouch, WolfLunge, WolfFangs, WolfFangsClosed,
    IceRockA, IceRockB, IceClearA, IceClearB, IceCloudA, IceCloudB, PackedSnowA, PackedSnowB,
    CoalCutter, CutterTeeth, CoalSeam,
    WaterPipe, WaterPipeBroken, SpringIntake,
    SteamDrive, SteamDriveOff, SteamDriveBroken,
    CraneOperator, CraneOperatorWork, CraneOperatorWait, CraneOperatorAlarm, CraneConsole,
    WidowWorkCast, WidowWorkReel, PickhandCarry,
    RoofLogFarA, RoofLogFarB, RoofLogFarC, RoofLogNearA, RoofLogNearB, RoofLogNearC,
    TallTree, TallTreeCut, TallTreeChar, TallTreeStump, FallenLog,
    LightTower, TowerBuckled, TowerFoot, TowerWreck, TowerLamps,
    LavaFlow0, LavaFlow1, LavaFlow2, LavaFlow3, LavaFlow4, LavaFlow5, LavaFlow6, LavaFlow7,
    LavaGlob,
    FissureCrack, FissureGlow, FissureCooled, FissureSteamA, FissureSteamB, FissureLavaA, FissureLavaB,
    CrateBruised, CrateSplintered, CrateBroken,
    IceAxe, TuskPike, IceAxeArc, TuskPikeThrust,
    Pallet, PalletStack, PalletBroken, BoundRocks, BoundRocksBroken, ContainerSide, ContainerSideBroken, ContainerFloor, LogFloor,
    RoofContainerA, RoofContainerB, RoofContainerC, RoofContainerEndA, RoofContainerEndB, RoofContainerEndC,
    RoofContainerFarA, RoofContainerFarB, RoofContainerFarC, RoofContainerNearA, RoofContainerNearB, RoofContainerNearC,
    DogBody, DogCrouch, DogBite,
    SleepZ,
    FallenLogBruised, FallenLogSplit, FallenLogBroken, WoodBruised, WoodSplit,
    BearBed, BonePile,
    RiverFish, BearFishWatch, BearFishPaw, BearFishSwat, BearFishEat,
    BearCub, BearMother, BearFather, BearOld,
    IceSpikesA, IceSpikesB, IceSpikesC, SnowPileA, SnowPileB, SnowPileC,
    IcePillarForked, IcePillarForkedCracked, IcePillarBroad, IcePillarBroadCracked,
    IceArchFoot, IceArchCrown, IceArchShoulder, IceArchCrownV,
    ForestSpider, ForestSpiderTell, ForestSpiderBite, ForestSpiderYoung, ForestSpiderYoungTell, ForestSpiderYoungBite, ForestSpiderMother, ForestSpiderMotherTell, ForestSpiderMotherBite, ForestWeb0, ForestWeb1, ForestWeb2,
    ForestSpiderFangs, ForestSpiderFangsClosed,
    Snake, SnakeCoil, SnakeStrike,
    Ant, AntWalk, AntTell, AntBite, AntCarry, AntCaptain, AntCaptainWalk, AntCaptainTell, AntCaptainBite, AntCaptainCarry, AntNest, AntSugar, AntSugarLow, AntSugarEmpty, AntPuller, AntPullerWalk, AntPullerTell, AntPullerBite, AntPullerCarry, AntLoad, AntLoadFull,
    Root0, Root1, Root2, Root3, Root4, Root5, Root6, Root7, Root8, Root9, Root10, Root11, Root12, Root13, Root14, Root15, RootCut,
    GiantTreeCanopy, GiantTreeBark,
    MimicOpen, MimicGape, MimicSnap, MimicRecover, MimicJaws, MimicJawsClosed,
    GnomeStick, GnomeStickWalk, GnomeStickTell, GnomeStickHit, GnomeBow, GnomeBowWalk, GnomeBowTell, GnomeBowHit, GnomeCrossbow, GnomeCrossbowWalk, GnomeCrossbowTell, GnomeCrossbowHit, GnomeRider, GnomeRiderWalk, GnomeRiderTell, GnomeRiderHit,
    GnomeHouse, GnomeHouseClosed, GnomeHouseBlue, GnomeHouseBlueClosed, GnomeHouseOchre, GnomeHouseOchreClosed, TallMushroom, TallMushroomBlue, TallMushroomOchre,
    OldGrowthBear, OldGrowthRear, OldGrowthSwipe, OldGrowthPaw, OldGrowthRush, OldGrowthStagger, TallTreeSnowCap, RiverLily, RiverLog,
    Count,
};

struct GameGraphics {
    mutable SDL_Texture* interaction_canvas = nullptr;
    mutable SDL_Texture* overhead_canvas = nullptr;
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
