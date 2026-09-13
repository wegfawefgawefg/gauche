# Gauche

Gauche is a C++20, tile-step co-op dungeon run built from the original [Rust prototype](../gauche-rs). The Rust TestArena, its actors, items, train, art, and sound are the baseline. The new run adds keyed and switched exits, procedural room routes, forest/fire/ice worlds, guns, traps, artifacts, rewards, shops, lighting, and direct UDP co-op.

## Build and run

The project uses SDL3 and the pinned Gubsy host. CMake fetches dependencies when they are not available locally. From the repository root, build and launch with:

```sh
./scripts/run.sh
```

Set `GAUCHE_PRESET=dev` for a debug build. `./scripts/build.sh` builds without launching. Both scripts work from any current directory; `run.sh` passes game arguments through unchanged.

The GView title page uses the stone controls from the Splonks C++ menu workspace. Play opens Gubsy's lobby for solo play or direct hosting and joining; Quick Run starts alone. Settings opens Gubsy's display, audio, and control configuration. One player can join from each machine, up to four total. The default direct host port in the lobby is 35355. A CLI host can use a chosen port:

```sh
./scripts/run.sh --host 39000 --death next-floor
./scripts/run.sh --join HOST_IP:39000
```

`--death` accepts `no-respawn`, `entrance`, or `next-floor`. A player who disconnects keeps their slot and loadout, can return with the same local identity, and does not block the party's exit or reward choices. Direct hosting requires the UDP port to be reachable; there is no relay or room-code service yet.

Move with WASD; aim and use with the arrow keys or left mouse button, or use the held item with Space. On a controller, move with the left stick or D-pad, turn with the right stick, use with the right trigger, and cycle quick slots with the bumpers. Select one of six quick slots with 1–6 or the keypad, pick up with E, interact with F, drop with Q, and reload with R. The mouse wheel or `-`/`=` changes zoom. Gameplay keys and controller buttons are editable in Gubsy's Controls screen, and its audio levels control Gauche's music and directional effects. Stand near the exit together, clear its key or switch gate, then choose one of three rewards. Enter continues from a shop. The host can press Enter to start a new run after a loss or clear. Escape opens the in-game menu.

## Current game

Each four-floor world changes terrain, encounters, and hazards. Forest has bats, wolves, bears, passive animals, wolf dens, and an optional crusher side room; fire adds ember gunners and lava; ice adds frost bats and slower ice movement. Reward artifacts can pierce actors, reflect hits, heal nearby friends, or shorten step intervals. Every gun owns its own loaded and spare ammo. Generic ammo fills each carried gun separately. Bucklers block from the facing direction until they break and can shove actors or loose items into hard obstacles. The conductor hat lays the full track before its train follows it, including through walls and the crusher.

Game state uses integer tile positions, a saved RNG, explicit snapshots and hashes, host-canonical input frames, rollback, and snapshot recovery. Directional sound and small impact particles stay local. The core loop runs at 60 ticks per second. Source files follow [AGENTS.md](AGENTS.md) and stay under 500 lines.

Presentation now keeps local sprite footprints, blood spray and puddles, zombie corpses, debris, slow parallax clouds, campfire smoke, train trails, shockwaves, tile shake, and per-actor lean and hit shake. Zombies can scratch nearby chickens as in the Rust arena. Gauche's compact in-game UI has a selected inventory row, offset health bar, selected and ground-item details, mouse cursor and target preview, and outlined Manhattan item ranges. The default 2× zoom shows the same map area as Rust's 1280×720 view on the half-size render target. Entity behavior lives under `src/entities/`, floor layout and content under `src/world/`, UI under `src/ui/`, and cosmetics under `src/particles/`.

Run the checks with:

```sh
cmake --build build-release --parallel 8
ctest --test-dir build-release --output-on-failure
```

The detailed source comparison and design decisions are in [the port plan](docs/PORT_PLAN.md). The [forest content sketch](docs/FOREST_CONTENT_IDEAS.md) is an idea pool, not a promise that every listed item is implemented.
