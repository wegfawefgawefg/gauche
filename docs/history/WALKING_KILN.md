# Walking Kiln and kiln courts

Implemented September 15, 2026. Industry now has eleven catalog enemy behaviors
and thirteen of fifty regional items. This encounter gives coolant, movable coal,
wooden stock and the shared fire system another practical purpose. Music remains
parked after the bounded third batch; only Shop Arrives is approved for use.

## Behavior and counters

The one-cell kiln has 160 HP and a 36-tick movement beat. After four pursuit beats
(including blocked attempts), a target within its three-cell-deep, three-cell-wide
front fan prompts a 60-tick door-opening warning. The origin and direction commit
at that point. Three rows fire outward, twelve ticks apart, each dealing eight
blockable damage and applying ordinary ignition. It cannot swivel after a dodge.
There is a 48-tick recovery, then a fresh approach/warning cycle.

Each row checks current cover before changing anything. Intact walls, crates,
grated cover and closed fixtures shelter cells behind them. A wooden wall can
catch fire at the contacted endpoint without letting heat through it. A blocking
prop also shelters an actor occupying its cell. Existing surface rules decide
what ignites, melts or stays wet; dry stone does not become permanent fire.
Physical hot-projectile damage can hurt kilns, but scorch/burning cannot. Fueled,
unquenched kilns provide shared local warmth and owned furnace ambience.

The open mouth doubles blockable damage during warning, breath, feeding and the
cooled state. Ordinary hits do not cancel its attack; cold/water does. Cooling
cancels the commitment and leaves the mouth open for 120 ticks, refreshed while
chilled or standing in wetness. Sleep, stun, airborne toss, rooting or displacement
interrupt committed actions and require a fresh warning. Fuel is preserved when
cooled. This tuning awaits player feedback.

It starts with two fuel and holds at most three. One fuel is spent when a breath
begins. Below capacity it can pause for 60 ticks to eat a real nearby Coal Lump,
crate, rotten log or twigs. Coal consumes exactly one stack unit. Wood uses normal
prop destruction, including existing contents. Interrupted feeding consumes
nothing; coal references retain generation and cell checks. Full kilns refuse
extra bait. Empty ones seek a reachable feeding stance beside stock within six
cells, or wander without inventing free breath fuel. General junk is not fuel.

Shared slots: `label_a` phase, `timer_a` phase clock, `counter_a` fuel,
`counter_b` pursuit beats or next breath row, `point_a` committed origin,
`point_b` facing or food cell, `entity_a` coal handle, `label_b` expected wood kind.
The c slots remain available for shared hearing memory. Snapshot validation checks
phase/fuel/row/food bounds. Gameplay compatibility advances to `0x2026091546`.
Cosmetic flame events, ambience, and fragments never enter gameplay hashes.

Current loot is a 15% roll for up to two remaining coal. The catalog's ceramic
plate and coal biscuit are still candidates; their rolls remain empty instead
of substituting unrelated rewards. Death scatters existing pottery and coal
fragments, not an invented additional debris quota entry.

## Native scene

Kiln Court joins Repair Bay, Cooling Works and Cable Trench as equally weighted
alternatives when the optional maintenance room is selected (two-thirds chance).
Its offset stock area has a kiln, crate, rotten log and three oil cells, with dry
margins and a Coolant Can on the other side. Whole-footprint checks reject
reserved/occupied/lava cells before mutation, try mirroring, and preserve the
room's protected central cross. Cost: three threat and one equipment budget;
failure falls back to an ordinary kiln. Incidental Industry encounters can also
place kilns. Ceramic chests and the catalog's U-shaped alcoves remain candidates.

## Assets and validation

Built-in image generation made these three transparent originals, copied without
pixel editing into `assets/graphics/`:

- `walking_kiln.png`: `exec-f43b0640-8202-436a-abad-b79c419a3442.png`
- `kiln_open.png`: `exec-1ce712f7-1f7a-44c1-ba23-53467287a1f2.png`
- `kiln_cooled.png`: `exec-f5189a63-6c63-481a-a38c-97383b01b727.png`

Original directory:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.
Base prompt: minimalist top-down pixel Walking Kiln, squat cream-grey refractory
shell, iron bands/feet, closed right-facing furnace door with orange seam, short
chimney, muted cream/charcoal/rust palette, transparent background, no floor or
shadow. First edit opens only the door to an orange/yellow mouth, preserving
body/pose/scale. Second edit extinguishes only that mouth to charcoal/cold blue
ash, preserving the open door and transparency.

Five offline effects are reproducible with `tools/sound/walking_kiln.py`:
`kiln_step`, `kiln_door`, `kiln_breath`, `kiln_feed`, `kiln_death`. Peaks are
0.23–0.36; durations 0.38–1.1s. Furnace ambience and quenching reuse existing cues.

Strict release build passed. Temporary direct checks cover the warning, row
timing, committed retreat, wood-wall ignition without leakage, crate cover,
open-mouth damage, cold/water cancellation, burn immunity, feeding/interruption,
finite stack consumption, stale handles, empty-kiln approach to solid stock,
displacement/sleep cancellation and snapshot roundtrip/malformed fuel rejection.
Protected-placement failure leaves the game hash unchanged. Across 64 generated
Industry floors, routes remained reachable and exit locks necessary; 28 kilns
and 13 complete courts appeared. SDL dummy loading covered the sprites and all
five sounds; the inspected static capture is `/tmp/gauche-kiln-court.png`.
No interactive playtest or permanent test suite was added.
