# Gauche C++ port plan

## Target

Make `gauche` the new C++ game and keep the original Rust history in
`gauche-rs`. Port the *observable* June 2025 prototype closely as the mechanics
and visual baseline: compact pixel art, dark presentation, inventory, tile
damage, chickens, zombies, rail/train behavior, particles, sound, and HUD.
The actual target is a fast, rectilinear, top-down co-op dungeon run: a party
spawns into a dense authored/procedural map, fights and improvises through
rooms, gates, traps, and spawners, and reaches an exit. Roguelike variation,
powerful terrain-changing weapons, and lighting are core direction; the Rust
TestArena is not the final game loop. Add online co-op and stereo directional
sound. Use modern C++20 and SDL3/Gubsy at the host boundary without reproducing
Rust ownership workarounds.

The C++ source should read like Adventures with Chickens: plain value/state
types, free functions, a visible loop in `main`, direct mode switches, and
focused files. Use `splonks-cpp` as the implementation reference for the
Gubsy-owned window/render target, input bindings, asset lifetime, audio,
fixed-step simulation, menu/settings shell, and useful lobby/transport hooks.
Adapt its rollback mechanisms to Gauche's smaller state, leaving its
Spelunky-specific gameplay, network content protocol, theme, and large debug
infrastructure. Gauche's in-game HUD
remains Gauche's HUD; Gubsy owns the surrounding menus and settings.

## Source baseline and current limits

- Rust source: `../gauche-rs` at `5d7e74b`. It is clean and
  `cargo check --locked` passes, with warnings. The existing screenshot is
  `../gauche-rs/screenshots/image.png`.
- Architectural references: `../adventures-with-chickens-remastered-internal`
  at `05dd563` and `../Splonks/splonks-cpp` at `02efeacf`. The latter has a
  substantial dirty working tree, so take a deliberate snapshot of any code
  copied from it rather than silently depending on those edits.
- The Rust loop is 60 Hz and renders 16-pixel tiles into a rescaled render
  texture. Starting a run builds a 64x64 noise-based TestArena and spawns a
  player, 32 zombies, and 32 chickens. Other systems include item use and
  pickup/drop, destructible tiles, spatial queries, rail laying and trains,
  five particle kinds, camera zoom/follow, distance fade, health bars, and
  layered UI.
- The Rust prototype has no actual spawn/exit objective, locked progression,
  room or prefab generator, spawners, keycards/switches, firearms/projectiles,
  or world lighting. `Win` is a placeholder. These are target-game additions,
  not behavior to infer from unused source fields.
- Gauche water is a generated, impassable tile with two intended PNG variants.
  Its two flip passes currently cancel, leaving each cell on its randomized
  initial sprite. It has no fluid amount, flow, buoyancy, or water simulation.
  Actors move between grid cells after tile/occupancy checks; their stored
  velocity field is unused. Particle velocities and accelerations are local
  visual effects, not world physics.
- Rust Gauche already attenuates some world sounds by distance from its sole
  player, but it applies one volume to both channels. It has no left/right
  panning or persistent positional sound instances. It also has one
  `player_vid` and no network/session state.
- Rust Gauche stores particles in `State`, but gameplay only spawns and steps
  them; no collision, damage, inventory, or AI rule reads particle data. Clouds
  also spawn relative to the local camera. They are presentation, not
  synchronized world state.
- Asset IDs are consistent with the files: all 40 `Sprite` enum members, 41
  `SoundEffect` members, and 2 `Song` members resolve to existing PNG/OGG files.
  The directory also has one unlisted `no_sprite.png` and ten unlisted sound
  files (UI/climbing). The authored asset set is about 4 MB.
- The Rust loaders derive lowercase snake-case filenames from enums and eagerly
  load every declared PNG/OGG from paths relative to the process working
  directory. Both music tracks load, but no active call starts one. Graphics
  initialization also requests an absent, unused `grayscale.fs` shader.
- `Settings`, `VideoSettings`, and `Win` are present as mostly unreachable
  modes. The source references `src/shaders/grayscale.fs`, but that file is
  absent. Treat these as gaps in the source, not established game behavior.
