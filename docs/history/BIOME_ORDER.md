# Forest → Ice → Industrial progression

Runtime floors are Forest 1–4, Ice 5–8 and Industrial 9–12. The fourth biome still
needs its catalog and integration; the current run finishes after floor 12.
This replaces the old Forest → Fire → Ice order so fire carried from the woods
is useful in the next zone, followed by Ice tools carried into the hot zone.

`src/biome.hpp` owns biome identity, first floor, stage number, display name and
implemented run length. Generation, fallback spawners, terrain atlas/tint, cold
wall contours, smoke weather, shops, rewards, HUD and stage announcements follow
those identities. Ice room/enemy/item pools now occur in the middle four floors;
Industrial retains the existing lava/ember content while its full catalog is
unfinished. Forest behavior and the Rust arena's zombie spawner remain intact.

Debug level lists and representative entry loadouts follow the new order. Saved
playtest settings move from format 1 to format 2: old Fire selections map to
Industrial and old Ice selections map to Ice. Selected row, starting override
and repeat target migrate independently, while actual saved item loadouts stay
unchanged. Haunted House retains its Forest stage and selector identity.

Gameplay version is `0x2026091520`, snapshot version 39. Old snapshots are rejected
because their floor numbers imply different biomes. The field layout itself is
unchanged. All multiplayer peers must use the same gameplay version.

Validation: release game/static renderer builds; direct generation of all twelve
stages at seed 1729 with matching repeat hashes, snapshot round trips, reachable
objectives and required locks. Named cold-wall/lava placement and announcements
match each region. Direct transition checks preserve torch condition, carried
cold flasks and gold across floors 4→5 and 8→9; floor 12 still reaches the win
phase. Saved debug v1→v2 migration and reloading v2 preserve all three selections.
The arena spawner still releases zombies. Static Ice, Industrial and debug-level
captures confirm the region palette and selector order. The final build uses the
published dependency pins. No live playtesting.
