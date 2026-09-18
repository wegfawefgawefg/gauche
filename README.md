# Teeming

[Play in your browser](https://teeming.pages.dev/) · [Browser build and multiplayer](docs/engineering/BROWSER_BUILD.md)

Teeming (formerly Gauche) is a C++20, tile-step co-op dungeon run built from the original [Rust prototype](../gauche-rs). The Rust TestArena, its actors, items, train, art, and sound are the baseline. The new run adds keyed and switched exits, procedural room routes, Forest → Ice → Industrial worlds, guns, traps, artifacts, rewards, shops, lighting, and online co-op with room discovery and direct/relay connections.

The current work is tracked in one place: [the master task list](docs/MASTER_TASKS.md).

## Build and run

The project uses our Vulkan renderer on Linux, WebGPU with WebGL 2 fallback in
the browser, SDL3 for platform services, and the pinned Gubsy host. See the
[renderer guide](docs/engineering/RENDERER.md). Building requires Git, CMake
3.24 or newer, a C++20 compiler and a native build tool. The shell scripts also
require Bash. CMake fetches the pinned engine/UI sources and missing SDL
dependencies; the first build needs internet access. Native platform development
libraries may still need installing. The UI needs FreeType and HarfBuzz, supplied
by dependency targets or system development packages through pkg-config.

On a recent Debian/Ubuntu-based Linux installation, install the native build,
font, window and audio dependencies once:

```sh
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config git \
  libglm-dev libfreetype-dev libharfbuzz-dev libcurl4-openssl-dev \
  libx11-dev libxext-dev libxcursor-dev libxi-dev libxfixes-dev \
  libxrandr-dev libxrender-dev libxss-dev \
  libwayland-dev wayland-protocols libxkbcommon-dev libdecor-0-dev \
  libasound2-dev libpulse-dev libpipewire-0.3-dev \
  libdrm-dev libgbm-dev libudev-dev libgl1-mesa-dev libegl1-mesa-dev \
  libvulkan-dev mesa-vulkan-drivers
cmake --version
```

CMake must report at least 3.24. Older distributions may need a newer toolchain;
other Linux distributions use their equivalent development packages. SDL3 is
fetched automatically if its complete system dependency stack is unavailable.
Lua is not needed for Gauche's build. This package list follows the engine's
Linux setup plus Gauche's font dependencies; a clean-machine install has not
been verified here.

Builds use published, commit-pinned Gubsy, GView and GLayout sources by default,
even if other checkouts exist beside Gauche. Engine/UI development can opt into
those sibling checkouts with `-DGAUCHE_USE_LOCAL_DEPS=ON`; the `GAUCHE_GUBSY_DIR`,
`GAUCHE_GVIEW_DIR` and `GAUCHE_GLAYOUT_DIR` CMake paths then select them.

If an older checkout failed with `Failed to checkout tag` for GView, update Gauche
and run the build again. The previously pinned commit had not been published;
the corrected pins are available remotely. No game source edits or switch to a
random GView revision are needed:

```sh
git pull --ff-only
./scripts/run.sh
```

Clone the repository, then build and launch from its root:

```sh
git clone https://github.com/wegfawefgawefg/gauche.git
cd gauche
./scripts/run.sh
```

If already cloned, just run `./scripts/run.sh`. It configures CMake, builds the
Release `gauche` target incrementally, and launches only if the build succeeds.
It builds the **current local checkout**; it does not download new game commits.
To update an unmodified checkout and play the latest pushed version:

```sh
git pull --ff-only
./scripts/run.sh
```

These are Bash commands, not native PowerShell commands. Windows needs a suitable
C++ build environment as well as Bash; merely installing Git Bash is insufficient.

Set `GAUCHE_PRESET=dev` for a debug build. `./scripts/build.sh` builds without launching. Both scripts work from any current directory; `run.sh` passes game arguments through unchanged.

The complete menu flow uses GView and the stone controls from the Splonks C++ menu workspace. Play opens Gauche's GView lobby for player setup, death policy, public room hosting, browsing/joining, and starting a run; Quick Run starts alone. Display, audio, control profiles, button and analog mappings, and input tuning also use GView screens backed by Gubsy's settings and input systems. One player can join from each machine, up to four total. The default direct host port in the lobby is 35355. A CLI host can use a chosen port:

```sh
./scripts/run.sh --host 39000 --death next-floor
./scripts/run.sh --join HOST_IP:39000
```

`--death` accepts `no-respawn`, `entrance`, or `next-floor`. A player who disconnects keeps their slot and loadout, can return with the same local identity, and does not block the party's exit or reward choices. For internet play, use **Play → Host Game** to publish a room and **Play → Join Game** to browse or enter its code. Automatic connections try a direct path and can fall back to the room service's relay; no port entry or forwarding is required for that flow. The direct CLI examples above require a reachable UDP endpoint.

Move with WASD; aim and use with the arrow keys or left mouse button, or use the held item with Space. On a controller, move with the left stick or D-pad, turn with the right stick, use with the right trigger, and cycle quick slots with the bumpers. Select one of six quick slots with 1–6 or the keypad, pick up with E, interact with F, and reload with R. Q/Tab or controller Y opens the inventory; E/Delete or controller X drops its focused item. C or right-stick click compares a reward or a ground item with any inventory slot. Equipping a slot keeps the inventory open. V or controller Select/View toggles the selected and ground-item cards between detailed and compact views. Hold Alt to hide item names and description cards while keeping counts and cooldowns visible. The mouse wheel or `-`/`=` changes zoom. Gameplay keys and controller buttons are editable in Gubsy's Controls screen, and its audio levels control Gauche's music and directional effects. Stand near the exit together, clear its key or switch gate, then choose one of three rewards. Enter continues from a shop. Escape opens the in-game menu.

## Current game

The current run has twelve floors: four Forest, four Ice, then four Industrial. Forest has twenty enemy types, passive animals, breakable woodland props and fifty-two regional items. Ice has twenty enemy types and fifty regional items built around slippery ground, cold water, heat, optics and machinery. Industry has excavation crews, machinery installations and lava hazards; its scene dressing and the fourth biome remain in development. Fire tools carried from Forest remain useful in Ice, and cold tools can carry onward into Industrial. Reward artifacts can pierce actors, reflect hits, heal nearby friends, or shorten step intervals. Rare reward item attributes change damage, cooldown, reach, blast shape, healing, or condition. Every gun owns its own loaded and spare ammo. Generic ammo fills each carried gun separately. Dropped items and generated loot spread to the nearest free walkable tile when their intended tile is occupied. Bucklers block from the facing direction until they break and can shove actors or loose items into hard obstacles. Bear traps must be opened, then placed; they deal 100 damage and leave closed jaws to recover. The conductor hat lays the full track before its train follows it, including through walls and the crusher.

Game state uses integer tile positions, a saved RNG, explicit snapshots and hashes, host-canonical input frames, rollback, and snapshot recovery. Directional sound and small impact particles stay local. The core loop runs at 60 ticks per second. Source files follow [AGENTS.md](AGENTS.md) and stay under 500 lines.

Presentation keeps local sprite footprints, blood spray and puddles, zombie corpses, debris, slow parallax clouds, campfire smoke, train trails, shockwaves, tile shake, and per-actor lean and hit shake. Zombies can scratch nearby chickens as in the Rust arena. The compact in-game UI shows stack counts, condition, uses, magazine ammo, cooldown bars, item details, grid-based attack patterns, and a directional world preview. Entities, items, and roof fixtures own their light color, strength, and radius in saved state; ambient openness and those colored sources light all four corners of terrain tiles, actors, world sprite particles, and the exterior border. The default 2× zoom shows the same map area as Rust's 1280×720 view on the half-size render target. Entity behavior lives under `src/entities/`, floor layout and content under `src/world/`, UI under `src/ui/`, lighting under `src/lighting/`, and cosmetics under `src/particles/`.

Run the checks with:

```sh
cmake --build build-release --parallel 8
ctest --test-dir build-release --output-on-failure
```

The detailed source comparison and design decisions are in [the port plan](docs/engineering/PORT_PLAN.md). The [forest content sketch](docs/design/FOREST_CONTENT_IDEAS.md) is an idea pool, not a promise that every listed item is implemented.

`build-release/gauche --audit-generation > /tmp/gauche-generation.csv` generates 64 ordinary floors each for Ice and Industry, without opening a window or advancing gameplay. The CSV lists planned rooms, installation/encounter outcomes, budgeted enemy and supply attempts, rejected placements, fallbacks, and actual entity/ground-item counts. Entity IDs refer to `EntityKind` in `src/game.hpp`; scene-internal drops are included in actual counts, not the room-supply attempt counters. The command also checks repeatability, required routes/locks and snapshot round-trips; it exits with an error on a failed check.

For a controllable host plus three local bots, run `./scripts/multiplayer.sh`.
See [the local multiplayer workshop](docs/guides/LOCAL_MULTIPLAYER.md) for i3 layouts,
headless clients, isolated profiles and logs. Normal internet play uses
**Play → Host Game** / **Play → Join Game**, with no port entry required.