- `../gauche-rs/docs/review-notes.md` identifies stale entity handles,
  double-deactivation of free-list slots, and mouse/world conversion based on
  stale window dimensions. Resolve these while preserving gameplay behavior.

## Run loop and world design

A run starts the player and any joined friends near a clear spawn, then asks
them to reach an exit through hostile rooms. A map can remain around 64x64
tiles, but it needs deliberate density: chokepoints, guarded objectives,
spawners, supplies, hazards, and recognizable set pieces. Some routes use keys
or keycards, switches, or local fights. Exit activation and party transition
need explicit co-op rules rather than inheriting the Rust single-player mode.

Generate the **progression graph** before painting terrain: choose a spawn,
exit, critical route, optional branches, and any lock/key or switch dependency.
Then assemble authored room prefabs and connective corridors, with marked
spawn points for enemies, spawners, loot, switches, and set pieces. Allow fixed
rooms mixed with random variants. Validate that every required key or switch
is reachable before its gate and that an ordinary route to the exit exists.
Rare terrain-breaking tools may create shortcuts; the generator must not rely
on finding one to make a run solvable. Keep the first format simple and
compiled-in if possible; this is authored game content, not a mod loader.

Use the tile grid for terrain and simple movement/collision queries. Put
interactive fixtures such as doors, switches, spawners, traps, and exits in the
entity/fixture layer with explicit state and spatial membership. Keys and
weapons use inventory/item rules. This preserves bespoke entity steps while
allowing room prefabs to compose the same small pieces in different layouts.

Combat should be fast and readable on rectilinear terrain. Weapons, rockets,
explosives, and placeable traps can use small top-down hit, projectile, blast,
and tile-change helpers; no Splonks platformer physics is required. Smooth
sub-tile motion may need fixed-point positions and collision even while the
map remains tile aligned. Port the Rust tile-step movement first, then test it
in the first firearm encounter; if it makes aiming and dodging feel too stiff,
adapt player/projectile motion without changing the orthogonal map. Extremely
strong tools should change routes and create memorable consequences. The
source train currently dies when its next tile is not rail; a train that cuts an
otherwise unbreakable wall is a new terrain-changing rule, not source parity.

Lighting is part of the target game's readability and tension. Add a Gauche
2D light pass with dark ambient, tile/wall occlusion, and sources such as
players, exits, muzzle flashes, rockets, fires, and active machinery. Borrow
useful SDL/render-target techniques from Splonks, without its entire light
system. Rendering light may remain local presentation. If visibility later
affects enemy decisions or hidden information, implement that rule separately
in deterministic gameplay state so rollback peers agree.

The host supplies the full initial generated level—terrain, fixtures,
objectives, enemies, loot, and RNG state—to joining peers. Thereafter locks,
switches, spawners, projectiles, blasts, pickups, and terrain changes belong
in the gameplay step, snapshots, and confirmed hashes. Light blooms,
particles, shake, and audio remain local presentation.

## Intended shape

