# Teeming Rust source audit

- Rust source: [gauche-rs at `5d7e74b`](https://github.com/wegfawefgawefg/gauche-rs/tree/5d7e74b).
  At the audit it was clean and `cargo check --locked` passed with warnings.
  The existing [screenshot](https://github.com/wegfawefgawefg/gauche-rs/blob/5d7e74b/screenshots/image.png)
  shows the prototype.
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
- Rust inventory has ten quick slots. It has no between-floor rewards, shops,
  artifacts, or implemented sleep/stun/freeze/burn rules; an unused
  `can_be_stunned` entity field is not a status system.
- Teeming water is a generated, impassable tile with two intended PNG variants.
  Its two flip passes currently cancel, leaving each cell on its randomized
  initial sprite. It has no fluid amount, flow, buoyancy, or water simulation.
  Actors move between grid cells after tile/occupancy checks; their stored
  velocity field is unused. Particle velocities and accelerations are local
  visual effects, not world physics.
- Rust Teeming already attenuates some world sounds by distance from its sole
  player, but it applies one volume to both channels. It has no left/right
  panning or persistent positional sound instances. It also has one
  `player_vid` and no network/session state.
- Rust Teeming stores particles in `State`, but gameplay only spawns and steps
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
- The [Rust review notes](https://github.com/wegfawefgawefg/gauche-rs/blob/5d7e74b/docs/review-notes.md)
  identify stale entity handles,
  double-deactivation of free-list slots, and mouse/world conversion based on
  stale window dimensions. Resolve these while preserving gameplay behavior.
