# Gauche C++ port plan

## Target

Make `gauche` the new C++ game and keep the original Rust history in
`gauche-rs`. Port the *observable* June 2025 prototype closely: its top-down
survival play, compact pixel art, dark presentation, inventory, tile damage,
chickens, zombies, rail/train behavior, particles, sound, and HUD. Preserve the
feel and rules while adding online co-op and stereo directional sound as
explicit new capabilities. Use modern C++20 and SDL3/Gubsy at the host boundary
without reproducing Rust ownership workarounds.

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
- Gauche water is a generated, impassable tile that alternates between two
  PNGs. It has no fluid amount, flow, buoyancy, or water simulation. Actors
  move between grid cells after tile/occupancy checks; their stored velocity
  field is unused. Particle velocities and accelerations are local visual
  effects, not world physics.
- Rust Gauche already attenuates some world sounds by distance from its sole
  player, but it applies one volume to both channels. It has no left/right
  panning or persistent positional sound instances. It also has one
  `player_vid` and no network/session state.
- Rust Gauche stores particles in `State`, but gameplay only spawns and steps
  them; no collision, damage, inventory, or AI rule reads particle data. Clouds
  also spawn relative to the local camera. They are presentation, not
  synchronized world state.
- `Settings`, `VideoSettings`, and `Win` are present as mostly unreachable
  modes. The source references `src/shaders/grayscale.fs`, but that file is
  absent. Treat these as gaps in the source, not established game behavior.
- `../gauche-rs/docs/review-notes.md` identifies stale entity handles,
  double-deactivation of free-list slots, and mouse/world conversion based on
  stale window dimensions. Resolve these while preserving gameplay behavior.

## Intended shape

| Part | C++ ownership and source reference |
| --- | --- |
| `main` / shell | Own Gubsy runtime, SDL3 window/renderer/target, graphics, audio, event pump, fixed-step accumulator, drawing and shutdown. Start from the `splonks-cpp` owned-frame path, simplified to Gauche's smaller game and co-op needs. |
| `state`, `stage`, `entity`, `inventory`, `item` | Plain Gauche gameplay data and direct operations, based on the Rust rules. Keep a fixed entity pool and versioned handles; maintain a spatial grid. A contiguous 64x64 tile array is simpler than Rust's nested vectors. Give player avatars stable ownership IDs so online co-op does not require untangling a global single-player assumption later. Store gameplay grid positions and timers as integers; use fixed point only for fractional values that affect rules. |
| `inputs`, `step` | Gubsy actions and live mouse coordinates feed explicit input snapshots per player and tick. A pure 60 Hz gameplay step processes movement/items, AI, tiles, cleanup, then state transitions. Own deterministic gameplay RNG in `State`; keep graphics, audio, weather and other cosmetics outside the hashed simulation. |
| `graphics`, `render`, `render_ui` | Reuse the SDL texture load/unload and render-target pattern, but load Gauche's individual PNGs through a small `Sprite` enum/path table. Draw Gauche-specific world and HUD layers. Convert mouse coordinates using the actual presented viewport, render size, zoom, and camera. |
| `particles` / presentation | Keep Gauche's blood, debris, footprints, corpses, and camera-relative clouds in local presentation state. Spawn them from gameplay event IDs or local weather decisions, with a separate cosmetic RNG. They never affect simulation rules. |
| `audio`, menus | Reuse the SDL3 audio device/lifetime and music/SFX ideas with a small Gauche sound table, volume, and per-effect cooldowns. Add source/listener positions for world sounds, with distance attenuation and a small left/right stereo pan; UI sounds stay centered. Use plain Gubsy menu/settings/input/lobby widgets without importing the Splonks theme. |
| `network` | Add host-arbitrated input lockstep with client prediction, bounded rollback, confirmed-frame hashes, and snapshot resync. Every peer simulates the same Gauche gameplay state; the host canonicalizes inputs and owns session decisions. Use Gubsy host/join UI and suitable transport hooks, while keeping Gauche's sync code separate from the game rules. |

## What to take from Splonks, and what to leave there

| Reuse or adapt | Omit from Gauche |
| --- | --- |
| Gubsy-owned SDL3 window, renderer, render target, resize/present path, and a visible fixed-tick loop. | Splonks stage/biome generation, room templates, quests, shops, progression and content databases. Gauche has one generated TestArena. |
| Gubsy input binding and generic title, pause, settings, controller, host, and join UI. Adapt Splonks' input-frame, prediction/rollback, state-hash, and snapshot-resync patterns to Gauche's much smaller state. | Splonks' game-specific lobby policies, network entity/content protocol, elaborate replay UI, mod hosting, theme, and broad debug UI. |
| SDL texture/audio loading, deterministic cleanup, useful error handling, and simple asset reload only if it helps iteration. | AFrame annotations, animation database, atlas pipeline, per-frame hit/physics boxes, tile source/contact metadata. Gauche's 41 graphics files are individual PNGs with enum names; two water variants and particle sprite lists can switch directly. |
| Gauche's grid occupancy/collision, tile damage, water sprite flip, and small particle update rules. Use integer tile positions/tick counters and `gfxp` fixed scalars for rule-relevant fractions. | Splonks rigid/platformer physics, gravity, broad fixed-point vector/AABB physics layer, contact solver, fluid/water/lava simulation, and dynamic lighting. |
| Gauche's 53 OGG files with music, effects and cooldowns; adapt Splonks' small stereo-pan calculation for positioned world sounds. | Splonks' full audio emitter graph, reverb, low-pass filters, and other acoustic processing unless a specific Gauche sound later needs them. |

