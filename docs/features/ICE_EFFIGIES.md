# Snow effigies and memorial courts

## Implemented behavior

- Snow effigy: 60 HP, eighteen-tick ordinary steps, dormant until first observed
  or disturbed. Actual damage, sleep/stun effects or heat can also awaken it.
- Any connected living player who is awake can hold it still with a clear
  ninety-degree facing cone, within seven cardinal-distance tiles. Sleeping
  players cannot observe; stunned players can still look. Walls, blocking props,
  closed fixtures, smoke and whiteout interrupt the shared sight query.
- Every co-op observer counts. Observation depends only on deterministic
  positions, facing and status, never camera, zoom, cursor graphics or local
  lighting. Looking away permits motion; looking back cancels a pending strike.
- Once awakened, seeks a visible player or straw decoy within seven tiles.
  An adjacent target gets a fixed thirty-tick tell: lit twig face and creaking
  wood. Resolution deals sixteen damage in that cell, then recovers 54 ticks.
  Sidestepping avoids it; guard and reflection keep their ordinary rules.
- Damage, sleep, stun or displacement interrupts a windup. Rooting prevents its
  committed strike. No turn or movement occurs while a player observes it.
- Exposed flame, burning or a hot capsule patch permanently removes the snow
  shell. Maximum HP becomes eighteen, current HP is capped at eighteen without
  healing, and the enemy recovers for 45 ticks. It keeps the same gaze rule.
  Thermal state updates even during sleep/stun; cold does not rebuild the shell.
- Shared slots: label_a phase, label_b awakened, timer_a phase; counter_a prior
  observation, counter_b exposed frame; point_a stance, point_b strike cell.
  No per-client state or new snapshot fields. Protocol advances to F9.

## Rooms and presentation

- Memorial courts enter the Ice room pool: courtyard layout, bare slate center,
  snow edging, existing protected routes. One effigy costs two threat; later
  rounds may add an Echo Hound. Heat capsules use the room's equipment budget.
- Four original 16px sprites: snow shell/frame, each resting and warning.
  Seven offline cues: wake, stop, creak, warning, strike, thaw and frame break.
- Thawing sheds local snow clumps; death leaves twig litter, not blood. Creaks,
  warnings, hits and death can draw nearby hearing enemies within seven tiles.
- Static pose scene `effigies` demonstrates all four silhouettes and litter.

## Still open

- Candle Stub now supplies the first 20% of the planned single drop roll; see
  [candles](ICE_CANDLES.md). Effigy Mask remains unimplemented: reserve the next
  10% of that roll when its behavior and item exist. No substitute reward.
- The mask needs a useful additional observer behavior, not a redundant charge
  for the player's existing free gaze. Preserve this intent when implementing it.
- Player playtesting owns the gaze boundary, timings and balance. Ice content
  and the master goal remain incomplete.

## Validation

Strict game/render/codec builds, existing codec check, source/asset constraints,
finite unclipped audio and a static pose capture. No live playtest or new suite.
