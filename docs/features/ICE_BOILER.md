# Boiler porters, tanks and maintenance tools

## Porter and vessel

- Boiler Porter: 68 HP, eighteen-tick walking beat. It maneuvers behind its own
  generation-linked tank, braces for eighteen ticks, then pushes it exactly one
  cardinal cell and steps into the vacated cell. Cover, bodies and loose items
  block the push. Path searches are bounded; actors snap to their actual cells.
- Damage, sleep, stun, rooting or displacement cancels the porter's committed
  action. A nearby player/decoy provokes a thirty-tick warning before a fixed
  adjacent fourteen-damage jab. Without its tank it takes twelve-tick steps and
  pursues targets itself. Killing the porter leaves its tank operating.
- The tank is a separate sixty-HP hard blocker. It blocks passage and sight,
  accepts ordinary weapon damage, and is too heavy for a normal shove. The porter
  moves it explicitly after checking the whole push. Rocks/boomerangs damage it
  before bouncing/landing; arrows, bullets, beams and blasts use existing damage.
- Pair creation reserves actual free cells and both actors; it does not leave a
  half-created porter when the entity pool is full. Tanks start on dry ground.

## Pressure and counterplay

- A tank starts with sixty seconds of fuel. Adjacent Coal Lump use adds twenty
  seconds, capped at 120 seconds. A full vessel rejects coal without consuming it
  or unexpectedly throwing it. Fuel runs down even when the porter is stunned.
- Fuel builds one pressure every three ticks, capped at 100. Without fuel it
  loses one every six ticks. A damaged, unsealed tank leaks one every six ticks;
  standing in water removes two each tick, condensing pressure without deleting
  fuel. The gauge shows pressure; below 25 it is cool enough to recover hardware.
- At 100 pressure it automatically commits a vent. A porter with a visible target
  aligned within four tiles can arm it from 60 pressure. The nozzle direction and
  starting cell are copied for a 45-tick warning; later aiming cannot retarget it.
- A normal vent deals eighteen blockable damage down four cells, then empties
  pressure and rests for thirty ticks. Moving the vessel, cooling below 25 or
  plugging it cancels the pending vent. It must warn again before a later burst.
- Destruction ruptures a cross up to three cells away. At pressure 25 or higher
  it deals twenty-four blockable damage; a cool rupture only spills water. The
  footprint is captured before damage or melting changes cover. Other porters,
  tanks, enemies and teammates can be struck; chain ruptures are permitted.
- Both actions leave ten seconds of water. Hot water melts ice cover/native ice,
  leaves one second of warmth and quenches flames. This is not an oil ignition.
  A rupture is marked spent before resolving damage so it cannot recurse twice.
- Tank pressure runs in the timer phase independently of its owner's decisions.
  All fuel, pressure, plugs, warnings, links and attached hardware are saved and
  hashed integers. Cosmetic steam, sound playback and metal debris stay local.

## Tools and rooms

- Pressure Valve: eighteen gold, nonstackable, 0.5s cooldown. Fits an adjacent
  tank and fixes future vents away from the installer. It does not alter a warning
  already underway. The actual item is stored on the tank; another cannot fit over it.
- Face a cool idle tank and use the normal pickup action to recover its valve.
  Floor loot has priority. Full inventories use the existing swap/drop rules;
  blocked pickup and entity-pool exhaustion leave the valve attached. HUD and
  inventory comparison include this reachable hardware.
- Sealant: nine gold, three portions (Durable six), 0.75s cooldown. Repairs twenty
  HP and plugs the tank for ten seconds; pressure continues building. Unsealing a
  full tank begins a new warning. Damage can still rupture a plugged tank.
- Both items join Ice shops/rewards. Fifteenth regional room role: Boiler Gallery,
  a dry pillared workshop with a porter, optional later leech, maintenance locker
  and budgeted valve/sealant. Bathhouses can also host porters in alternating rounds.
- Maintenance Locker: twenty-four HP, blocking metal, nonflammable. One drop roll:
  20% coal, 20% sealant, 15% valve, 15% ammunition, 30% empty. Its placement respects
  protected routes and does not close a one-cell aisle.
- Porter drop: one roll, 25% valve, next 20% two coal lumps. Tank destruction
  releases attached valve hardware. Leeches now roll 20% Heat Capsule and 10%
  Sealant, completing their previously deferred secondary drop.

## Presentation and verification

- Twelve new 16x16 sprites cover porter poses, tank pressure/plug states, nozzle,
  hardware, sealant, locker and brass rivets. Fifteen original offline-generated
  sounds cover pressure, steam, wheels, hardware, work grunts and container opening.
- A small nozzle overlay shows the actual committed direction. A pressure gauge
  and compact warning/plug countdown remain visible without debug attack patterns.
  Item details distinguish recoverable attachment from consumed supplies.
- Brass Rivet is the nineteenth Ice debris material, with local heavy-piece motion.
  Broken tanks/lockers scatter rivets and copper curls. Loud boiler/worker actions
  enter deterministic hound hearing; local per-cell hiss does not spam hearing.
- Gameplay protocol `0x2026091501`; snapshot layout remains 34. Existing codec
  fixture retains a linked porter/tank, pressure/fuel, plug timer, valve and locker.
- Strict game/render/codec builds, existing codec checks, static warning/plug/item
  captures, PNG/audio checks and diff checks. No live playtest or new test suite.
  The user still owns behavior, difficulty and audiovisual playtesting.
- Ice has twenty implemented enemy behaviors, thirty-one regional items, nineteen
  debris materials and fifteen room roles. Remaining items, ambience, unique floors
  and the other biomes are unfinished; this is not completion of the full catalog.