| Part | C++ ownership and source reference |
| --- | --- |
| `main` / shell | Own Gubsy runtime, SDL3 window/renderer/target, graphics, audio, event pump, fixed-step accumulator, drawing and shutdown. Start from the `splonks-cpp` owned-frame path, focused on Gauche's top-down game and co-op needs. |
| `state`, `stage`, `entity`, `inventory`, `item` | Plain Gauche gameplay data and direct operations, based on the Rust rules. Keep a fixed entity pool and versioned handles; maintain a spatial grid. A flat tile array supports the initial 64x64 map and later prefab rooms. Give player avatars stable ownership IDs so online co-op does not require untangling a global single-player assumption later. Store gameplay grid positions and timers as integers; use fixed point for fractional values that affect rules. |
| `rooms`, `objectives`, `fixtures` | Assemble authored and random room layouts around a validated spawn-to-exit progression graph. Place locks/keys, switches, spawners, fixed encounters, traps, and loot as gameplay objects, reusing the entity and tile rules. |
| `inputs`, `step` | Gubsy actions and live mouse coordinates feed explicit input snapshots per player and tick. A pure 60 Hz gameplay step processes movement/items, AI, fixtures, projectiles, terrain, objectives, cleanup, then transitions. Own deterministic gameplay RNG in `State`; keep graphics, audio, weather and other cosmetics outside the hashed simulation. |
| `graphics`, `render`, `render_ui`, `lighting` | Reuse the SDL texture load/unload and render-target pattern, but load Gauche's individual PNGs through a small `Sprite` enum/path table. Draw Gauche-specific world and HUD layers, then an occluded top-down light pass. Convert mouse coordinates using the actual presented viewport, render size, zoom, and camera. |
| `particles` / presentation | Keep Gauche's blood, debris, footprints, corpses, and camera-relative clouds in local presentation state. Spawn them from gameplay event IDs or local weather decisions, with a separate cosmetic RNG. They never affect simulation rules. |
| `audio`, `menus` | Reuse the SDL3 audio device/lifetime and music/SFX ideas with a small Gauche sound table, volume, and per-effect cooldowns. Add source/listener positions for world sounds, with distance attenuation and a small left/right stereo pan; UI sounds stay centered. Use plain Gubsy menu/settings/input/lobby widgets without importing the Splonks theme. |
| `network` | Add host-arbitrated input lockstep with client prediction, bounded rollback, confirmed-frame hashes, and snapshot resync. Every peer simulates the same Gauche gameplay state; the host canonicalizes inputs and owns session decisions. Initial snapshots include generated terrain, fixtures, objectives, and actors. Use Gubsy host/join UI and suitable transport hooks, while keeping Gauche's sync code separate from the game rules. |

## What to take from Splonks, and what to leave there

| Reuse or adapt | Omit from Gauche |
| --- | --- |
| Gubsy-owned SDL3 window, renderer, render target, resize/present path, and a visible fixed-tick loop. Adapt the useful idea of authored room pieces in generated stages to Gauche's spawn-to-exit maps. | Splonks-specific biomes, room templates, quests, shops, progression, and content databases. The Rust TestArena is only a baseline. |
| Gubsy input binding and generic title, pause, settings, controller, host, and join UI. Adapt Splonks' input-frame, prediction/rollback, state-hash, and snapshot-resync patterns to Gauche's much smaller state. | Splonks' game-specific lobby policies, network entity/content protocol, elaborate replay UI, mod hosting, theme, and broad debug UI. |
| SDL texture/audio loading, deterministic cleanup, useful error handling, and simple asset reload only if it helps iteration. | AFrame annotations, animation database, atlas pipeline, per-frame hit/physics boxes, tile source/contact metadata. Gauche's 41 graphics files are individual PNGs with enum names; two water variants and particle sprite lists can switch directly. |
| Gauche's grid occupancy/collision, tile damage, intended water sprite flip, and small particle update rules. Use integer tile positions/tick counters and `gfxp` fixed scalars for rule-relevant fractions and top-down projectiles. Build a focused 2D light pass. | Splonks rigid/platformer physics, gravity, broad fixed-point vector/AABB physics layer, contact solver, fluid/water/lava simulation, and full lighting pipeline. |
| Gauche's 53 OGG files with music, effects and cooldowns; adapt Splonks' small stereo-pan calculation for positioned world sounds. | Splonks' full audio emitter graph, reverb, low-pass filters, and other acoustic processing unless a specific Gauche sound later needs them. |

The omission boundary is about game behavior, not a ban on ordinary velocity
or animation calculations. Gauche's blood, debris, footprints, and clouds
still need their original small particle motions. Keep Rust's distance fade
and dark palette as the presentation baseline, then add Gauche's own lighting
to serve room navigation and combat. Rust requests the missing shader at
startup but never uses it for drawing.

## Asset loading and ownership

Keep Gauche's individual PNGs and OGGs, `Sprite`/`SoundEffect`/`Song` IDs, and
simple direct lookup. The current enum-to-filename convention is already
coherent; no atlas, annotations, YAML database, or asset converter is needed.
Use a small explicit ID/path table or a checked enum-name mapping, with SDL3
texture/audio handles owned by `Graphics` and `Audio`. Validate every declared
path and report the exact missing file on failure. Eager loading is reasonable
for the current 4 MB set; stream the two music tracks if the chosen SDL mixer
supports it. Preserve the unlisted authored files in the repo, but do not load
them until content refers to them.

