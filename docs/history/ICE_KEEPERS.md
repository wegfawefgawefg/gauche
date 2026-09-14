# Candle keepers and chapel alcoves

## Behavior and counterplay

- Candle Keeper: 56 HP, eighteen-tick steps. Home-centered candle search uses a
  bounded flood, respecting walls, closed fixtures and bodies. Dry, unlit fueled
  wicks take priority over the nearest lit candle. Exhausted stubs stay exhausted.
- Reaches an adjacent wick, tends it for 36 ticks, then lights it. Does not refill
  fuel or invent a new candle. Water, theft, breakage and displacement can prevent
  completion. Damage/sleep/stun cancel work and impose 45 ticks of recovery.
- Guards a two-tile area around its chosen candle (home if none). Passers-by
  outside that area are not automatically hunted. Visible decoys can distract it.
- Witnessing a candle stolen within seven tiles and its eight-tile territory
  records the thief's generation-checked handle for five seconds. Closed sight,
  smoke and sleeping keepers prevent witnessing; real damage also records its
  attacker when the source cell identifies one. No unobserved player tracking.
- Pursuit stays within nine tiles of home. A displaced keeper can return home;
  blocked paths consume a movement beat rather than endlessly probing a wall.
- In a cardinal lane within two tiles, winds up a flame cast for 36 ticks. Fixed
  stance and facing determine the two affected cells. Sidestep, shield, cover,
  damage, sleep, stun, root or forced movement provide counterplay.
- Cast deals ten blockable damage and, only after actual damage on a dry living
  actor, 120 ticks of scorch (up to eight additional damage). Friends and enemies
  in the pattern are affected. Cover is sampled before destruction; breaking the
  front prop cannot expose the next cell in that cast. Dry fuel can ignite.
- Casting extinguishes its own lamp for 180 ticks, with 54 ticks recovery. Water
  keeps it snuffed while occupied; cold-flask quenching also suppresses it. The
  dark keeper cannot cast or relight candles until its lamp returns.
- Lamp heat and colored light use the existing shared source rules. Lamp updates
  run during sleep/stun, so wet keepers do not retain invisible flames. Steam
  leeches can feed on the lamp, and nearby heat-dependent creatures react to it.
- Pilgrim warmth searches now include placed candles, matching the warmth they
  already receive on arrival. Player/entity positions remain authoritative cells.

## Rooms, loot and presentation

- Chapel joins Ice's room-role pool with a gallery floor and candle alcoves.
  Up to three lamps and a cabinet occupy suitable off-route cells. Required
  crossings and spawn clearance are preserved; small/occupied alcoves may omit
  the cabinet. Keeper costs two threat, with a later-round effigy costing two more.
- Brine Flask supplies use the ordinary room equipment budget: a way to extinguish
  candles or the keeper. Candles remain recoverable by ordinary pickup/swap.
- Keeper drops use one roll: 30% fresh Candle Stub, next 15% Wick Spool, else empty.
- Candle cabinet: 18 HP, blocking, burnable wood. One break/open roll: 35% candle,
  20% wick, 15% wool wrap, 30% empty. Leaves local wood scraps and cloth.
- Four original 16px body poses: lit, tending, warning and dim. The body remains
  visible during attacks. Original cabinet sprite; nine offline cues distinguish
  muttering, matchwork, warning, cast, fire, dousing, relighting, death and cabinet.
  Fire casts emit local flames in their affected cells; death sheds wax and wool.

## State and validation

Shared slots are documented next to behavior: point_a home, point_b guarded
candle, label_b known candle; label_a phase, timer_a phase, timer_b lamp recharge;
counter_a/b stance; entity_b culprit, attack_wait grudge. c attention slots stay
available for hearing. No new snapshot fields; protocol advances to FB.

Strict builds, existing snapshot roundtrip with a dim keeper remembering a
player and a damaged cabinet, asset/audio checks and static `keepers` capture.
No live playtesting or new suite. Player feedback still owns behavior balance.

Ice now has seventeen enemy behaviors, twenty-seven items, seventeen debris
materials and twelve room roles. Shard Colony, Icicle Spider, Boiler Porter,
remaining items/ambience and the rest of the master goal remain unfinished.
