# Icicle spiders and service passages

## Ambush

- Icicle Spider has 34 HP, nine-tick steps, and a 27-tick visible bite warning
  before a fixed adjacent 12-damage strike. Recovery lasts 54 ticks. Targets
  moving away from the committed cell avoid the bite; shields work normally.
- It searches within five Manhattan tiles for a reachable perch beside a wall.
  A strand needs another real wall across two to six open floor cells. Searches
  and pathfinding are bounded. It walks to the perch, then weaves for 60 ticks.
- Weaving requires clear, dry, unoccupied cells. It does not hide a new trap
  underneath a stationary player or loot, overwrite another prop, or cover an
  existing spider's reservation. An unreachable site causes a delayed rescan.
- The first ground creature crossing the completed strand consumes the whole
  line and receives one second of rope root. The spider remembers that creature
  by generation-checked handle and pursues it for up to three seconds while it
  remains in sight. This includes other enemies; spiders ignore spider silk.
  Airborne actors do not trigger the ground strand. Standing still cannot repeat it.
- An adjacent player or decoy can provoke a bite even without a usable weave site.
  The spider releases its own line before committing that attack, so bite target
  coordinates cannot accidentally move or redraw a strand.

## Shared world interactions

- Each silk cell uses the existing eight-byte prop structure: one HP, nonblocking,
  no inventory, no actor slot, no independent thinker. The spider stores its two
  wall anchors and whether the strand exists in its shared entity fields.
- Any damaging prop hit cuts the entire line immediately. This covers ordinary
  melee, arrow travel, physical thrown weapons, beams and blast prop damage.
  Shots may keep traveling through the now-cut, nonblocking silk.
- Ignition burns silk immediately without leaving a perpetual fire on bare floor.
  Actual flame contact also burns it. Water alone does not cut it. Breaking an
  anchor, replacing a strand cell, moving its spider away, killing it or removing
  it invalidates the old span and clears its remaining cells.
- Damage interrupts unfinished weaving and bite tells. Sleep, stun and roots
  interrupt those phases in the timer phase. An already finished line can catch
  something while its spider is asleep; all line cleanup still runs during sleep.
- A cut or invalid line causes 90 ticks of recovery before rebuilding. A trigger
  causes hunting instead. Rebuilding always spends the full weaving time.
- `label_a` stores phase, `counter_a` owns the span, `timer_a` handles work and
  recovery, `timer_b` handles hunting/travel. `point_a/b` store anchors, then the
  fixed stance/target only after the line is gone. `entity_a` stores caught prey.
  Shared hearing fields remain available. No new per-enemy struct or function table.

## Content and validation

- Service passages are a fourteenth Ice room role: dry stone galleries with narrow
  wall-to-wall alcoves and the existing protected central route. One spider costs
  two threat; later regional rounds can supply a second. A lighter is eligible
  for the room's equipment budget, giving another way to dismantle strands.
- One death drop roll: 20% Fishing Line, next 10% Ice Needle, otherwise nothing.
- Six original 16x16 sprites: body, weave, bite, recovery, horizontal and vertical
  silk. Eight offline-generated sounds cover weave, set, cut, burn, catch, bite
  warning, bite and death. Local rope fibers/ice pieces accompany breaks/death;
  these reuse existing debris artwork and do not claim a new material quota.
- Ice now has 19 implemented enemy behaviors, 29 items, 18 debris materials and
  14 room roles. Boiler Porter and other unfinished catalog tasks remain open.
- Protocol FF; snapshot layout 34. Existing codec fixture retains woven props,
  anchors, phase and a prey handle. Strict game/render/codec builds, existing codec
  checks, static intact/caught captures, sprite/audio checks and diff checks.
  No live playtest or new simulation test suite; user playtesting owns balance.