Resolve assets from a packaged/executable-relative root rather than assuming
the shell's working directory is the repo root. Arrange CMake's development
run path and release bundle so the same table works in both. Borrow Splonks'
SDL texture/audio creation, error handling, and deterministic cleanup; keep
Gauche's own asset naming and omit AFrame/annotation parsing. Remove the unused
shader request. Decide explicitly where title and playing music should start:
the Rust files exist but the current game never calls `play_song`. Add a small
asset-ID-to-file check at the bootstrap gate, plus a normal renderer/audio load
smoke check.

## Tiles and animation

Rust uses the same `TileData` shape for every cell: kind, HP/max HP,
breakability, variant, flip speed, rotation, and shake. The stage is a nested
`Vec<Vec<TileData>>` indexed by x then y. Only water has two tile sprites
(`Water3` and `Water4`); grass, wall, ruin, and rail each have one, and empty
space has none. `Water1` and `Water2` exist in the sprite enum but are not used
by the tile lookup. Water gets a random starting variant and flip speed of 60
ticks at generation. The comment says two seconds, but at 60 Hz that is one.

Animation is explicitly coded, not driven by an asset annotation. Both
`flip_tile` and `flip_stage_tiles` run over every tile in one gameplay step and
toggle water on the same eligible ticks, so they cancel and the current game
shows only the starting variant. Correct the duplicate-pass bug in the C++
port and intentionally animate `Water3`/`Water4` once per second, retaining a
per-cell starting phase. Record this as a small visual fix against the Rust
reference. Do not advance or hash a per-cell animation counter: choose the
water sprite from presentation tick and phase at draw time.

Use one flat row-major tile array with an explicit width, height, and index
helper. Gameplay cells need tile kind and, for placed walls, current HP;
derive wall max HP/breakability and rail's 90-degree orientation from kind for
the current rules. Keep tile shake and water phase in presentation state,
outside rollback hashes. Lookup sprites with a direct type switch or small
fixed array instead of allocating a `Vec<Sprite>` for every rendered tile.
Change tile gameplay state only on placement, damage, and rail events; step
only active local shake effects. Draw only tiles in the visible camera
rectangle. Preserve the current walkability, buildability, wall-to-ruin, and
train-rail rules.

## Entity stepping and content architecture

The Rust prototype has one hostile enemy type, the zombie. Chickens are neutral
wanderers with chick, hen, and rooster stat/sprite variants; rail layers and
trains are scripted entities. Rust's `init_as_*` spawn templates fill one
`Entity` struct. They are procedural archetypes/specs: they serve the same
spawn-default purpose as Splonks' `EntSpec`, without being a data table.
`EntityType` selects behavior; the optional `Sprite` field is independent and
can change.
All three chicken variants stay `Chicken`, with different sprites and stats;
train heads, cars, and cabooses stay `Train`. Current gameplay never tests an
entity's `Sprite`: it uses type, mood, alignment, stats, timers, and item data.
Sprite assignments in gameplay code only change what gets drawn. Keep that
separation in C++.

Rust runs the same ordered list of helpers over every active entity. The
helpers then check type or mood. Zombies and chickens share wandering and
occasional growls; zombies additionally scratch an adjacent entity with
different alignment. The current attack rule can therefore hit a chicken as
well as a player. `Noticing`, `ChasingTarget`, and `LosingTarget` exist as mood
names but have no corresponding enemy logic to port. Preserve the actual simple
rules; do not invent pursuit AI while claiming literal parity.

`Wander` functions as a reusable mood-gated behavior: setting an entity to
`Mood::Wander` made it participate in the generic pass. In the current game,
zombies and chickens are initialized in that mood and do not transition out.
The C++ type steps can call the shared movement helper directly; no global
`Wander` call or unused mood state machine is needed for the existing rules.
Do not add a general AI-mode framework in anticipation of future content.
When a specific entity needs phases, its step can interpret a small state or
counter in the shared `Entity` using named values local to that code. Give
independent concurrent behaviors separate state so they do not fight over one
slot. Serialize and hash gameplay-relevant values for rollback.

