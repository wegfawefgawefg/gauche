# Teeming C++ port plan

Original architectural blueprint. Current open work is tracked in the
[Teeming master task list](../MASTER_TASKS.md).

## Target

Make `teeming` the new C++ game and keep the original Rust history in
`gauche-rs`. Port the *observable* June 2025 prototype closely as the mechanics
and visual baseline: compact pixel art, dark presentation, inventory, tile
damage, chickens, zombies, rail/train behavior, particles, sound, and HUD.
The actual target is a fast, rectilinear, top-down co-op dungeon run: a party
spawns into a dense authored/procedural map, fights and improvises through
rooms, gates, traps, and spawners, and reaches an exit. Cleared floors lead to
three-way reward choices, occasional shops, and themed four-floor worlds.
Roguelike variation, powerful terrain-changing weapons, and lighting are core
direction; the Rust TestArena is not the final game loop. Add online co-op and
stereo directional sound. Use modern C++20 and SDL3/Gubsy at the host boundary
without reproducing Rust ownership workarounds.

The C++ source should read like Adventures with Chickens: plain value/state
types, free functions, a visible loop in `main`, direct mode switches, and
focused files. Use `splonks-cpp` as the implementation reference for the
Gubsy-owned window/render target, input bindings, asset lifetime, audio,
fixed-step simulation, menu/settings shell, and useful lobby/transport hooks.
Adapt its rollback mechanisms to Teeming's simpler state, leaving its
Spelunky-specific gameplay, network content protocol, theme, and large debug
infrastructure. Teeming's in-game HUD
remains Teeming's HUD; Gubsy owns the surrounding menus and settings.

## Source baseline

The [Rust source audit](SOURCE_AUDIT.md) records the prototype behavior and gaps used for this port.

## Run loop and world design

A run starts the player and any joined friends near a clear spawn, then asks
them to reach an exit through hostile rooms. Rust's 64×64 TestArena is a source
reference, not a target floor size. Let the room graph and pacing determine a
floor's width and height; larger maps are welcome when they stay dense with
chokepoints, guarded objectives, spawners, supplies, hazards, and recognizable
set pieces. Some routes use keys or keycards, switches, or local fights. Exit
activation and party transition need explicit co-op rules rather than
inheriting the Rust single-player mode.

Treat a run as a sequence of themed worlds, provisionally four floors each.
Floors one and two establish that world's familiar rooms and enemies; floor
three introduces a special layout or encounter; floor four is a harder
capstone. Keep that cadence flexible enough for authored exceptions. Each
floor still has its own reachable spawn-to-exit progression graph. Clearing a
floor that continues the run opens a safe interlude: each eligible player
chooses one of three rewards, then the party can visit an occasional shop
before entering the next floor.

Generate the **progression graph** before painting terrain: choose a spawn,
exit, critical route, optional branches, and any lock/key or switch dependency.
Then assemble authored room prefabs and connective corridors, with marked
spawn points for enemies, spawners, loot, switches, and set pieces. Allow fixed
rooms mixed with random variants. Validate that every required key or switch
is reachable before its gate and that an ordinary route to the exit exists.
Rare terrain-breaking tools may create shortcuts; the generator must not rely
on finding one to make a run solvable. Keep the first format simple and
compiled-in if possible; this is authored game content, not a mod loader.

Place forest room stamps on an expandable tile canvas. Choose them by required
door sockets and room role, then populate each from its tagged local loot/enemy
pools under a floor-wide supply and threat budget. Room stamps may vary in
size; trim and pad the final bounds after the graph is embedded.
See [the forest content sketch](../design/FOREST_CONTENT_IDEAS.md) for candidate rooms,
items, artifacts, hazards, and rewards.

Use the tile grid for terrain and simple movement/collision queries. Put
interactive fixtures such as doors, switches, spawners, traps, and exits in the
entity/fixture layer with explicit state and spatial membership. Keys and
weapons use inventory/item rules. This preserves bespoke entity steps while
allowing room prefabs to compose the same small pieces in different layouts.

Combat should be fast and readable on rectilinear terrain. **Keep Teeming's
tile-step actor movement**: its quick cadence and hard cell occupancy are part
of the game, and make blocking or opening a tile tactically meaningful. Weapons,
rockets, explosives, and placeable traps can use small top-down hit, projectile,
blast, and tile-change helpers; no Splonks platformer physics is required.
Projectiles may use fixed-point sub-tile positions, and visuals may interpolate,
without changing actor movement into continuous physics. Pickaxes, bombs, and
barricades should act on the same tile/passability rules as doors and walls.
Strong tools should change routes and create memorable consequences.

