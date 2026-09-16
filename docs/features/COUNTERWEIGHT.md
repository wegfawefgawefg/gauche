# Counterweight and hoist shaft — 2026-09-15

Industry now has sixteen of its twenty catalog enemy behaviors. The Counterweight
is a 140-HP anchored chain winch with a suspended stone block. It acquires a
visible player or straw decoy within five tiles, travels overhead for 0.5s,
marks one fixed cell for 0.8s, and drops over eight ticks. The block hits whoever
occupies that cell for 48 damage and props for 120, then rewinds for two seconds.
Moving away dodges it; another creature entering the mark can be crushed instead.
The shadow, marked landing and descending block remain visible without debug
attack overlays. The anchored winch is the damageable part.

Cold, control effects, displacement or an actual health hit of at least 12 cancel
the commitment. Small rapid hits do not permanently suppress it. Rewinding
preserves the interrupted height/travel fraction. Solid cover blocks acquisition;
cover placed under an already committed drop can be broken by the impact.
It never creates another hit when winding up or dying.

Destroying the machine leaves an 80-HP metal-cuttable wreck at the old blocked
footprint. It does not create a new blocker underneath the marked victim or
overwrite existing props. Native debris combines chain links and basalt chips.
One death roll gives a 25% Chain Hook; the planned Counterweight Bag range stays
empty until implemented. Wreck placement precedes loot placement so a dropped
hook can land on accessible neighboring floor.

## Encounter and implementation

An optional hoist shaft joins the nine maintenance-room choices. A side pocket
contains the winch, pressure rat, scrap/ore bins and eight real coins within the
winch's reach. Coolant and a Chain Hook on the margins allow freezing the motor,
hauling loot or pulling toward a fixed anchor. Protected central paths remain
clear. Full footprint validation, mirrored fallback and five free entity slots
precede mutation. Budget cost: three threat and two equipment. The more elaborate
ring/pressure-plate cache layout remains a separate unimplemented variant.

The enemy uses shared phase, clock, two cells and two rewind-fraction counters.
No extra entity per overhead component and no serialized fields were added.
Snapshot validation bounds phase clocks, fractions and reach. Gameplay version
is `0x2026091557`. The room-space search and budgeted spawning helpers moved intact
to `world/room_supplies.*`, keeping room population below 500 lines.

## Assets and verification

Built-in image generation produced two original transparent sprites, copied to
`assets/graphics/counterweight.png` and `assets/graphics/weight_block.png`.
Originals remain under
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:

- `exec-e0b2255c-8309-4f0c-b839-1da61a1b8799.png`: squat chain winch.
- `exec-b73b06f6-c438-4761-b898-1e16d2b43ab1.png`: banded stone block and hanging ring.

Prompts requested isolated minimalist top-down pixel sprites, muted charcoal,
rust and ivory, broad angular shapes, genuine transparent alpha, no scenery,
text, glow or cast shadows. The first separates winding machinery from its
weight; the second is a single squat stone mass with two iron bands and a ring.
`tools/sound/counterweight.py` synthesizes five original ratchet/strained-chain/
impact/rewind/break OGGs, with peaks 0.17–0.40. No music changes.

Strict release build and temporary direct checks passed: complete warning and
single impact, dodging, cover destruction and creature replacement, blocked
acquisition, weak/heavy damage, cold and displacement cancellation, decoys,
full cosmetic buffers, mid-drop snapshot continuation, malformed-state rejection,
safe wreck placement/cutting and protected footprint refusal. Sixty-four Industry
floors retained reachable routes and required locks; five hoists appeared.
Both sprites and `/tmp/gauche-hoist-shaft.png` were visually inspected; the
SDL dummy render loaded all five OGGs. No autonomous playthrough. Live balance
and readability still need user feedback.