Port the current rules with a plain entity pool, an `InitEntityAs` switch on
`EntityType` that calls the appropriate initializer, and a `StepEntity` switch
that calls focused per-type steps. `StepZombie` and `StepChicken` each compose
`Wander`; zombies add their attack rule, while rail layers and trains run their
scripts. Move the zombie-only sprite reset out of Rust's `wander` helper into
`StepZombie`.
Keep common cooldown, damage, inventory, and removal work in shared helpers;
player actions run in the input phase and item entities need no special AI
tick. `MaybeGrowl` remains an optional common presentation pass: setting a
non-player entity's growl sound opts it in without bespoke AI wiring. Preserve
the meaningful Rust update order for each type and use stable pool-slot
iteration for deterministic multiplayer. Gameplay-affecting random choices
use saved gameplay RNG; growl timing and shake use cosmetic RNG outside hashes.

Design this port for a larger, compositional Gauche, even though the source
prototype is small. Put each entity's authored initialization and bespoke step
in focused code; central enum switches only dispatch to those functions.
Share a helper when two steps use the same rule, and retain simple
data-driven common passes such as optional growling. Do not add a spec table,
function-pointer dispatch, generic behavior registry, Splonks'
physics/animation/flag matrix, or runtime mod loading. `EntityType` plus
stored gameplay fields are enough to restore dispatch after rollback. New
gameplay state and AI transitions must be included in snapshots and hashes.

For co-op, the host arbitrates input frames and session events. Each peer
simulates the same deterministic gameplay tick; clients immediately predict
local and missing remote input, then restore a recent snapshot and replay when
the host's confirmed inputs differ. Compare hashes only for confirmed frames,
and request a host snapshot if a real desync or a late input exceeds the
rollback window. Tune input delay and rollback history for measured latency;
rollback cannot hide unlimited round-trip time.

This requires one explicit gameplay RNG state saved in snapshots, stable
entity/tile iteration order, integer/fixed-point rule math, and no dependence
on wall-clock time or graphics state during gameplay steps. The Rust Perlin
arena, and later generated room maps, can be built once by the host and sent
as part of the full initial level snapshot, so cross-platform generation code
does not have to match. Blood, clouds, shake, audio, and other presentation
effects use separate local randomness and stable tick/event IDs to avoid
replaying a sound twice after rollback. The grid and prototype entity set
simplify this relative to Splonks, but joining, ownership, item contention,
deaths, disconnects, and latency still need explicit rules. Start with
shared-world co-op; single-player uses one local player in the same world model.

Lockstep lets a deterministic gameplay feature use the existing input stream
without a new per-entity replication protocol. It still requires peers to run
the same content/version, any new input actions to be encoded, and new
gameplay fields to be covered by save/restore, state hashes, and snapshot
resync. Particles do not meet that threshold: omit them from network packets,
gameplay hashes, and simulation snapshots. Preserve local presentation across
rollback and deduplicate emitted cosmetic events; camera-relative clouds can
remain entirely local. Do not let particle RNG advance the gameplay RNG.

The roughly 7,000 lines of Rust are a behavior map, not a file-for-file
translation template. In particular, `step.rs` temporarily swaps an inventory
item for a dummy Fist to satisfy Rust's mutable-borrow rules; C++ should call
the use rule directly with a valid entity/item reference and an explicit
post-use cleanup. Keep such changes localized and document any rule that
actually changes.

## Work sequence and review gates

1. **Capture the reference.** Inventory reachable modes, controls, items,
   entity rules, render layers, sounds, and constants. Record a few repeatable
   source scenarios and screenshots. If the Rust executable cannot run because
   of the absent shader, use the checked-in screenshot and source behavior as
   reference and record the limitation.
2. **Bootstrap the host.** Add CMake/C++20, SDL3 and Gubsy at a pinned revision,
   a simple run path, and the owned window/render-target loop. Port PNG/OGG
   files, make small sprite/sound lookup tables, validate all declared paths,
   and package an executable-relative asset root. Render the Gauche title and
   make a finite headless smoke/capture command. This gate is a clean build,
   successful asset loads, and a captured title frame.
