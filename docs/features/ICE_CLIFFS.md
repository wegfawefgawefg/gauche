# Avalanche rams and cliff paths

## Implemented enemy 15/20

- Avalanche ram: 76 HP, twenty-tick ordinary steps. Seeks a visible player or
  decoy within six tiles; hearing can redirect it between attacks.
- Only starts its headbutt within two cardinal cells. Pawing lasts 36 ticks,
  then it traverses at most two cells at six ticks per cell, with a fixed heading.
  No turning or distant charge after the player sidesteps the tell.
- Hits the first impassable actor for 12 damage and shoves it one cell. Existing
  crush rules apply against solid terrain/anchored blockers; ordinary teammates
  do not become crush walls. Allies can be hit. An active facing buckler blocks
  the blow, pays normal durability and catches the horns without being shoved.
- A wall, surviving solid prop, anchored actor or guarded blow stuns the ram
  for 75 ticks, followed by the remainder of its 90-tick recovery. A normal
  headbutt or missed lunge recovers for 48 ticks.
- Clears snow crossed by the lunge, revealing burrowers/caches, and shatters
  temporary ice-block props. It does not tunnel through rock or timber.
- Damage, sleep, stun, rooting or displacement cancel its committed attack.
  Springs, slips and portals keep their actual landing and terminate the lunge.
  Reflected damage can kill the ram; no post-death stance is applied.
- Shared state: label_a phase, timer_a phase clock, counter_a remaining lunge;
  point_a expected cell, point_b heading. c-slots remain available for hearing.
- One death roll: 35% two raw meat, 10% Wool Wrap, otherwise nothing.

## Room and presentation

- Cliff paths join the Ice room pool. Stepped outlines and broad snow banks
  surround a dry central shelf; the existing protected route remains passable.
- One ram costs two threat; later rounds may add a snow burrower for one.
  Wool Wrap joins the room's equipment pool. Existing placement and floor
  budgets still apply; no mandatory enemy spawn into occupied cells.
- Four original 16px poses: roaming, pawing, lowered horns and dazed recovery.
  Six offline-generated cues cover paw/bleat, rush, hit, bonk, plough and death.
  Snow ploughing sheds existing local snow clumps; death sheds wool tufts.
- The optional debug attack overlay reads the committed two-cell lane. Pawing,
  bonks and impacts are heard by investigating enemies at radius seven.
- Added missing Weather station room label while extending the names table,
  and a compile-time check tying its length to the last role.
- Protocol F8; no saved field or snapshot layout change.

## Validation

Strict builds, the existing codec check, original audio/sprite checks and a
static four-pose render. No live playtest or new test suite; feel and balance
remain open to user feedback. Ice and the master goal remain incomplete.
