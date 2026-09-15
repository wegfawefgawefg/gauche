# Mold Thief and casting floor — 2026-09-15

Industry now implements eighteen of twenty catalog enemy behaviors and twenty-two
of fifty regional items. This slice adds an optional casting-floor encounter,
Mold Thief, reusable breakable molds and an eight-use Mold Key. It makes loose
steel gear part of an encounter without taking equipment from player inventories.

## Encounter and actual cargo

The 58-HP thief searches within eight cells for reachable loose steel gear, using
the existing magnet material predicate. A 0.3-second tongs tell precedes pickup;
the stored generation-safe handle must still name that item on that cell. Flying,
anchored, burning and sled-loaded items are excluded. The whole real Item/stack
moves into the thief's existing `ground_item`: magazines, reserves, condition,
uses and modifiers survive. It moves faster while carrying, seeking a reachable
empty mold within twelve cells.

Sealing takes three seconds from a fixed adjacent stance. A progress bar, rocking
body and one-second crank cues expose the action. Two thieves cannot overwrite
one mold. Damage spills cargo and makes the thief flee for three seconds;
cold, root, sleep, stun and toss interrupt a grab or seal. Displacing it during
one spills its cargo. A full entity pool leaves cargo on the live carrier.

Molds have sixty HP and block bodies. Sealing closes the lid; the key opens it
without damage. Normal adjacent pickup/swap retrieves the actual contents, and
an emptied open mold can be reused. Breaking either carrier or mold returns its
cargo in the dead body's own slot with a new generation, guaranteeing recovery
even when the pool is full. Full inventories and failed item release leave
contents accessible for another attempt. Personal thief loot is separate: 20%
Mold Key; the reserved Foundry Tongs range stays empty until that item exists.

The key costs eight gold, has eight uses (sixteen with Durable), responds to
magnets, and appears in native stage-one supply/reward/cache/secret/workshop
selection. Invalid/open targets cost nothing. Breaking a mold remains a universal
alternative, so carrying a specialist key is optional.

## Placement and implementation

The initial optional room variant places two offset workstations with four short
grate partitions, one thief, a ten-use Pocket Drill and a key on the dry side.
Its protected central route remains clear; all placement cells and five free
entity slots are checked before mutation. Costs: two threat, two equipment.
It is the tenth maintenance-room choice. The catalog's larger connected yards
and tongs rack remain pending; this is not a claim of a full multi-room prefab.

`entities/mold_thief.*`, `entities/casting_mold.*`, `entities/mold_render.*`,
`items/mold_key.*` and `world/casting_floor.*` own their respective behavior.
The thief reuses shared phase/timer/point/handle fields; the c fields remain for
hearing. Rare molds use Entity storage because they own actual gameplay cargo;
ordinary decoration stays in compact Props. No new entity wire fields.
Snapshot validators constrain phases, clocks and mold state. Gameplay version
is `0x2026091561`.

## Assets and checks

Four original transparent sprites were generated with the built-in image tool
and copied without alpha removal to `assets/graphics/mold_*.png`. Sources under
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:

- Thief: `exec-a4e2977f-9f0a-49de-8523-41f6ab24afa3.png`; hunched foundry scavenger,
  soot apron, rust cap, long steel tongs, pale nose, sparse top-down silhouette.
- Open mold: `exec-d58165f7-c021-44cc-b0a2-04c0bb342803.png`; squat iron clamshell,
  raised lid, dark casting cavity, brass latch, small feet.
- Closed mold: `exec-9a635254-27f6-4f01-aadb-c6d4201af1d3.png`; edit of the inspected
  open mold, same base/palette with heavy lid clamped shut.
- Key: `exec-9b005944-d2fe-480e-ab1b-412ed77f365d.png`; short machinist key,
  squared loop, stepped steel tooth, brass band, diagonal icon.

Ten original metal/grunt sounds come from `tools/sound/mold_thief.py`, normalized
peaks 0.20–0.38. Mold breakage adds local chain-link debris. No music changes.

Strict release build and temporary direct checks passed: delayed exact theft,
stale item/mold handles, full sealing duration, competing deposits, successful
and refused key uses, adjacent pickup, control/damage/displacement interruption,
full-inventory/pool refusal, guaranteed full-pool death salvage, mid-seal snapshot
continuation and invalid-clock rejection. Sixty-four generated Industry floors
retained reachability and required locks; three included casting thieves.
All ten sounds loaded under SDL dummy audio. The static render was inspected;
the clipped first description was shortened. No interactive playthrough or
permanent test suite. Player feedback and broader room variety remain pending.