3. **Port the world and player.** Implement state, versioned entities, tile
   grid, seeded TestArena generation as a source reference, deterministic
   fixed ticks, camera and coordinate conversion, player movement, wall
   collision, and game-over restart. Track player identity separately from
   avatar handles, even while only one player is present. Use integer tile
   coordinates and tick counters, and fixed point for remaining fractional
   gameplay values. Keep `StepGameplay` independent of rendering
   and audio, with an explicit RNG and input frame. This gate is a repeatable
   arena whose gameplay hash matches after replaying the same inputs, plus
   correct click targets after resize/fullscreen.
4. **Port interactions and AI.** Bring over inventory selection/stacking,
   item use and pickup/drop, health/damage, destructible tiles, zombie and
   chicken behavior, rail layers and trains. Validate each against the Rust
   rules; make entity removal safe and grid membership consistent.
5. **Match presentation and sound.** Recreate Gauche's world draw order,
   distance fade, particles/weather, shakes, camera, cursor, range indicators,
   inventory, item details, health bars, and sound cues. Decide how to start
   and stop the two authored music tracks, which Rust loads but never plays.
   Add stereo panning for positioned events while keeping Gauche's existing
   distance falloff and centered UI/music. Reuse Gubsy menus/settings as host
   UI while preserving the game's visual identity. Omit the unused shader.
6. **Build one real run.** Make an authored 64x64 level with a party spawn,
   exit, one key/door or switch dependency, guarded room, enemy spawner, loot,
   first firearm/projectile, explosive or trap, and one train shortcut that
   cuts terrain. Add a focused wall-occluded light pass so darkness and bright
   cues work in the same encounter. Validate that the ordinary route works,
   the exceptional shortcut is intentional, and terrain/fixture state remains
   consistent after combat. This gate is a complete solo spawn-to-exit run.
7. **Add rollback co-op.** Wire Gubsy host/join to a Gauche session. Send a
   full initial level snapshot and tick-stamped per-player input; have the host
   publish canonical inputs. Predict locally, retain a bounded pre-tick
   snapshot history, roll back and replay on input corrections, exchange
   confirmed-frame gameplay hashes, and resync from a host snapshot when
   needed. Keep audio/cosmetic events out of the hash and deduplicate them
   across replay. Validate two processes through party spawn, gate/switch use,
   simultaneous pickup, spawner combat, explosion, train terrain cut, exit
   transition, death, join, and disconnect, including added latency, jitter,
   packet loss, and a deliberate desync.
8. **Expand maps and content.** Assemble prefab rooms from a solvable
   progression graph, mixing fixed landmarks with random rooms, encounters,
   gates, weapons, traps, and loot. Test many seeds for reachability, useful
   route variety, and runs that remain viable without rare wall-breaking gear.
   Tune combat pace, resource pressure, and lighting around the complete run.
9. **Stabilize and publish.** Compare captures and gameplay scenarios, run
   focused deterministic checks for entity handles/grid and item rules, then
   a sanitizer build and normal desktop smoke. Document genuine differences.
   Rename the current GitHub Rust repository to `gauche-rs`, create/push the
   new GitHub `gauche`, update the local origins and links, and make C++ the
   primary README/download target after a playable co-op run is reviewable.

## Boundaries and decisions for implementation

- Keep the old Rust commit/history intact. Do not transplant Rust history into
  the new C++ repository; link it as the source reference.
- Do not port dead placeholders merely to match enum names. Replace the old
  settings stubs with Gubsy settings; the new exit objective supplies a real
  win/transition rule in the first run slice.
- Preserve the old control semantics first (movement, use direction, mouse,
  inventory, pickup/drop, zoom). Gubsy remapping can expose those same actions.
- The first pass should use the original authored PNG/OGG assets. Load each
  PNG directly by its `Sprite` name. Keep asset metadata in code unless a
  concrete Gauche asset later needs richer data.
- Pin the Gubsy dependency rather than relying on whichever of the two local
  Gubsy checkouts happens to be on disk. They currently differ.
- Remote GitHub changes are a later handoff. The local Rust checkout is named
  `gauche-rs`; its `origin` still names the existing GitHub `gauche` until that
  remote migration happens. This new local `gauche` has no remote yet.