Make the buckler an active facing block with a short shove. A shoved actor
that would enter a hard, impassable blocker—a wall, solid fixture, or an enemy
that cannot be moved—is crushed instantly, whether the shoved actor is enemy
or player. The blocker survives. A normal teammate or loose item is not a
crush surface merely because it occupies a tile; handle ordinary occupancy
with the normal push/collision rule. An item shoved into a wall breaks.
Telegraph push direction and give the action a recovery cost rather than
softening a real crush into ordinary damage.

The conductor hat already demonstrates that in Rust: using it spawns a rail
layer, which travels across the row and replaces each in-bounds tile with rail
regardless of normal tile collision or breakability. Only after the track is
laid does it spawn the train, which follows that track. Preserve the existing
track-first shortcut rather than treating wall cutting as a new train rule.
When adding new blocking fixtures, make their interaction with rail laying
explicit so they do not accidentally negate that behavior.

Add a broad set of distinct firearms, plus supplies such as sleep meds. Each
weapon instance owns its own loaded and spare ammo counts; its weapon kind
defines magazine size, firing, and reload behavior. Reloading moves rounds
from that weapon's spare count into its magazine. A rocket launcher's rockets
never become a pistol's bullets. Avoid an inventory full of ammo categories:
one generic ammo pickup can add spare rounds to the held weapon, while a
larger pickup can add spare rounds to each carried weapon separately, using
appropriate amounts for each. Neither pickup creates a shared ammo reserve.
Keep ammo generous; encounters, positioning, and route choices should carry
more pressure than chronic ammo starvation. Firing, reloads, pickups, and any
item effects that change gameplay must be deterministic and included in
snapshots and hashes.

### Rewards, inventory, and statuses

Offer a mixed three-choice draft after each cleared floor: a carried item or
weapon, a passive artifact, or a lasting power-up effect can appear in any
slot. Curate offers so they remain useful to the player's current loadout and
world; a consumable or gun offer should be strong enough to compete with a
lasting artifact. Each player chooses independently in co-op. Save the
generated offers and choices in host session state; a disconnected player's
pending choice waits for their return without blocking the rest of the party's
transition. The death policy determines whether a dead player is eligible for
a reward.

Reduce the quick-use inventory from Rust's ten slots to **six initially**, then
playtest whether five feels better. Guns, consumables, and placeables compete
for those active slots. Passive artifacts and each carried weapon's own ammo
counts do not occupy extra quick slots. When a reward or shop purchase would
overfill the pack, let the player deliberately replace or drop an item in the
safe interlude; never silently discard the chosen reward. Preserve the Rust
ten-slot behavior only while validating source parity, then make this explicit
target-game change.

Keep the character sheet small: health and movement step interval are
meaningful; a broad strength/defense/agility stat ladder is unnecessary.
Speed bonuses shorten the integer number of ticks between tile steps, with a
floor that preserves readable movement. Most artifacts should change a
specific rule instead: for example, an aura that helps friends within a
grid-distance radius, an effect on reload, or a response to taking damage.
Rare artifacts can break a whole rule instead of merely adding a percentage:
"All Piercing" can let every direct attack pass through actors along its line
or arc, while a mirror artifact can sometimes reflect a direct hit. Keep
solid-wall interaction separate from actor piercing so terrain tools remain
distinct. Reflection rolls use saved gameplay RNG and cannot reflect a
reflection forever; show the proc clearly in graphics and sound.
Aura membership and stacking order must be deterministic. Give sleep, stun,
frozen, and burning distinct, readable effects and integer-tick durations;
specify wake-up, movement/action limits, and damage timing before adding each
one. Sleep meds can use that same status machinery. Avoid building a general
effect scripting framework for the first few artifacts and statuses.

Shops are optional interludes, not a stop after every floor. Seed their
appearance and stock when a world is built, announce an upcoming shop on the
run map, and offer a reliable way to turn found currency into needed supplies
or a build choice. This gives players something to plan around without
replacing the free reward after a clear. The host resolves purchases and saves
stock, currency, and any resulting loadout change in the run state.

### Themed worlds and first content set

