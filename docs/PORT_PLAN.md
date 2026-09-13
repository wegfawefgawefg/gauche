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
Copy the useful mechanisms, not its Spelunky-specific gameplay, lockstep/
rollback model, theme, or large debug infrastructure. Gauche's in-game HUD
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
| `state`, `stage`, `entity`, `inventory`, `item`, `particle` | Plain Gauche data and direct operations, based on the Rust rules. Keep a fixed entity pool and versioned handles; maintain a spatial grid. A contiguous 64x64 tile array is simpler than Rust's nested vectors. Give player avatars stable ownership IDs so online co-op does not require untangling a global single-player assumption later. |
| `inputs`, `step` | Gubsy actions and live mouse coordinates feed one explicit input snapshot per tick. At 60 Hz, process player movement and items, AI, tiles, particles, cleanup, then state transitions. Keep simulation RNG seeded and separate from cosmetic randomness. |
| `graphics`, `render`, `render_ui` | Reuse the SDL texture load/unload and render-target pattern, but load Gauche's individual PNGs through a small `Sprite` enum/path table. Draw Gauche-specific world and HUD layers. Convert mouse coordinates using the actual presented viewport, render size, zoom, and camera. |
| `audio`, menus | Reuse the SDL3 audio device/lifetime and music/SFX ideas with a small Gauche sound table, volume, and per-effect cooldowns. Add source/listener positions for world sounds, with distance attenuation and a small left/right stereo pan; UI sounds stay centered. Use plain Gubsy menu/settings/input/lobby widgets without importing the Splonks theme. |
| `network` | Add a small co-op layer around Gauche's 60 Hz world: one host runs AI, tile changes, items, trains, and random choices; clients send player input and receive authoritative state/events. Use Gubsy host/join UI and suitable transport hooks, but keep Gauche's synchronization code separate from the game rules. |

## What to take from Splonks, and what to leave there

| Reuse or adapt | Omit from Gauche |
| --- | --- |
| Gubsy-owned SDL3 window, renderer, render target, resize/present path, and a visible fixed-tick loop. | Splonks stage/biome generation, room templates, quests, shops, progression and content databases. Gauche has one generated TestArena. |
| Gubsy input binding and generic title, pause, settings, controller, host, and join UI. Adapt transport/session hooks if they fit Gauche's host-authoritative model. | Splonks' game-specific lobby policies, lockstep prediction/rollback, replay/desync machinery, mod hosting, theme, and broad debug UI. |
| SDL texture/audio loading, deterministic cleanup, useful error handling, and simple asset reload only if it helps iteration. | AFrame annotations, animation database, atlas pipeline, per-frame hit/physics boxes, tile source/contact metadata. Gauche's 41 graphics files are individual PNGs with enum names; two water variants and particle sprite lists can switch directly. |
| Gauche's grid occupancy/collision, tile damage, water sprite flip, and small particle update rules. | Splonks rigid/platformer physics, gravity, fixed-point world math, contact solver, fluid/water/lava simulation, and dynamic lighting. |
| Gauche's 53 OGG files with music, effects and cooldowns; adapt Splonks' small stereo-pan calculation for positioned world sounds. | Splonks' full audio emitter graph, reverb, low-pass filters, and other acoustic processing unless a specific Gauche sound later needs them. |

The omission boundary is about game behavior, not a ban on ordinary velocity
or animation calculations. Gauche's blood, debris, footprints, and clouds
still need their original small particle motions. Gauche's distance fade and
dark palette also remain, but do not require Splonks' lighting or post-process
systems. The Rust shader is loaded but never used and its file is missing.

For co-op, host authority is the starting design because Gauche's scattered
random calls and render-time cosmetic randomness make deterministic peer
lockstep an unnecessary prerequisite. The host sends an initial world snapshot
and later tick-stamped state changes; each player sends actions for their own
avatar. Clients play audio locally from positioned game events. The grid and
small entity set simplify this relative to Splonks, but joining, ownership,
item contention, deaths, disconnects, and latency still need explicit rules.
The initial game mode is shared-world cooperative survival; preserve the
single-player path as one local player in the same world model.

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
   files, make small sprite/sound lookup tables, render the Gauche title, and make a
   finite headless smoke/capture command. This gate is a clean build and a
   captured title frame.
3. **Port the world and player.** Implement state, versioned entities, tile
   grid, seeded TestArena generation, fixed ticks, camera/coordinate conversion,
   player movement, wall collision, and game-over restart. Track player identity
   separately from avatar handles, even while only one player is present. This
   gate is a repeatable generated arena with movement and correct click targets
   after resize/fullscreen.
4. **Port interactions and AI.** Bring over inventory selection/stacking,
   item use and pickup/drop, health/damage, destructible tiles, zombie and
   chicken behavior, rail layers and trains. Validate each against the Rust
   rules; make entity removal safe and grid membership consistent.
5. **Match presentation and sound.** Recreate Gauche's world draw order,
   distance fade, particles/weather, shakes, camera, cursor, range indicators, inventory,
   item details, health bars, sound cues and music. Add stereo panning for
   positioned events while keeping Gauche's existing distance falloff and
   centered UI/music. Reuse Gubsy menus/settings as host UI while preserving
   the game's visual identity. Resolve the shader gap only if its effect is
   visible in the reference.
6. **Add co-op.** Wire Gubsy host/join to a Gauche session. Run one authoritative
   simulation on the host; send per-player inputs and an initial snapshot plus
   tick-stamped entity, tile, inventory and event updates. Rebuild client grids
   from authoritative data, and play positional audio from synced events.
   Validate two processes through start, movement, simultaneous item pickup,
   enemy combat, death/restart, join, and disconnect. Add interpolation or
   local input prediction only where the measured feel needs it.
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
