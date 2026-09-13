# Gauche

Gauche is a C++20, tile-step co-op dungeon run built from the original [Rust prototype](../gauche-rs). The Rust TestArena, its actors, items, train, art, and sound are the baseline. The new run adds keyed and switched exits, procedural room routes, forest/fire/ice worlds, guns, traps, artifacts, rewards, shops, lighting, and direct UDP co-op.

## Build and run

The project uses SDL3 and the pinned Gubsy host. CMake fetches dependencies when they are not available locally.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target gauche -j8
./build/gauche
```

The title menu opens a lobby for solo play, direct hosting, joining, death policy, and Gubsy's display and control settings. One player can join from each machine, up to four total. The default direct host port in the menu is 35355. A CLI host can use a chosen port:

```sh
./build/gauche --host 39000 --death next-floor
./build/gauche --join HOST_IP:39000
```

`--death` accepts `no-respawn`, `entrance`, or `next-floor`. A player who disconnects keeps their slot and loadout, can return with the same local identity, and does not block the party's exit or reward choices. Direct hosting requires the UDP port to be reachable; there is no relay or room-code service yet.

Move with WASD, aim/use with the arrow keys or left mouse, switch the six quick slots with 1–6, pick up with E, interact with F, drop with Q, and reload with R. Gameplay keys and controller buttons are editable in Gubsy's Controls screen. Stand near the exit together, clear its key or switch gate, then choose one of three rewards. Enter continues from a shop. The host can press Enter to start a new run after a loss or clear. Escape opens the in-game menu.

## Current game

Each four-floor world changes terrain, encounters, and hazards. Forest has bats, wolves, bears, passive animals, and occasional wolf dens in side rooms; fire adds ember gunners and lava; ice adds frost bats and slower ice movement. Reward artifacts can pierce actors, reflect hits, heal nearby friends, or shorten step intervals. Every gun owns its own loaded and spare ammo. Generic ammo fills each carried gun separately. Bucklers block from the facing direction until they break and can shove actors or loose items into hard obstacles. The conductor hat lays the full track before its train follows it, including through walls.

Game state uses integer tile positions, a saved RNG, explicit snapshots and hashes, host-canonical input frames, rollback, and snapshot recovery. Directional sound and small impact particles stay local. The core loop runs at 60 ticks per second. Source files follow [AGENTS.md](AGENTS.md) and stay under 500 lines.

Run the checks with:

```sh
cmake --build build --target gauche_tests gauche_rollback_tests gauche_codec_tests gauche_socket_tests gauche_session_tests gauche_loss_tests -j8
ctest --test-dir build --output-on-failure
```

The detailed source comparison and design decisions are in [the port plan](docs/PORT_PLAN.md). The [forest content sketch](docs/FOREST_CONTENT_IDEAS.md) is an idea pool, not a promise that every listed item is implemented.