Start with a forest world that mixes leafy outdoor rooms and grassy caves
with shafts of light from the roof. Give it authored room pieces, forest dens
as optional set pieces (especially plausible on floors two or four), and
encounters featuring bats, wolves, and bears. Seed its item pool with a bow
and visible arrow projectiles, a musket, and bear traps alongside other Teeming
tools. The bow owns its own arrow count and nocking behavior; it does not draw
from a pistol or launcher.

Fire and ice worlds are later themes, each with its own terrain, hazards,
enemy mix, room shapes, lighting, and status interactions. A world should
change the decisions in a run, not just recolor the same floor. The third-
floor special and fourth-floor harder pattern should help make each world
recognizable while leaving room for rare variants.

Lighting is part of the target game's readability and tension. Add a Teeming
2D light pass with dark ambient, tile/wall occlusion, and sources such as
players, exits, muzzle flashes, rockets, fires, and active machinery. Borrow
useful SDL/render-target techniques from Splonks, without its entire light
system. Rendering light may remain local presentation. If visibility later
affects enemy decisions or hidden information, implement that rule separately
in deterministic gameplay state so rollback peers agree.

The host supplies the full initial generated level—terrain, fixtures,
objectives, enemies, loot, and RNG state—to joining peers. Thereafter locks,
switches, spawners, projectiles, blasts, pickups, terrain changes, statuses,
artifacts, reward choices, and shop purchases belong in the gameplay step,
snapshots, and confirmed hashes. Light blooms, particles, shake, and audio
remain local presentation.

Lockstep's normal per-tick network traffic is player inputs, not a copy of the
map. A larger floor does enlarge initial join/rejoin and desync-resync
snapshots. It can also raise local generation, AI/path-query, hash, rollback
history, and memory costs, especially if every saved frame copies every tile.
For scale, 256×256 tiles at four bytes each are 256 KiB; 120 full rollback
copies would use about 30 MiB for tiles alone. That is manageable on a desktop
but grows with area and does not include entities or other state.

Use a compact flat tile array with dynamic dimensions, render only visible
tiles, and keep spatial queries local. Measure realistic larger floors before
adding incremental hashing or changed-tile rollback storage; those are
options if full-state work becomes expensive. Preserve the same deterministic
activation/iteration rules on all peers even when players split up.

## Intended shape

| Part | C++ ownership and source reference |
| --- | --- |
| `main` / shell | Own Gubsy runtime, SDL3 window/renderer/target, graphics, audio, event pump, fixed-step accumulator, drawing and shutdown. Start from the `splonks-cpp` owned-frame path, focused on Teeming's top-down game and co-op needs. |
| `state`, `stage`, `entity`, `inventory`, `item` | Plain Teeming gameplay data and direct operations, based on the Rust rules. Keep a fixed entity pool and versioned handles; maintain a spatial grid. A compact flat tile array with explicit dynamic width/height supports source TestArena and larger generated floors. Give player avatars stable ownership IDs so online co-op does not require untangling a global single-player assumption later. Preserve tile-step actor positions and integer timers; use fixed point for fractional projectile or other values that affect rules. |
| `rooms`, `objectives`, `fixtures` | Assemble themed four-floor worlds from authored and random rooms, each with a validated spawn-to-exit progression graph. Place locks/keys, switches, spawners, fixed encounters, dens, traps, and loot as gameplay objects, reusing the entity and tile rules. |
| `run`, `rewards`, `shop`, `status` | Track world/floor cadence, per-player three-choice drafts, occasional announced shops, passive artifacts, and small explicit status timers. Keep six quick-use slots separate from artifacts; each weapon instance stores its own ammo. Let the host generate offers and stock; serialize every gameplay-relevant choice and effect. |
| `inputs`, `step` | Gubsy actions and live mouse coordinates feed explicit input snapshots per player and tick. A pure 60 Hz gameplay step processes movement/items, AI, fixtures, projectiles, terrain, objectives, cleanup, then transitions. Own deterministic gameplay RNG in `State`; keep graphics, audio, weather and other cosmetics outside the hashed simulation. |
| `graphics`, `render`, `render_ui`, `lighting` | Reuse the SDL texture load/unload and render-target pattern, but load Teeming's individual PNGs through a small `Sprite` enum/path table. Draw Teeming-specific world and HUD layers, then an occluded top-down light pass. Convert mouse coordinates using the actual presented viewport, render size, zoom, and camera. |
| `particles` / presentation | Keep Teeming's blood, debris, footprints, corpses, and camera-relative clouds in local presentation state. Spawn them from gameplay event IDs or local weather decisions, with a separate cosmetic RNG. They never affect simulation rules. |
| `audio`, `menus` | Reuse the SDL3 audio device/lifetime and music/SFX ideas with a small Teeming sound table, volume, and per-effect cooldowns. Add source/listener positions for world sounds, with distance attenuation and a small left/right stereo pan; UI sounds stay centered. Use plain Gubsy menu/settings/input/lobby widgets plus a small run-route, reward, and shop UI without importing the Splonks theme. Expose the host's co-op death policy in game/lobby settings. |
| `network` | Add host-arbitrated input lockstep with client prediction, bounded rollback, confirmed-frame hashes, snapshot resync, and reconnect to a retained player slot. Every peer simulates the same Teeming gameplay state; the host canonicalizes inputs and owns session decisions. Initial snapshots include generated terrain, fixtures, objectives, and actors. Use Gubsy host/join UI and suitable transport hooks, while keeping Teeming's sync code separate from the game rules. |