The omission boundary is about game behavior, not a ban on ordinary velocity
or animation calculations. Gauche's blood, debris, footprints, and clouds
still need their original small particle motions. Gauche's distance fade and
dark palette also remain, but do not require Splonks' lighting or post-process
systems. The Rust shader is loaded but never used and its file is missing.

## Entity stepping and enemy scope

Gauche has one hostile enemy type, the zombie. Chickens are neutral wanderers
with chick, hen, and rooster stat/sprite variants; rail layers and trains are
scripted entities. Rust uses one `Entity` type with small `init_as_*` functions
and runs the same ordered list of helpers over every active entity. The helpers
then check type or mood. Zombies and chickens share wandering and occasional
growls; zombies additionally scratch an adjacent entity with different
alignment. The current attack rule can therefore hit a chicken as well as a
player. `Noticing`, `ChasingTarget`, and `LosingTarget` exist as mood names but
have no corresponding enemy logic to port. Preserve the actual simple rules;
do not invent pursuit AI while claiming literal parity.

Keep that small model in C++: a plain entity pool, a few focused initialization
functions, common cooldown/damage/removal helpers, and an explicit type switch
to call `step_zombie`, `step_chicken`, `step_rail_layer`, and `step_train` where
needed. Item entities need only their existing item behavior. Preserve the
Rust loop's meaningful order, including player actions before entity updates,
cooldown/AI/death sequencing, and cleanup after the pass; use a stable pool-slot
iteration order for deterministic multiplayer. Random spawn variants, wander
choices, and any gameplay-affecting event must draw from the saved gameplay
RNG. Use cosmetic RNG for growl timing and presentation-only shake, keep those
effects out of the hashed state, and deduplicate sound events during rollback.

Splonks' `EntSpec` registry, per-entity callback dispatch, large AI/state
catalog, AFrame animation hooks, and common/custom physics passes solve a much
larger platformer problem. They are not a template for Gauche's enemy step.
If later content truly needs richer AI, add focused rules then, with their
gameplay fields included in snapshots and hashes.

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
arena can be generated once by the host and sent as the initial tile map, so
cross-platform floating-point noise does not have to match. Blood, clouds,
shake, audio and other presentation effects use separate local randomness and
stable tick/event IDs to avoid replaying a sound twice after rollback. The
grid and small entity set simplify this relative to Splonks, but joining,
ownership, item contention, deaths, disconnects, and latency still need
explicit rules. The initial mode is shared-world co-op; single-player is one
local player in the same world model.

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
   files, make small sprite/sound lookup tables, render the Gauche title, and
   make a finite headless smoke/capture command. This gate is a clean build
   and a captured title frame.
3. **Port the world and player.** Implement state, versioned entities, tile
   grid, seeded TestArena generation, deterministic fixed ticks, camera and
   coordinate conversion, player movement, wall collision, and game-over
   restart. Track player identity
   separately from avatar handles, even while only one player is present. Use
   integer tile coordinates and tick counters, and fixed point for remaining
   fractional gameplay values. Keep `StepGameplay` independent of rendering
   and audio, with an explicit RNG and input frame. This gate is a repeatable
   arena whose gameplay hash matches after replaying the same inputs, plus
   correct click targets after resize/fullscreen.
4. **Port interactions and AI.** Bring over inventory selection/stacking,
   item use and pickup/drop, health/damage, destructible tiles, zombie and
   chicken behavior, rail layers and trains. Validate each against the Rust
   rules; make entity removal safe and grid membership consistent.
5. **Match presentation and sound.** Recreate Gauche's world draw order,
   distance fade, particles/weather, shakes, camera, cursor, range indicators,
   inventory,
   item details, health bars, sound cues and music. Add stereo panning for
   positioned events while keeping Gauche's existing distance falloff and
   centered UI/music. Reuse Gubsy menus/settings as host UI while preserving
   the game's visual identity. Resolve the shader gap only if its effect is
   visible in the reference.
6. **Add rollback co-op.** Wire Gubsy host/join to a Gauche session. Send an
   initial world snapshot and tick-stamped per-player input; have the host
   publish canonical inputs. Predict locally, retain a bounded pre-tick
   snapshot history, roll back and replay on input corrections, exchange
   confirmed-frame gameplay hashes, and resync from a host snapshot when
   needed. Keep audio/cosmetic events out of the hash and deduplicate them
   across replay. Validate two processes through start, movement, simultaneous
   item pickup, enemy combat, death/restart, join, and disconnect, including
   added latency, jitter, packet loss, and a deliberate desync.
7. **Stabilize and publish.** Compare captures and gameplay scenarios, run
   focused deterministic checks for entity handles/grid and item rules, then
   a sanitizer build and normal desktop smoke. Document genuine differences.
   Rename the current GitHub Rust repository to `gauche-rs`, create/push the
   new GitHub `gauche`, update the local origins and links, and make C++ the
   primary README/download target only after parity is reviewable.

## Boundaries and decisions for implementation

- Keep the old Rust commit/history intact. Do not transplant Rust history into
  the new C++ repository; link it as the source reference.
- Do not port dead placeholders merely to match enum names. Replace the old
  settings stubs with Gubsy settings; add a win flow only when there is a real
  game rule to reach it.
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
