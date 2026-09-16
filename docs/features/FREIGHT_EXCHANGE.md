# Freight Exchange

Implemented 2026-09-15. A whole-floor Industry unique that combines the new
machinery and equipment into places to use them. This addresses the feedback
that later biomes need more substantial locations and useful interactions.

## Layout and encounter

Industry 3-2 has one seeded one-in-five selection. Other stages do not roll it,
so it appears at most once per normal run. Explicit Generated layout opts out;
the dev selector can force it. A 64-by-48 authored map has two loading halls,
side passages, a lower sorting area and a required control booth switch.
The exit vestibule has a single door through an unbreakable quest seal.
Other interior walls remain diggable; dry routes do not require supplied gear.

- Two Rail Shunters start loaded carts on opposite lines. Working switch points
  connect those lines; the supplied key can redirect the same physical carts.
  Cargo is an actual 12-condition Press Hammer and three Bolt Pouch uses.
- A Magnet Crane has worn six-use Pickaxe salvage nearby. A supplied Chain Hook
  can recover salvage or move crates and carts; a Rubber Mallet offers shoves.
- A manual sorting belt carries a Nail Board toward its hot end. The crank is
  supplied nearby, within hearing range of an Ash Sleeper. Quiet retrieval is
  also possible; the sleeper does not require a scripted encounter trigger.
- A Rivet Gunner guards the booth among shoot-through grates. A Counterweight
  sits over a separate coin cache. Scrap bins, an ore bin and a crate give
  cover and use their existing thematic drop tables.
- Six breakable lamps identify work areas. The small lava sorting corner gives
  warm contrast. Existing machinery sounds and sprites are reused.

The whole party uses the existing living-online-player exit gathering rule.
The usual reward and shop resolve before Industry 3-3; no custom progression
or party-sealing state machine is introduced. The Last Shift reactor escape,
reserved multi-room landmarks and broader unique/boss quotas remain unfinished.

## State and debug integration

Actual floor layout is now explicit in Run, serialized, hashed and validated.
Automatic is a generation request only and is rejected as runtime state.
Stage announcements use that saved identity, rather than inferring a haunted
floor from an active encounter. Gameplay compatibility is 0x2026091568.

Freight Exchange is appended to ImGui Levels; saved indices are clamped against
the real list size. Existing selections retain their indices. Start overrides
and repeats use the existing tools. A fresh debug start on a later floor now
keeps default player health and starter equipment instead of copying zero
health from a nonexistent prior player.

The freight rail renderer also now distinguishes finite freight tracks using
current HP. Legacy train rails have zero HP but a default nonzero maximum;
they retain their original sprite instead of incorrectly using freight ties.

## Validation

Strict release build and temporary direct checks passed: required-switch route,
walkable spawn/unique solid occupancy, fresh later-floor start, exact four-player
health/equipment/gold carryover, actual switch and door use, split-party exit
refusal, gathered exit/reward/shop/next-floor progression, redirected moving
cargo, snapshot/hash replay, saved dev selection and layout announcements.
Sixty-four seeded automatic Industry 3-2 floors yielded sixteen Exchanges;
all retained reachability, required locks and matching snapshot roundtrips.
Neighbor stages and forced Generated opt out; forced Haunted House still works.
Static overview and normal-zoom junction renders were inspected after tightening
the layout and adding real lamps. No interactive playtest or permanent suite.

Cart timing, encounter pressure, light levels and amount of loot still need
player feedback. This does not count as finishing the remaining Industry item,
debris or ambient catalogs.