## What to take from Splonks, and what to leave there

| Reuse or adapt | Omit from Teeming |
| --- | --- |
| Gubsy-owned SDL3 window, renderer, render target, resize/present path, and a visible fixed-tick loop. Adapt the useful idea of authored room pieces in generated stages to Teeming's spawn-to-exit maps. | Splonks's biome, quest, shop, and progression content and large data systems. Write Teeming's smaller themed worlds, rewards, and shops for its own run structure. The Rust TestArena is only a baseline. |
| Gubsy input binding and generic title, pause, settings, controller, host, and join UI. Adapt Splonks' input-frame, prediction/rollback, state-hash, and snapshot-resync patterns to Teeming's simpler gameplay state. | Splonks' game-specific lobby policies, network entity/content protocol, elaborate replay UI, mod hosting, theme, and broad debug UI. |
| SDL texture/audio loading, deterministic cleanup, useful error handling, and simple asset reload only if it helps iteration. | AFrame annotations, animation database, atlas pipeline, per-frame hit/physics boxes, tile source/contact metadata. Teeming's 41 graphics files are individual PNGs with enum names; two water variants and particle sprite lists can switch directly. |
| Teeming's grid occupancy/collision, tile damage, intended water sprite flip, and small particle update rules. Use integer tile positions/tick counters and `gfxp` fixed scalars for rule-relevant fractions and top-down projectiles. Build a focused 2D light pass. | Splonks rigid/platformer physics, gravity, broad fixed-point vector/AABB physics layer, contact solver, fluid/water/lava simulation, and full lighting pipeline. |
| Teeming's 53 OGG files with music, effects and cooldowns; adapt Splonks' small stereo-pan calculation for positioned world sounds. | Splonks' full audio emitter graph, reverb, low-pass filters, and other acoustic processing unless a specific Teeming sound later needs them. |

The omission boundary is about game behavior, not a ban on ordinary velocity
or animation calculations. Teeming's blood, debris, footprints, and clouds
still need their original small particle motions. Keep Rust's distance fade
and dark palette as the presentation baseline, then add Teeming's own lighting
to serve room navigation and combat. Rust requests the missing shader at
startup but never uses it for drawing.

## Asset loading and ownership

Keep Teeming's individual PNGs and OGGs, `Sprite`/`SoundEffect`/`Song` IDs, and
simple direct lookup. The current enum-to-filename convention is already
coherent; no atlas, annotations, YAML database, or asset converter is needed.
Use a small explicit ID/path table or a checked enum-name mapping, with SDL3
texture/audio handles owned by `Graphics` and `Audio`. Validate every declared
path and report the exact missing file on failure. Eager loading is reasonable
for the current 4 MB set; stream the two music tracks if the chosen SDL mixer
supports it. Preserve the unlisted authored files in the repo, but do not load
them until content refers to them.

For new content, use quick Python-generated pixel placeholders when that
helps test a mechanic. Existing tiles and most actors/items are 16×16 pixels;
smaller particles and occasional larger details are exceptions. Preserve the
readable silhouette and palette, then polish art after the item or room proves
useful in play. The source-port gate still uses its original PNGs.

Resolve assets from a packaged/executable-relative root rather than assuming
the shell's working directory is the repo root. Arrange CMake's development
run path and release bundle so the same table works in both. Borrow Splonks'
SDL texture/audio creation, error handling, and deterministic cleanup; keep
Teeming's own asset naming and omit AFrame/annotation parsing. Remove the unused
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

Design this port for a larger, compositional Teeming, even though the source
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

