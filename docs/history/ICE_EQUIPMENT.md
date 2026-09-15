# Ice footing equipment

## Skate blade

- Detached steel runner, 16 gold, 12 base damage, 0.4s cooldown, four-tick
  windup and 36 repairable condition. Every completed swing wears one condition,
  including a missed swing. No digging power; glue repairs it normally.
- Sweeps the three cells across the row immediately ahead. A successful ice or
  oil slip grants twelve ticks of momentum (0.2s). During that window the central
  lane can reach one extra forward cell, subject to cover and the normal first-hit
  stop. Walking another ordinary step clears the old momentum; slipping again
  refreshes it. A blocked slip gives no bonus.
- The actual impact reads remaining momentum. Starting a swing is not a promise
  that the bonus will survive until impact. No entity interpolation or extra slide
  movement was introduced; all contacts remain at authoritative cells.
- Each actor is hit at most once per swing. Physical sight cuts the side cells at
  corners; a central enemy or wall stops the extra tip unless the attack pierces.
- Big widens the sweep to five cells. Long adds a row of reach. Strong, Heavy,
  Agile, Fragile and Durable keep their existing damage/time/condition effects.
- Item diagrams reserve the conditional tip cell and draw it outlined when inactive,
  filled during momentum. World debug previews use the same active pattern. The
  status HUD shows a short slide-reach timer while the blade is held.
- Rime skaters now have their full planned single drop roll: 20% grit pouch,
  the next 10% skate blade, otherwise nothing.

## Crampons

- Seventeen gold, six activations (Durable twelve), nonstackable. Activation gives
  five seconds of traction and a 0.75s item cooldown. Rejecting an already active
  effect or existing Sticky Boots grip consumes nothing.
- Traction suppresses ice slipping and doubles normal movement recovery. It does
  not resist shoves, oil slips, roots, enemy damage or hazards. Chill still applies
  its own slowdown. Sticky Boots and crampon slowdown do not multiply together.
- Activating clears old slide momentum. The timer outlasts switching/dropping the
  tool; the final activation still gives its complete duration before the exhausted
  bindings disappear. The HUD shows time remaining; the detail card previews the
  player's actual movement beat before and after activation.
- Cliff paths alternate crampons and Wool Wrap equipment within the floor budget.
  Both new tools are available in the regional shop and end-of-floor reward pools.

## Integration and verification

Two new 16x16 item sprites and six original offline sounds: slash, edge contact,
runner break, binding fit, release and exhausted spikes. Loud contact/fit actions
use the shared hearing rules; local playback does not influence AI.

`VitalEffects` owns the short momentum and traction timers, shared by actors rather
than borrowing species AI slots. Both count down in the vital-effect step, reset
on death, and are serialized and hashed. Snapshot layout is 35; gameplay protocol
is `0x2026091502`. The existing codec fixture preserves nonzero timers, a worn Big
blade and partially used Durable crampons.

Strict game/render/codec builds, the existing snapshot round-trip and static item
captures are the validation for this slice. No live playtest or new test suite.
The initial timing, damage and feel remain subject to the user's playtest feedback.
Ice now has thirty-three regional items. Seventeen more, the last debris material,
unique floors and remaining biome work are still outstanding.