Treat a disconnect as a lost connection, not a death or a new character. Keep
the player's stable ID and slot for the run, retain their avatar and inventory,
and apply neutral/no input while absent. A returning peer reclaims that slot
through its session identity and receives a current host snapshot plus the
input/catch-up state needed to resume; it must not create a duplicate avatar.
If the unattended avatar dies, the ordinary death policy applies. Carry this
identity through level transitions so a late return can still rejoin.

Make death policy a host-configured run setting, shared in the session
handshake and deterministic state. Splonks already offers the useful three
choices: no respawn (permadeath for that run), respawn at entrance, and return
on the next level. Implement the corresponding Teeming rules explicitly,
including what happens when the party reaches an exit with a dead member.
Reconnecting never overrides the selected death rule. Pick the default after
the first co-op playtest rather than burying it in the network layer.

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
   and package an executable-relative asset root. Render the Teeming title and
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
5. **Match presentation and sound.** Recreate Teeming's world draw order,
   distance fade, particles/weather, shakes, camera, cursor, range indicators,
   inventory, item details, health bars, and sound cues. Decide how to start
   and stop the two authored music tracks, which Rust loads but never plays.
   Add stereo panning for positioned events while keeping Teeming's existing
   distance falloff and centered UI/music. Reuse Gubsy menus/settings as host
   UI while preserving the game's visual identity. Omit the unused shader.
6. **Build one real run.** Make an authored level with a party spawn,
   exit, one key/door or switch dependency, guarded room, enemy spawner, loot,
   first firearm/projectile with magazine and ammo pickups, buckler block/shove
   and hard-blocker crush, explosive or trap, a tile-blocking barricade or
   tile-opening tool, and the existing rail-laying shortcut from the
   conductor hat. Add a focused wall-occluded light pass so darkness and
   bright cues work in the same encounter. Validate that the ordinary route
   stays open, the rail shortcut is intentional, and terrain/fixture state
   remains consistent after combat.
   Add a second small floor and the first three-choice reward interlude so the
   gate includes a complete solo clear-and-continue loop. Test at least two
   different stage dimensions rather than baking the Rust TestArena size into
   gameplay.
7. **Add rollback co-op.** Wire Gubsy host/join to a Teeming session. Send a
   full initial level snapshot and tick-stamped per-player input; have the host
   publish canonical inputs. Predict locally, retain a bounded pre-tick
   snapshot history, roll back and replay on input corrections, exchange
   confirmed-frame gameplay hashes, and resync from a host snapshot when
   needed. Keep audio/cosmetic events out of the hash and deduplicate them
   across replay. Validate two processes through party spawn, gate/switch use,
   simultaneous pickup, firing/reload and ammo resupply, spawner combat,
   buckler hard-blocker crush (including a player), explosion, rail-laid
   terrain cut, exit transition, each death policy, per-player reward
   selection, disconnect and rejoin to the same slot after a level change,
   with added latency, jitter, packet loss, and a deliberate desync.
8. **Build the forest world.** Assemble its four floors from a solvable
   progression graph, mixing grassy caves, roof-light shafts, outdoor rooms,
   dens, fixed landmarks, and random rooms. Add bats, wolves, bears, bow,
   musket, bear traps, many guns, sleep meds, and other loot. Establish the
   familiar/special/harder floor cadence, six-slot quick pack, passive
   artifacts, a speed modifier tied to step interval, distinct statuses,
   mixed three-choice rewards, and an occasional announced between-floor
   shop. Test seeds for reachability, route variety, useful reward choices,
   and viable runs without rare wall-breaking gear; verify multiplayer reward
   and purchase replay, reconnect, and snapshot restore on larger maps.
9. **Extend worlds and tune.** Add fire and ice themes with distinct room
   shapes, hazards, enemies, lighting, and status interactions. Tune combat
   pace, generous ammo supply, shop frequency, artifact auras, and four-floor
   world pacing around complete co-op runs.
10. **Stabilize and publish.** Compare captures and gameplay scenarios, run
   focused deterministic checks for entity handles/grid and item rules, then
   a sanitizer build and normal desktop smoke. Document genuine differences.
   Rename the current GitHub Rust repository to `gauche-rs`, create/push the
   new GitHub `teeming`, update the local origins and links, and make C++ the
   primary README/download target after a playable co-op run is reviewable.

## Implementation decisions

See [implementation decisions](IMPLEMENTATION_DECISIONS.md) for the source-history, asset, and remote boundaries.
